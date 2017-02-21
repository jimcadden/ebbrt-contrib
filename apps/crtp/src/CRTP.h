//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef APPS_CRTP_CRTP_H_
#define APPS_CRTP_CRTP_H_

#include <boost/container/flat_map.hpp>

#include <ebbrt/Cpu.h>
#include <ebbrt/Debug.h>
#include <ebbrt/EbbId.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/SpinLock.h>

// Ebb<T,R>
// An Ebb is a object for programmers to use in thier application. Ebbs
// present a forward facing public interface. 
//
// Ebb<T,R> Ebb with a base type <T> and a "root" object type <R>
//
// Ebb<T> Ebb with a base type <T> without root
//
// SharedLocalEbb<T, R> : public Ebb<T, R>
//
// EbbShard<T,C,R>  : public Ebb<T,R>
// Internal component of the Ebb <T> with a child <C> and root <R> types

using namespace ebbrt;
namespace detail {
template <class T> using RepMap = boost::container::flat_map<size_t, T *>;
}
using detail::RepMap;
constexpr ebbrt::EbbId shard_layer_up_(ebbrt::EbbId id) { return id + 1; }
constexpr ebbrt::EbbId shard_layer_down_(ebbrt::EbbId id) { return id - 1; }

// Ebb w/ root 
//
template <class T, class R = void> class Ebb {
public:
  Ebb(ebbrt::EbbId id) : id_{id}, root_{shard_layer_up_(id)} {};
  static T &HandleFault(ebbrt::EbbId id);
protected:
  ebbrt::EbbId id_;
  ebbrt::EbbRef<R> root_; // typed Root pointer
};
template <typename T, typename R> T &Ebb<T, R>::HandleFault(ebbrt::EbbId id) {
  // Invoke the root object on each miss to receive local rep
  // Call may trigger miss of the root type 
  auto r = ebbrt::EbbRef<R>(shard_layer_up_(id))->GetRep();
  ebbrt::EbbRef<T>::CacheRef(id, *r);
  return *r;
}

// Ebb w/o root
//
template <class T> class Ebb<T, void> {
public:
  Ebb(ebbrt::EbbId id) : id_{id} {};
  static T &HandleFault(ebbrt::EbbId id);
protected:
  ebbrt::EbbId id_;
};
template <typename T> T &Ebb<T, void>::HandleFault(ebbrt::EbbId id) {
  // Construct an object for each miss 
  T *t = new T(id);
  ebbrt::EbbRef<T>::CacheRef(id, *t);
  return *t;
};

// SharedLocalEbb specialized Ebb behavior 
// Instances are shared across local cores/contexts
template <class T, class R = void> class SharedLocalEbb : public Ebb<T, R> {
  using Ebb<T, R>::Ebb;
public:
  SharedLocalEbb(ebbrt::EbbId id) : Ebb<T,R>(id) {};
  // Create pre-configures the local translation for a constructed rep
  static EbbRef<T> Create(T* rep, EbbId id = ebbrt::ebb_allocator->Allocate()) {
    local_id_map->Insert(std::make_pair(id, std::move(rep)));
    return EbbRef<T>(id);
  }
  static T &HandleFault(ebbrt::EbbId id);
};
template <typename T, typename R> T &SharedLocalEbb<T, R>::HandleFault(ebbrt::EbbId id)
{
  // Check local translation for representative (read-lock)
  {
    LocalIdMap::ConstAccessor accessor;
    auto found = local_id_map->Find(accessor, id);
    if (found) {
      auto& rep = *boost::any_cast<T*>(accessor->second);
      EbbRef<T>::CacheRef(id, rep);
      return rep;
    }
  }
  // If no rep was found, we construct a new one
  T* rep;
  {
    // Insert into the local translation map while holding exclusive lock
    LocalIdMap::Accessor accessor;
    auto created = local_id_map->Insert(accessor, id);
    if (!created) {
      // We raced with another writer, use the rep it created and return
      rep = boost::any_cast<T*>(accessor->second);
    } else {
      // call inherited handle fault
      rep = &Ebb<T,R>::HandleFault(id);
      accessor->second = rep;
    }
  }
  // Cache the reference in the core-local cache 
  EbbRef<T>::CacheRef(id, *rep);
  return *rep;
};

