//          Copyright Boston University SESA Group 2013 - 2014.
//

//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef MULTICOREEBB_H_
#define MULTICOREEBB_H_

#include <cassert>
#include <utility>

#include <boost/container/flat_map.hpp>

#include <ebbrt/Cpu.h>
#include <ebbrt/EbbAllocator.h>
#include <ebbrt/EbbId.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/LocalIdMap.h>
#include <ebbrt/SpinLock.h>

#include <type_traits>

/*
 * MulticoreEbb class template defines a multicore Ebb class (i.e, one rep per
 * core/context) with an optional shared root object. The (2) templates
 * arguments
 * specify the base rep and root class types.
 *
 * class MyEbb : MulticoreEbb<MyEbb, MyEbbRoot>
 *
 * A rep of the multicore Ebb contains a list of the other
 * reps local to the node, as well as a pointer to shared root object.
 *
 * A Create() function is used to construct the initial ref and format the
 * local_id_map entry.
 */
namespace ebbrt {
namespace detail {
template <class T> using RepMap = boost::container::flat_map<size_t, T*>;
}
using detail::RepMap;

/* forward declarations of class templates */
template <class T, class R> class MulticoreEbbRootz;
template <class T, class R> class MulticoreEbbz;

/* Ebb Root class templace with R-type Rep Map */
template <class T, class R> class MulticoreEbbRootz {
protected:
  R* get_rep_(size_t core); 
  RepMap<R> reps_;
  EbbId id_;
private:
  friend class MulticoreEbbz<R, T>;
};

template <class T, class R> R* MulticoreEbbRootz<T,R>::get_rep_(size_t core) {
    auto it = reps_.find(core);
      if (it != reps_.end()) {
        return it->second;
      }else{
        auto ret = new R();
        reps_[core] = ret;
        ret->root_ = static_cast<T*>(this);
        return ret;
      }
}

/* Multicore Ebb class template with typed Root */
template <class T, class R> class MulticoreEbbz {
  static_assert(std::is_base_of<MulticoreEbbRootz<R, T>, R>::value,
                "Root type must inherit from MulticoreEbbRoot<R,T>");
public:
  MulticoreEbbz() = default;
  static T &HandleFault(EbbId id);
protected:
  R *root_ = nullptr;
private:
  // By making the base root type a friend allows the rep constructor to initialize protected members
  friend class MulticoreEbbRootz<R, T>;
};

template <class T, class R> T &MulticoreEbbz<T, R>::HandleFault(EbbId id) {
retry:
  {
    // Check for root in LocalIdMap (read-lock)
    LocalIdMap::ConstAccessor accessor;
    auto found = local_id_map->Find(accessor, id);
    if (found) {
      // Ask root for representative (remain locked) 
      // Root checks for exiting rep and constructs one if needed
      auto root = boost::any_cast<R*>(accessor->second);
      T* rep = root->get_rep_((size_t)Cpu::GetMine());
      EbbRef<T>::CacheRef(id, *rep);
      return *rep;
    }
  } 
  // No root was found. Let's construct one and try again
  {
    // Acquire exclusive lock, construct and insert root 
    LocalIdMap::Accessor accessor;
    auto created = local_id_map->Insert(accessor, id);
    // In this case of a race, the thread that successfully inserted into the
    // LocalIdMap will create the root object
    if (created) {
      auto root = new R();
      root->id_ = id; 
      accessor->second = root;
    }
  }
  // retry, expecting we'll find the root on the next pass 
  goto retry;
}

} // namespace ebbrt

#endif // COMMON_SRC_INCLUDE_EBBRT_MULTICOREEBB_H_
