#ifndef APPS_POLY_SRC_POLY_H_
#define APPS_POLY_SRC_POLY_H_

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

    auto rep = boost::any_cast<Root *>(accessor->second);
    return rep->HandleFault(id);
  };
  virtual void Foo() = 0;
  virtual void Bar() = 0;
};

class PolyFoo : Poly {
public:
  PolyFoo(){};
  static EbbRef<Poly> Create(EbbId id = ebb_allocator->Allocate()) {
    auto root = new PolyFoo::Root();
    local_id_map->Insert(std::make_pair(id, dynamic_cast<Poly::Root*>(root)));
    return EbbRef<Poly>(id);
  }
  class Root : public Poly::Root {
  public:
    Poly &HandleFault(EbbId id) override {
      PolyFoo *rep;
      rep = new PolyFoo();
      // Cache the reference to the rep in the local translation table
      EbbRef<PolyFoo>::CacheRef(id, *rep);
      return *rep;
    }
  };
  void Foo() override { kprintf("Foo!\n"); }
  void Bar() override { kprintf("Bar!\n"); }
};

} // namespace ebbrt
#endif

// class Poly {
//    class Root {
//        virtual Poly* HandleFault(EbbId id) = 0;
//    };
//    static Poly *HandleFault(EbbId id) {
//        // Look for Poly::Root * in LocalIdMap
//        auto root = ...;
//        return root->HandleFault(id);
//    }
//
//    virtual void foo() = 0;
//};
//
// class MyPoly : Poly {
//    class Root : Poly::Root {
//        Poly* HandleFault(EbbId id) override {
//            //Construct Rep
//        }
//        //members...
//    }
//    static EbbRef<Poly> Create() {
//        //alloc EbbId
//        // Construct MyPoly::Root and store pointer in LocalIdMap
//        // Return EbbRef
//    }
//    void foo() override {
//        // My Impl
//    }
//};
//

// EbbRef<Poly> my_ref
// Poly::HandleFault(ebbid)

//// more
// class Poly {
//    class Root {
//        virtual Poly* HandleFault(EbbId id) = 0;
//    };
//    static Poly *HandleFault(EbbId id) {
//        // Look for Poly::Root * in LocalIdMap
//        auto root = ...;
//        return root->HandleFault(id);
//    }
//
//    virtual void foo() = 0;
//};
//
// class MyPoly : Poly {
//    class Root : Poly::Root {
//        Poly* HandleFault(EbbId id) override {
//            //Construct Rep
//        }
//        //members...
//    }
//    static EbbRef<Poly> Create() {
//        //alloc EbbId
//        // Construct MyPoly::Root and store pointer in LocalIdMap
//        // Return EbbRef
//    }
//    void foo() override {
//        // My Impl
//    }
//};
//

// EbbRef<Poly> my_ref
// Poly::HandleFault(ebbid)
// This is *IMPORTANT*, it allows the messenger to resolve remote HandleFaults
// TODO: EBBRT_PUBLISH_TYPE(, Poly);
