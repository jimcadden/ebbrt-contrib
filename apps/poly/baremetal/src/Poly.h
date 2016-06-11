#ifndef APPS_POLY_SRC_POLY_H_
#define APPS_POLY_SRC_POLY_H_

#include <ebbrt/EbbAllocator.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/Message.h>
#include <ebbrt/StaticIds.h>
#include <string>

namespace ebbrt {

class Poly {
public:
  class Root {
  public:
    virtual Poly &HandleFault(EbbId id) = 0;

  private:
    friend class Poly;
  };
  static Poly &HandleFault(EbbId id) {
    LocalIdMap::ConstAccessor accessor;
    auto found = local_id_map->Find(accessor, id);
    if (!found)
      throw std::runtime_error("Failed to find root for Poly");

    auto root = boost::any_cast<Root *>(accessor->second);
    return root->HandleFault(id);
  };
  virtual void Foo() = 0;
  virtual void Bar() = 0;
};

class PolyFoo : public Poly {
public:
  PolyFoo(){};
  static EbbRef<PolyFoo> Create(EbbId id = ebb_allocator->Allocate()) {
    auto root = new PolyFoo::Root();
    local_id_map->Insert(std::make_pair(id, static_cast<Poly::Root *>(root)));
    return EbbRef<PolyFoo>(id);
  }
  static PolyFoo &HandleFault(EbbId id) {
    return static_cast<PolyFoo &>(Poly::HandleFault(id));
  }
  class Root : public Poly::Root {
  public:
    Poly &HandleFault(EbbId id) override {
      auto rep = new PolyFoo();
      // Cache the reference to the rep in the local translation table
      EbbRef<PolyFoo>::CacheRef(id, *rep);
      return *rep;
    }
  };
  void Foo() override { kprintf("Foo!\n"); }
  void Bar() override { kprintf("Bar!\n"); }
  void Foobar() { kprintf("fooBar!\n"); }
};

} // namespace ebbrt
#endif
