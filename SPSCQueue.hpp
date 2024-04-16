#pragma once

#include <atomic>
#include <vector>


namespace algorithms {

class SPSCQueue
{
 public:
  SPSCQueue(int capacity)
      : _data(capacity),  _head(0), _cap(capacity)
  {}

  bool enqueue(int value);
  bool dequeue(int &value);
  int size() const {return _size;}

  std::vector<int> _data;
  std::atomic<int> _head;
  std::atomic<int> _size;
  int _cap;
 private:
};

bool SPSCQueue::enqueue(int value)
{
  if (_size < _cap) {
    _data[(_head+_size++)%_cap] = value;
    return true;
  }
  else return false;
}

bool SPSCQueue::dequeue(int &value)
{
  if ( _size > 0 ) {
    value = _data[(_head++)%_cap];
    _size--;
    return true;
  }
  return false;
}


}  // end namespace algorithms
