### Thread creation

http://www.cplusplus.com/reference/thread/thread/

We can attach a callback with the std::thread object, that will be executed when this new thread starts, These callback are

+ Function Pointers
+ Function Objects
+ Lambda function

**New thread will start just after the creation of new thread object** and will execute the passed callback in parallel to thread that has started it.

Any thread can wait for another to exit by calling join() function on that thread object.

copy constructor and copy assign operator object is deleted.

```c++
// function pointer
void thread_function(int n)
{
    for(int i=0;i<n;++i)
        std::cout<<"thread funciton executing"<<std::endl;
}

void f(int &n)
{
}

// function objects
class DisplayThread{
public:
    void operator()(int n)
    {
        for(int i=0;i<n;++i)
        std::cout<<"thread funciton executing"<<std::endl;
    }
    
    void testThread(int n)
    {
    }
}

int main()
{
    // create thread with funciton pointer
    std::thread t1(thread_function, 10);
    
    // creaing thread with function objects
    std::thread t2((DisplayThread(), 10));
   
    // creating thread with lambda funciton
    std::thread t3([]{
        for(int i=0;i<10;++i)
            std::cout<<"haha"<<std::endl;
    });
    
    // reference argement pass
    int n=10;
    std::thread t4(f, std::ref(n));
    
    // assign pointer to memeber function of a class as thread function
    DisplayThread tmp;
    std::thread threadobj(&DisplayThread::testTread, &tmp, n);
    
    t1.join();
    t2.join();
    t3.detach();
}
```

#### Other method

+ get_id: return thread id
+ joinable: a thread is not joinable if 
  + it was default constructed
  + has been moved from, constructing or assigning to it.
  + either of its members join or detach has been called.
+ join: waiting this thread until it ends
+ detach: detaches the thread from the calling thread, allowing them to execute independently from each other.
+ hardware_concurrency: number of hardware thread contexts.



### Data race and mutex

#### std::mutex

+ lock()：若该mutex被锁住，则阻塞，自己锁住再调用会死锁
+ unlock：解锁
+ try_lock：若mutex被锁住，不阻塞，返回false，其他和lock一致

#### std::recursive_mutex

允许多次上锁。

#### std::time_mutex

比std::mutex多了两个成员函数

+ try_lock_for: 获取锁时至多阻塞t秒，超时返回false
+ try_lock_until: 接受一个时间点，指定时间点之前未获得锁则被阻塞，超时仍未获得返回false。

```c++
#include <mutex>
#include <chrono>

std::timed_mutex mtx;
void f()
{
    mtx.try_lock_for(std::chrono::milliseconds(200));
}
```

#### std::recursive_timed_mutex

略

#### std::lock_guard

Mutex 的 RAII相关，主要为了处理异常时锁的释放。在lock_guard的生命周期内，他所管理的对象会一直处于上锁状态

```c++
void print_block(int n, char c)
{
    std::mutex mtx;
    try{
        std::lock_guard<std::mutex> lock(mtx);
        std::cout<<1<<std::endl;
    }
    catch(std::logic_error&)
    {
        // the lock_guard will be destructed here and the lock is freed.
    }
}
// tag 版本
void print_block_tag_version(int n, char c)
{
    std::mutex mtx;
    mtx.lock();
    // already locked
    std::lock_guard<std::mutex> lock(mtx, std::adopt_lock);
}
```

#### std::unique_lock

Mutex的RAII相关，提供了更多的上锁和解锁控制



### std::atomic

https://www.zhihu.com/question/24301047

#### atomic_flag:

atomic_flags是一种简单的原子bool类型，操作只有test-and-set和clear。如果没有用宏**ATOMIC_FLAG_INIT**初始化，则其状态时未指定的，否则是clear状态。

+ test_and_set 函数会检查std::atomic标志，若没有被设置则设置其标志位，返回是否被设置过，是一个原子操作。
+ clear：值设置为false

#### std::memory_order

处理器乱序执行和编译器指令重排可能造成数据读写顺序错乱，CPU缓存可能造成数据更新不及时，memory_order的作用是明确数据的读写顺序以及数据的写入对其他线程的可见性。

+ Relaxed ordering:原子操作带上memory_order_relaxed，仅保证操作是原子性的，无其他约束
+ Release-Acquire ordering：对于同一个atomic，在线程A中使用memory_order_release调用store()，在线程B中使用memory_order_acquire调用load。这种模型保证在store之前发型的所有读写操作（A线程）不会在store（）后调用，在load()之后发生的所有读写操作不会在load()前调用，A线程的所有写入操作对B线程可见
+ Release-Consume ordering: 上边的弱化版。对于同一个atomic，线程A中使用memory_order_release调用store()，线程B用memory_order_consume调用load。这种模型保证store()之前发生的所有读写操作（A线程）不会在store后调用，在load()之后发生的依赖于该atomic的读写操作（B线程）不会在load()前面调用，A线程对该atomic的带依赖写入操作对B线程可见
+ Sequential consistency: 默认的顺序，执行代价最大，读写操作顺序均一致。

#### std::atomic

+ is_lock_free: 如果某个对象满足lock-free特性，多个线程访问该对象时不会导致线程阻塞

+ store:修改被封装的值，可指定memory order.

+ load:读取，可指定memory order

+ operator T:读取值，带类型转换操作

+ exchange:读取并修改被封装的值，返回之前该原子对象封装的值。

+ compare_exchange_weak

  ```c++
  // first type
  bool compare_exchange_weak (T& expected, T val,
             memory_order sync = memory_order_seq_cst) volatile noexcept;
  bool compare_exchange_weak (T& expected, T val,
             memory_order sync = memory_order_seq_cst) noexcept;
  
  // second type
  bool compare_exchange_weak (T& expected, T val,
             memory_order success, memory_order failure) volatile noexcept;
  bool compare_exchange_weak (T& expected, T val,
             memory_order success, memory_order failure) noexcept;
  ```

  比较被封装的值与参数expected指定的值是否相等。

  + 相等：用val替换原子对象的值，返回true
  + 不等：用原子对象的值替换expected的值，返回false

  在第二种情况下，memory order取决于比较操作的结果，如果比较结果为true，选择success指定的memory order。否则是failure。

+ compare_exchange_strong：与compare_exchange_weak基本一致

  不同点：两个函数都是比较的物理内容，所以对象的比较操作可能在operator==时判断相等，但compare_exchange_weak判断时却可能失败，因为对象底层的物理内容中可能存在位对齐或者其他逻辑表示相同但物理不同的值。strong版本不允许返回false，即原子对象所封装的值与参数 *expected* 的物理内容相同，比较操作一定会为 true。weak版本的效果更好，循环时使用，其他情况下用strong。