#pragma once

// This file comes from https://github.com/taskflow/work-stealing-queue
// and has been modified by Nazara author (SirLynix):
// - _top and _bottom atomics are now aligned to the double of the cacheline size
// - the queue allows to override the value type returned by pop/steal

#include <atomic>
#include <vector>
#include <optional>
#include <cassert>
#include <new>

/**
@class: WorkStealingQueue

@tparam T data type

@brief Lock-free unbounded single-producer multiple-consumer queue.

This class implements the work stealing queue described in the paper, 
"Correct and Efficient Work-Stealing for Weak Memory Models,"
available at https://www.di.ens.fr/~zappa/readings/ppopp13.pdf.

Only the queue owner can perform pop and push operations,
while others can steal data from the queue.
*/
template <typename T, typename V = std::optional<T>>
class WorkStealingQueue {

  struct Array {

    int64_t C;
    int64_t M;
    std::atomic<T>* S;

    explicit Array(int64_t c) : 
      C {c},
      M {c-1},
      S {new std::atomic<T>[static_cast<size_t>(C)]} {
    }

    ~Array() {
      delete [] S;
    }

    int64_t capacity() const noexcept {
      return C;
    }
    
    template <typename O>
    void push(int64_t i, O&& o) noexcept {
      S[i & M].store(std::forward<O>(o), std::memory_order_relaxed);
    }

    T pop(int64_t i) noexcept {
      return S[i & M].load(std::memory_order_relaxed);
    }

    Array* resize(int64_t b, int64_t t) {
      Array* ptr = new Array {2*C};
      for(int64_t i=t; i!=b; ++i) {
        ptr->push(i, pop(i));
      }
      return ptr;
    }

  };

  // avoids false sharing between _top and _bottom
#ifdef __cpp_lib_hardware_interference_size
  alignas(std::hardware_destructive_interference_size * 2) std::atomic<int64_t> _top;
  alignas(std::hardware_destructive_interference_size * 2) std::atomic<int64_t> _bottom;
#else
  alignas(64 * 2) std::atomic<int64_t> _top;
  alignas(64 * 2) std::atomic<int64_t> _bottom;
#endif
  std::atomic<Array*> _array;
  std::vector<Array*> _garbage;

  public:
    
    /**
    @brief constructs the queue with a given capacity

    @param capacity the capacity of the queue (must be power of 2)
    */
    explicit WorkStealingQueue(int64_t capacity = 1024);

    /**
    @brief destructs the queue
    */
    ~WorkStealingQueue();
    
    /**
    @brief queries if the queue is empty at the time of this call
    */
    bool empty() const noexcept;
    
    /**
    @brief queries the number of items at the time of this call
    */
    size_t size() const noexcept;

    /**
    @brief queries the capacity of the queue
    */
    int64_t capacity() const noexcept;
    
    /**
    @brief inserts an item to the queue

    Only the owner thread can insert an item to the queue. 
    The operation can trigger the queue to resize its capacity 
    if more space is required.

    @tparam O data type 

    @param item the item to perfect-forward to the queue
    */
    template <typename O>
    void push(O&& item);
    
    /**
    @brief pops out an item from the queue

    Only the owner thread can pop out an item from the queue. 
    The return can be a default-constructed V if this operation failed (empty queue).
    */
	V pop();
    
    /**
    @brief steals an item from the queue

    Any threads can try to steal an item from the queue.
    The return can be a default-constructed V if this operation failed (not necessary empty).
    */
	V steal();
};

// Constructor
template <typename T, typename V>
WorkStealingQueue<T, V>::WorkStealingQueue(int64_t c) {
  assert(c && (!(c & (c-1))));
  _top.store(0, std::memory_order_relaxed);
  _bottom.store(0, std::memory_order_relaxed);
  _array.store(new Array{c}, std::memory_order_relaxed);
  _garbage.reserve(32);
}

// Destructor
template <typename T, typename V>
WorkStealingQueue<T, V>::~WorkStealingQueue() {
  for(auto a : _garbage) {
    delete a;
  }
  delete _array.load();
}
  
// Function: empty
template <typename T, typename V>
bool WorkStealingQueue<T, V>::empty() const noexcept {
  int64_t b = _bottom.load(std::memory_order_relaxed);
  int64_t t = _top.load(std::memory_order_relaxed);
  return b <= t;
}

// Function: size
template <typename T, typename V>
size_t WorkStealingQueue<T, V>::size() const noexcept {
  int64_t b = _bottom.load(std::memory_order_relaxed);
  int64_t t = _top.load(std::memory_order_relaxed);
  return static_cast<size_t>(b >= t ? b - t : 0);
}

// Function: push
template <typename T, typename V>
template <typename O>
void WorkStealingQueue<T, V>::push(O&& o) {
  int64_t b = _bottom.load(std::memory_order_relaxed);
  int64_t t = _top.load(std::memory_order_acquire);
  Array* a = _array.load(std::memory_order_relaxed);

  // queue is full
  if(a->capacity() - 1 < (b - t)) {
    Array* tmp = a->resize(b, t);
    _garbage.push_back(a);
    std::swap(a, tmp);
    _array.store(a, std::memory_order_relaxed);
  }

  a->push(b, std::forward<O>(o));
  std::atomic_thread_fence(std::memory_order_release);
  _bottom.store(b + 1, std::memory_order_relaxed);
}

// Function: pop
template <typename T, typename V>
V WorkStealingQueue<T, V>::pop() {
  int64_t b = _bottom.load(std::memory_order_relaxed) - 1;
  Array* a = _array.load(std::memory_order_relaxed);
  _bottom.store(b, std::memory_order_relaxed);
  std::atomic_thread_fence(std::memory_order_seq_cst);
  int64_t t = _top.load(std::memory_order_relaxed);

  V item = {};

  if(t <= b) {
    item = a->pop(b);
    if(t == b) {
      // the last item just got stolen
      if(!_top.compare_exchange_strong(t, t+1, 
                                       std::memory_order_seq_cst, 
                                       std::memory_order_relaxed)) {
        item = V{};
      }
      _bottom.store(b + 1, std::memory_order_relaxed);
    }
  }
  else {
    _bottom.store(b + 1, std::memory_order_relaxed);
  }

  return item;
}

// Function: steal
template <typename T, typename V>
V WorkStealingQueue<T, V>::steal() {
  int64_t t = _top.load(std::memory_order_acquire);
  std::atomic_thread_fence(std::memory_order_seq_cst);
  int64_t b = _bottom.load(std::memory_order_acquire);
  
  V item = {};

  if(t < b) {
    Array* a = _array.load(std::memory_order_consume);
    item = a->pop(t);
    if(!_top.compare_exchange_strong(t, t+1,
                                     std::memory_order_seq_cst,
                                     std::memory_order_relaxed)) {
      return V{};
    }
  }

  return item;
}

// Function: capacity
template <typename T, typename V>
int64_t WorkStealingQueue<T, V>::capacity() const noexcept {
  return _array.load(std::memory_order_relaxed)->capacity();
}