// EbbRoot <T, C>
// An internal Ebb structure with both and child <C> types
template <class T, class C> class EbbRoot {
public:
  EbbRoot(ebbrt::EbbId id) : id_{id} {};
  C *GetRep();
protected:
  C* new_child_(){ 
    auto rep = new C(shard_layer_down_(id_));
    local_reps_[Cpu::GetMine()] = rep;
    return rep;
  }
  RepMap<C> local_reps_;
  ebbrt::EbbId id_;
private:
  ebbrt::SpinLock lock_;
};

template <class T, class C> C *EbbRoot<T, C>::GetRep() {
  // TODO:Check for previously constructed rep for this core &  lock around structure
  auto core = (size_t)ebbrt::Cpu::GetMine();
  auto it = local_reps_.find(core);
  if (it != local_reps_.end()) {
    // return cached rep 
    return it->second;
  } else {
    //  construct a new rep and cache it 
    C *rep = new_child_();
    {
      std::lock_guard<ebbrt::SpinLock> guard(lock_);
      local_reps_[core] = rep;
    }
    return rep;
  }
};

// EbbShard
// 
template <class T, class C, class R = void> class EbbShard : public Ebb<T, R>, public EbbRoot<T,C> {
  using Ebb<T, R>::Ebb;
  using EbbRoot<T,C>::EbbRoot;
public:
  EbbShard(ebbrt::EbbId id) : Ebb<T, R>::Ebb(id), EbbRoot<T,C>::EbbRoot(id){};
};

// SharedLocalEbbShard
// 
template <class T, class C, class R = void> class SharedLocalEbbShard : public SharedLocalEbb<T, R>, public EbbRoot<T,C> {
  using SharedLocalEbb<T, R>::SharedLocalEbb;
  using EbbRoot<T,C>::EbbRoot;
public:
  SharedLocalEbbShard(ebbrt::EbbId id) : SharedLocalEbb<T, R>::SharedLocalEbb(id), EbbRoot<T,C>::EbbRoot(id){};
};

/*
// EbbShard <T, C, R>
// An internal Ebb structure with both root <R> and child <C> types
template <class T, class C, class R = void> class EbbShard : public SharedLocalEbb<T, R> {
  using SharedLocalEbb<T, R>::SharedLocalEbb;
public:
  EbbShard(ebbrt::EbbId id) : SharedLocalEbb<T, R>::SharedLocalEbb(id){};
  C *GetRep();
protected:
  C* new_child_(){ 
  kprintf("Ebb new child %d\n", Cpu::GetMine());
    auto rep = new C(shard_layer_down_(Ebb<T, R>::id_));
    local_reps_[Cpu::GetMine()] = rep;
    return rep;
  }
  RepMap<C> local_reps_;
private:
  ebbrt::SpinLock lock_;
};

template <class T, class C, class R> C *EbbShard<T, C, R>::GetRep() {
  kprintf("Ebb shard GetRep\n");
  // TODO:Check for previously constructed rep for this core &  lock around structure
  auto core = (size_t)ebbrt::Cpu::GetMine();
  auto it = local_reps_.find(core);
  if (it != local_reps_.end()) {
    // return cached rep 
    return it->second;
  } else {
    //  construct a new rep and cache it 
  kprintf("Ebb shard new_child_\n");
    C *rep = new_child_();
    {
      std::lock_guard<ebbrt::SpinLock> guard(lock_);
      local_reps_[core] = rep;
    }
    return rep;
  }
};
*/

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
