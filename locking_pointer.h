#ifndef _LOCKPTR_H
#define _LOCKPTR_H
#include <thread>
#include <mutex>

template<class T>
class lockptr{
public:
 lockptr() : ptr(nullptr), lck(nullptr) {}
 lockptr(T* ptr, std::mutex* lock) : ptr(ptr), lck(lock){
    if (ptr != nullptr || lck != nullptr)
      lck->lock();
  }
  ~lockptr(){
    if (lck)
      lck->unlock();
  }
  lockptr(lockptr<T>&& ptr) : ptr(ptr.ptr), lck(ptr.lck){
    ptr.ptr = nullptr;
    ptr.lck = nullptr;
  }

  T* operator ->(){
    return ptr;
  }
  T const* operator ->() const{
    return ptr;
  }
  bool isNull(){return ptr == nullptr;}
private:
  // disallow copy/assignment
  lockptr(lockptr<T> const& ptr){ }
  lockptr& operator = (lockptr<T> const& ptr){
    return *this;
  }
  T* ptr;
  std::mutex* lck;
};
#endif
