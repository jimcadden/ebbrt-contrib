//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_CRTP_CRTP_H_
#define APPS_CRTP_CRTP_H_

#include <boost/container/flat_map.hpp>

#include "EbbRef.h"
#include <ebbrt/Cpu.h>
#include <ebbrt/EbbId.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/SpinLock.h>

// e.g., Origin -> Domain -> Region -> Subregion -> Context
// Origin -> Shard -> Shard -> Shart
//          (layer) (layer) (layer)
//
// id space: [ 0 | ID | INTERNAL | XXXX ]
//
// Ebb<T,R,C>

// TRY NEXT, swap root and child ordering


using namespace ebbrt;

namespace detail {
template <class T> using RepMap = boost::container::flat_map<size_t, T *>;
}
using detail::RepMap;

constexpr ebbrt::EbbId layer_up_(ebbrt::EbbId id) { return id + 1; }
constexpr ebbrt::EbbId layer_down_(ebbrt::EbbId id) { return id - 1; }


// Ebb Interface
template <class T, class R = void> class Ebb {
public:
  Ebb(ebbrt::EbbId id) : id_{id}, root_{layer_up_(id)} {};
  static T &HandleFault(ebbrt::EbbId id);

protected:
  ebbrt::EbbId id_;
  ebbrt::EbbRef<R> root_;
};

template <typename T, typename R> T &Ebb<T, R>::HandleFault(ebbrt::EbbId id) {
  std::cout << "Ebb<T,R>::HF " << id << std::endl;
  auto r = ebbrt::EbbRef<R>(layer_up_(id))->NewChild();
  ebbrt::EbbRef<T>::CacheRef(id, *r);
  return *r;
}

// Ebb Interface w/o a specified root type
//
template <class T> class Ebb<T, void> {
public:
  Ebb(ebbrt::EbbId id) : id_{id} {};
  static T &HandleFault(ebbrt::EbbId id);
protected:
  ebbrt::EbbId id_;
};

template <typename T> T &Ebb<T, void>::HandleFault(ebbrt::EbbId id) {
  std::cout << "Ebb<T>::HF " << id << std::endl;
  T *t = new T(id);
  ebbrt::EbbRef<T>::CacheRef(id, *t);
  return *t;
};

// (Internal) Ebb Shard
//
template <class T, class C, class R = void> class EbbShard : public Ebb<T, R> {
public:
  EbbShard(ebbrt::EbbId id) : Ebb<T, R>::Ebb(id){};
  C *NewChild();
protected:
  C* create_initial_rep_(){ return create_rep_();}
  C* create_rep_(){ return new C(layer_down_(Ebb<T, R>::id_));}
  RepMap<C> local_reps_;
private:
  ebbrt::SpinLock lock_;
};

template <class T, class C, class R> C *EbbShard<T, C, R>::NewChild() {
  auto core = (size_t)ebbrt::Cpu::GetMine();
  auto it = local_reps_.find(core);
  if (it != local_reps_.end()) {
    // rep was already cached for this core, return it
    return it->second;
  } else {
    //  construct a new rep and cache the address
    C *rep;
    if (local_reps_.size() == 0) {
      // construct initial rep on the node
      rep = create_initial_rep_();
    } else {
      rep = create_rep_();
    }
    // cached rep
    {
      std::lock_guard<ebbrt::SpinLock> guard(lock_);
      local_reps_[core] = rep;
    }
    return rep;
  }
};

// SharedLocalEbb
//
template <class T, class R = void> class SharedLocalEbb : public Ebb<T, R> {
  using Ebb<T, R>::Ebb;
public:
  static EbbRef<T> Create(T* rep, EbbId id = ebbrt::ebb_allocator->Allocate()) {
    local_id_map->Insert(std::make_pair(id, std::move(rep)));
    return EbbRef<T>(id);
  }
  static T &HandleFault(ebbrt::EbbId id);
};

// Handle a fault on SharedLocalEbb by checking local id map for rep and, if not found, locks and calls underlying HF to
// process creation of Ebb.
template <typename T, typename R> T &SharedLocalEbb<T, R>::HandleFault(ebbrt::EbbId id)
{
  std::cout << "SharedLocalEbb<T,R>::HF[" << id << std::endl;
  {
    // First we check if the representative is in the LocalIdMap (using a
    // read-lock)
    LocalIdMap::ConstAccessor accessor;
    auto found = local_id_map->Find(accessor, id);
    if (found) {
      auto& rep = *boost::any_cast<T*>(accessor->second);
      EbbRef<T>::CacheRef(id, rep);
      return rep;
    }
  }
  // else, 
  T* rep;
  {
    // Insert entry into the id map while holding exclusive (write) lock
    LocalIdMap::Accessor accessor;
    auto created = local_id_map->Insert(accessor, id);
    if (!created) {
      // We raced with another writer, use the rep it created and return
      rep = boost::any_cast<T*>(accessor->second);
    } else {
      // Create a new rep and insert it into the LocalIdMap
      // XXX: We are still locked and this may block...
      accessor->second = Ebb<T,R>::HandleFault(id);
    }
  }
  // Cache the reference to the rep in the local translation table
  EbbRef<T>::CacheRef(id, *rep);
  return *rep;
};


