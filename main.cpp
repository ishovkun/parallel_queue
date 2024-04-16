#include <atomic>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <new>
#include <thread>
#include <cassert>
#include "FixedCapacityQueue.hpp"
#include "SPSCQueue.hpp"

using namespace std;


void test_serial()
{
  int n = 5;
  algorithms::FixedCapacityQueue q(n);
  for (int i = 0; i < n; i++) {
    auto success = q.push(i);
    if (!success) throw 3;
    if (q.size() != i+1) throw 2;
    // std::cout << "pushing " << i << " (h = " << q._head << " s = " << q._size << ") " << success << std::endl;
  }
  for (int i = 0; i < n + 3; i++) {
    int x{-1};
    auto success = q.pop(x);
    if (i < n) {
      if (!success) throw 3;
      if (x != i) throw 2;
    }
    // std::cout << "popping " << x << " (h = " << q._head << " size = " << q._size << ") " << success << std::endl;
  }
  for (int i = 0; i < n; i++) {
    auto success = q.push(i);
    if (!success) throw 3;
    if (q.size() != i+1) throw 2;
    // std::cout << "pushing " << i << " (h = " << q._head << " s = " << q._size << ") " << success << std::endl;
  }
}

int test_parallel_mutex()
{
  algorithms::FixedCapacityQueue q(100);
  int chunk = 50;
  int nthreads = 8;

  std::vector<std::thread> workers(nthreads);
  std::mutex mr;

  for (int ithread = 0; ithread < nthreads; ithread++)
    workers[ithread] = std::thread([&](int offset) {
      for (int i = 0; i < chunk; i++) {
        q.lock_and_push(offset + i);
      }
    }, ithread*1000);

  std::ranges::for_each(workers, [](auto & thread) {thread.join();});

  std::cout << "size = " << q.size() << std::endl;

  std::vector<std::vector<int>> retreive(nthreads);
  for (int ithread = 0; ithread < nthreads; ithread++) {
    workers[ithread] = std::thread([&](int offset) {
      while (q.size() > 0) {
        int x{-666};
        // q.lock_and_pop(x);
        if (q.lock_and_pop(x))
        {
          std::scoped_lock<std::mutex> lock(mr);
          retreive[ithread].push_back(x);
        }
      }
    }, ithread);
  }

  std::ranges::for_each(workers, [](auto & thread) {thread.join();});

  for (int ithread = 0; ithread < nthreads; ithread++) {
    std::cout << "thread " << ithread << ": ";
    for (auto value : retreive[ithread]) std::cout << value << " ";
    std::cout << std::endl;
  }
  return 0;
}

int test_spsc()
{
  algorithms::SPSCQueue queue(4);
  if (queue.size() != 0) throw 3;

  queue.enqueue(1);
  queue.enqueue(2);
  queue.enqueue(3);
  queue.enqueue(4);

  int store;
  for (size_t d = 1; d < 5; ++d) {
    queue.dequeue(store);
    assert(store == static_cast<int>(d));
    assert(queue.size() == 4 - d);
  }
  assert(queue.size() == 0);
  if (queue.size() != 0) throw 3;

  queue.enqueue(1);
  queue.enqueue(2);
  for (size_t d = 1; d < 3; ++d) {
    queue.dequeue(store);
    assert(store == static_cast<int>(d));
    assert(queue.size() == 2 - d);
  }
  assert(queue.size() == 0);

  thread t1([&](){
    for (int i = 0; i < 16; ++i) {
      while (!queue.enqueue(static_cast<int>(i))) {
        // std::cout << "enq " << i << std::endl;
        // printf("try enq %d\n",i);
      }
      printf("Enqueued %d\n", i);
    }
  });

  thread t2([&](){
    size_t it{0};
    int store;
    while (it < 16) {
      if (queue.dequeue(store)) {
        // std::cout << "deq " << it << std::endl;
        printf("deq %d\n", store);
        assert(store == static_cast<int>(it++));
      }
    }
  });

  t1.join();
  t2.join();

  // std::cout << "size = " << queue.size() << std::endl;
  assert(!queue.dequeue(store));

  assert(queue.enqueue(5));
  return 0;
}

int main()
{
  // test_serial();
  // test_parallel_mutex();
  test_spsc();



  return EXIT_SUCCESS;
}
