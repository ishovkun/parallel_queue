#pragma once

#include <atomic>
#include <vector>


namespace algorithms {

class SPSCQueue
{
 public:
  SPSCQueue(int capacity)
      : _data(capacity),  _head(0), _cap(capacity), _tail(0)
  {}

  bool enqueue(int value);
  bool dequeue(int &value);
  int size() const {return _tail.load(std::memory_order_relaxed) - _head.load(std::memory_order_relaxed);}

  std::vector<int> _data;
  std::atomic<int> _head;
  std::atomic<int> _tail;
  // std::atomic<int> _size;
  int _cap;
 private:
};

bool SPSCQueue::enqueue(int value)
{
  auto head = _head.load(std::memory_order_relaxed);
  auto tail = _tail.load(std::memory_order_acquire);
  if (tail < head + _cap) {
    _data[tail % _cap] = value;
    _tail.store(tail + 1, std::memory_order_release);
    return true;
  }
  else return false;
}
// {
//   if (_size < _cap) {
//     _data[(_head + _size++)%_cap] = value;
//     return true;
//   }
//   else return false;
// }

bool SPSCQueue::dequeue(int &value)
{
  auto tail = _tail.load(std::memory_order_relaxed);
  auto head = _head.load(std::memory_order_acquire);
  if (tail > head) {
    value = _data[head % _cap];
    _head.store(head + 1, std::memory_order_release);
    return true;
  }
  else return false;
}
// {
//   if ( _size > 0 ) {
//     value = _data[(_head++)%_cap];
//     _size--;
//     return true;
//   }
//   return false;
// }


}  // end namespace algorithms