#if 0
struct VoidEbb {};

// Ebb Type
template <class T, class R = void>
class Ebb
{
  public:
  Ebb(ebbrt::EbbId id) : id_{id}, root_{layer_up_(id)} {};
  static T& HandleFault(ebbrt::EbbId id); 
  C& NewChild();
  ebbrt::EbbId id_;
  ebbrt::EbbRef<R> root_;
};



template <typename T, typename C, typename R>
T& Ebb<T,C,R>::HandleFault(ebbrt::EbbId id){
  std::cout << "Ebb<T,xVx,C>::HF" << std::endl;
  T* t = new T(id);
  return *t; 
}

// NO ROOT, NO CHILD
template <class T>
class Ebb<T,VoidEbb,VoidEbb>
{
  public:
  Ebb(ebbrt::EbbId id) : id_{id} {};
  static T& HandleFault(ebbrt::EbbId id); 
  T& NewChild();
  ebbrt::EbbId id_;
};

template <typename T>
T& Ebb<T,VoidEbb,VoidEbb>::NewChild(){
  return T::HandleFault(id_);
}

template <typename T>
T& Ebb<T,VoidEbb,VoidEbb>::HandleFault(ebbrt::EbbId id){
  std::cout << "Ebb<T>::HF" << std::endl;
  T* t = new T(id);
  return *t; 
}

#endif

// DEFINITIONS
// Childless
// template <typename T, typename R>
// T* Ebb<T,R,void>::HandleFault(EbbId id){
//  return new T(id);
//}

// template <typename T, typename R, typename C>
// R* Ebb<T,R,C>::Root(){
//  if( !root_ ){
//    root_ = R::HandleFault(id_);
//  }
//  return root_;
//}
// Childless
// template <typename T, typename R, typename C>
// C* Ebb<T,R,C>::NewChild(){
//  return new C(id_);
//}

#if 0


template <typename T, typename Root = NilRep>
struct Ebb
{
  Ebb(uint32_t id) : id_{id} {};
  static T* create(uint32_t id){
    auto root = Root::create(id);
    return new T(id, root);
  }
  Root* root_;
  uint32_t id_ = 0;
};


// ORIGIN
struct Ebby3 : Ebb<Ebby3> {
  Ebby3(uint32_t id) : Ebb(id){ 
    std::cout << "head["<< id_ << "]-" ; 
  }
};
// NODE
struct Ebby2 : Ebb<Ebby2, Ebby3> {
  Ebby2(uint32_t id) : Ebb(id) { 
    std::cout << "link["<< id_ << "]-" ; 
  }
};
// REP
struct Ebby : Ebb<Ebby, Ebby2> {
  Ebby(uint32_t id) : Ebb(id) { 
    std::cout << "tail["<< id_ << "]" << std::endl;
  }
  void foo(int x){
    std::cout << "Ebby foo!" << id_ << "~" << x << std::endl;
  }
};


///////////////////////////////////////////////////////////////////
//
// A -> B -> C
template <typename T>
struct A
{
  void call_foo(int x){
    static_cast<T*>(this)->foo(x);
  }
  static void call_fault(int x){
    T::fault(x);
  }
  void foo(int x){
    std::cout << "A foo:" << x << std::endl;
  }
  static void fault(int x){
  std::cout << "A fault: " << x << std::endl;
  }
};

template <typename T>
struct B : A<T>
{
  void call_foo(int x){
    static_cast<T*>(this)->foo(x);
  }
  static void call_fault(int x){
    A<T>::fault(x);
    fault(x);
    T::fault(x);
  }
  void foo(int x){
    std::cout << "B foo:" << x << std::endl;
  }
  static void fault(int x){
  std::cout << "B fault: " << x << std::endl;
  }
};


template <typename T>
struct C : B<T>
{
  void call_foo(int x){
    static_cast<T*>(this)->foo(x);
  }
  static void call_fault(int x){

    T::fault(x);
  }
  void foo(int x){
    std::cout << "C foo:" << x << std::endl;
  }
  static void fault(int x){
  std::cout << "C fault: " << x << std::endl;
  }
};

struct C1 : C<C1> {
  // use defaults
  void bar(){
  std::cout << "C1 bar" << std::endl;
  }
};


// TEST 1 - basic CRTP 
struct
template <class T>
struct Origin {
  void call_foo(int x){
    static_cast<T*>(this)->foo(x);
  }
  static void call_fault(int x){
    T::fault(x);
  }
  void foo(int x){
    std::cout << "Origin foo:" << x << std::endl;
  }
  static void fault(int x){
  std::cout << "Origin static fault: " << x << std::endl;
  }
};

struct DomainA : Origin<DomainA>
{
  // use inherited fault 
  void foo(int x){
    std::cout << "DomainA foo:" << x << std::endl;
  }
};

struct DomainB : Origin<DomainB>
{
  // use inherited foo
  static void fault(int x){
  std::cout << "DomainB static fault: " << x << std::endl;
  }
};

#endif

#endif
