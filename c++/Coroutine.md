https://lewissbaker.github.io/

### Coroutine Theory

#### Activation Frames

**Activation frame** is a block of memory that holds the current state of a particular invocation of a function. The data structure used for allocating and freeing the activation frames for function calls are called the stack. When an activation frame is allocated on this stack it is called the **stack frame.**

With coroutines there are some parts of the activation frame that need to be preserved across coroutine suspension. This is the **coroutine frame**. Other parts only need to be kept around while the coroutine  is executing. This the the **stack frame**.

When a coroutine is called, the first thing the coroutine does is  allocate a coroutine-frame on the heap and copy/move the parameters from the stack-frame into the coroutine frame so that the lifetime of the parameters extends beyond the first suspend-point.



### 				Operator co_await


The coroutines defines two kinds of interfaces:

+ The **Promise** interface specifies methods for customizing the behavior of the coroutine itself. For example, what happens when the coroutine returns and customize the behavior of any co_await and co_yield expression within the coroutine.
+ The **Awaitable** interface specifies methods that control the semantics of a co_await expression. When a value is co_awaited, the code is translated into a series of calls to methods on the awaitable object that allow it to specify: wither to suspend the coroutine(awiat_ready), execute some logic after it has suspended to schedule the coroutine for later resumption (await_suspend) , and so on.

####  Awaitables and Awaiters

**A type that supports the co_await operator is called an Awaitable type**. The promise type used for a coroutine can alter the meaning of a co_await expression within the coroutine  via its **await_transform** method.

+ **Normally Awaitable**: a type that supports the co_await operator whose promise type does not have an await_tranform
+ **Contextually Awaitable** : promise have await_transform

**Awaiter** type is a type that implements the three special methods that are called as part of a co_await expression: await_ready, await_suspend, await_resume.

A type can be both an Awaitable type and An Awaiter type.

#### compiler operation for co_await\<expr>

##### Obtaining the Awaiter

1. Get the Awaitable: 
   + If the promise type P, has a member  name await_transform, then \<expr> is first passed into a call to promise.await_transform(\<expr>) to obtain the Awaitable value.
   + Otherwise, if the promise type does not have an await_transform member then we use the result of evaluating \<expr> as the Awaiable object.

2. Get the Awaiter object:
   + if the Awaitable object, has an applicable operator co_await() overload then this is called to obtain the Awaiter object.
   + otherwise, the Awaitable object is used as the awaiter object.

##### Awaiting the Awaiter

```c++
{
  auto&& value = <expr>;
  auto&& awaitable = get_awaitable(promise, static_cast<decltype(value)>(value));
  auto&& awaiter = get_awaiter(static_cast<decltype(awaitable)>(awaitable));
  if (!awaiter.await_ready())
  {
    using handle_t = std::experimental::coroutine_handle<P>;

    using await_suspend_result_t =
      decltype(awaiter.await_suspend(handle_t::from_promise(p)));

    <suspend-coroutine>

    if constexpr (std::is_void_v<await_suspend_result_t>)
    {
      awaiter.await_suspend(handle_t::from_promise(p));
      <return-to-caller-or-resumer>
    }
    else
    {
      static_assert(
         std::is_same_v<await_suspend_result_t, bool>,
         "await_suspend() must return 'void' or 'bool'.");

      if (awaiter.await_suspend(handle_t::from_promise(p)))
      {
        <return-to-caller-or-resumer>
      }
    }
  }
  <resume-point>
  return awaiter.await_resume();
}
```

+ the void or true return value of the await_suspend transfers execution back to the caller of the coroutine.
+ When the suspended coroutine is eventually resumed then the execution resumes at the \<resume_point> immediately before the await_resume method is called. The return value of the await_resume method call becomes the result of the co_await.

#### Coroutine Handles

The coroutine_handle represents a non-owning handle to the coroutine frame and can be used to resume execution of the coroutine or to destroy the coroutine frame. Also be used to access the coroutine's promise object. The definition of coroutine is:

```c++
namespace std::experimental
{
  template<typename Promise>
  struct coroutine_handle;

  template<>
  struct coroutine_handle<void>
  {
    bool done() const;

    void resume();
    void destroy();

    void* address() const;
    static coroutine_handle from_address(void* address);
  };

  template<typename Promise>
  struct coroutine_handle : coroutine_handle<void>
  {
    Promise& promise() const;
    static coroutine_handle from_promise(Promise& promise);

    static coroutine_handle from_address(void* address);
  };
}
```

+ Calling .resume() on a coroutine_handle reactivates a suspended coroutine at the \<resume-pioint>
+ The promise() methods returns a reference to the coroutine's promise object. Consider the coroutine's promise object as an internal implementation detail of the coroutine.
+ The courouine_handle\<P>::from_promise(P& promise) function allows reconstructing the coroutine handle from a reference to the coroutine's promise object.
+ The .address() / from_address() allows converting a coroutine handle to/ from a void* pointer.

#### Synchronization-free async coode

co_await operator has the ability to **execute code after the coroutine has been suspended** but before execution is returned to the caller/ resumer by await_suspend function of the awaiter object. This allows an Awaiter object to initiate an async operation after the coroutine is already suspended, **passing the coroutine handle of the suspended coroutine to the operation which it can safely resume when the operation completes without any additional synchronization required.** One example is:

```
Time     Thread 1                           Thread 2
  |      --------                           --------
  |      ....                               Call OS - Wait for I/O event
  |      Call await_ready()                    |
  |      <supend-point>                        |
  |      Call await_suspend(handle)            |
  |        Store handle in operation           |
  V        Start AsyncFileRead ---+            V
                                  +----->   <AsyncFileRead Completion Event>
                                            Load coroutine_handle from operation
                                            Call handle.resume()
                                              <resume-point>
                                              Call to await_resume()
                                              execution continues....
           Call to AsyncFileRead returns
         Call to await_suspend() returns
         <return-to-caller/resumer>
```

As soon as you have started the operation which publishes the coroutine handle to other threads then another thread may resume the coroutine on another thread before await_suspend returns and may continue executing concurrently with the rest of the await_suspend method

When coroutine resumes, the first thing it do is call await_resume() to get the result and then often it will immediately destruct the Awaiter object. The coroutine could potentially run to completion, destruct the coroutine and promise object, all before await_suspend returns. **Avoid accessing this or the coroutine's.promise method object because both could already destroyed.**





## 								Promise type

#### Promise objects

The Promise objects defines and controls the behavior of the coroutine itself by implementing methods that are called at specific points during execution of coroutine. Think about the coroutine's promise object as being a "coroutine state controller" object that controls the behavior of the coroutine and can be used to track its state.

An instance of the promise object is constructed within the coroutine frame for each invocation of a coroutine function. The compiler generates calls to certain methods on the promise object at key points during execution of the coroutine. The code is

```c++
{
    co_await promise.initial_suspend();
    try{
        <body-statements>  // coroutine function body.
    }
    catch(...)
    {
        promise.unhandled_exception();
    }
    FinalSuspend:
    	co_await promise.final_suspend()
}
```

When a coroutine function is called, steps are:

1. Allocate a coroutine frame using operator new (optional).
2. Copy any function parameters to the coroutine frame.
3. Call the constructor for the promise object type, P.
4. Call the **promise.get_return_object()** method to obtain the result to return to the caller when the coroutine first suspends. Save the result as local variable.
5. Call the promise.initial_suspend() method and co_await the result.
6. When the co_await.initial_suspend() expression resumes, then the coroutine starts executing the coroutine body statements that you wrote.

Additional steps executed when reaches a **co_return** statement:

1. Call promise.return_void() or promise.return_value(\<expr>)
2. Destroy all variables with automatic storage duration in reverse order they were created.
3. Call promise.final_suspend() and co_await the result.

if instead, execution leaves \<body-statements> due to an unhandled exception then:

1. catch the exception and call promise.unhandled_exception from within the catch-block.
2. Call promise.final_suspend() and co_await the result.

Destroying the coroutine frame involves:

1. Call the destructor of the promise object.
2. Call the destructor of the function parameter copies.
3. Call operator delete to free the memory used by the coroutine frame.
4. Transfer execution back to the caller/ resumer.

#### Customizing coroutine frame memory allocation

Extra work is needed to make a copy of the allocator inside the allocated memory so you can reference it in the corresponding call to operator delete since the parameters are not passed to the corresponding operator delete call. **This is because the parameters are stored in the coroutine frame and so they will have already be destructed by the time that operator delete is called.**

```c++
template<typename ALLOCATOR>
struct my_promise_type
{
    // customize the operator new
    template<typename... ARGS>
    void* operator new(std::size_t sz, std::allocator_arg_t, ALLOCATOR& allocator, Args&... args)
    {
        // round up sz to next multiple of ALLOCATOR alignment.
        std::size_t allocatorOffset = (sz + alignof(ALLOCATOR) - 1u) & ~(alignof(ALLOCATOR) - 1u);
        // allcoate space for coroutien frame
        void* ptr = allocator.allocator(allocatorOffset + sizeof(ALLOCATOR));
        
        // take a copy of the allocator
        new(((char*)ptr) + allocatorOffset) ALLOCATOR(allocator);
        return ptr;
    }
    
    void operator delete(void* ptr, std::size_t sz)
    {
        std::size_t allocatorOffset = (sz + alignof(ALLOCATOR) - 1u) & ~(alignof(ALLOCATOR) - 1u);
        ALLOCATOR& allocator = *reinterpret_cast<ALLOCATOR*>((char*)ptr + allocatorOffset);
        
        // move allocator to local variable first so it isn't freeing its own memory from undernneath itself
        ALLOCATOR allocatorCopy = std::move(allocator);
        
        // but don't forget to destruct allocator object in coroutine frame
        allocator.~ALLOCATOR();
        allocatorCopy.deallocate(ptr, allocatorOffset + sizeof(ALLOCATOR)); 
    }
}
```

#### Copying parameters to the coroutine frame

The coroutine needs to copy any parameters passed to the coroutine function by the original caller into the coroutine frame so that they remain valid after the coroutine is suspended.

+ pass by value: parameters are copied to the coroutine frame by calling the type's move-constructor
+ pass by reference: only references are copied into the coroutine frame, not the values they point to.

If any of the parameter copy/move constructor throws an exception then any parameters already constructed are destructed, the coroutine frame is freed and the exception propagates back to the caller.

#### Constructing the promise object.

+ The constructor that accept lvalue reference to each of the copied parameters is called if there is one.
+ If don't, call the default constructor.

#### Obtaining the return object

The control flow can be something like:

```c++
struct coroutine_frame { ... };

T some_coroutine(P param)
{
  auto* f = new coroutine_frame(std::forward<P>(param));

  auto returnObject = f->promise.get_return_object();

  // Start execution of the coroutine body by resuming it.
  // This call will return when the coroutine gets to the first
  // suspend-point or when the coroutine runs to completion.
  coroutine_handle<decltype(f->promise)>::from_promise(f->promise).resume();

  // Then the return object is returned to the caller.
  return returnObject;
}
```

#### The initial-suspend point

The statement **co_await promise.intial_suspend()** is executed after coroutine frame has been initialized and the return object has been obtained.

This allows the author of the promise_type to **control whether the coroutine should suspend before executing the coroutine body that appears in the source code or start executing the coroutine body immediately**.

#### Returning to the caller

When coroutine function reaches its first \<return-to-caller-or-resumer> point (or runs to completion), then the return-object returned from the get_return_object() call is returned to the caller of the coroutine.

**Note that the type of the return-object doesn't need to be the same type as the return-type of the coroutine function. An implicit conversion from the return-object to the return-type of the coroutine is performed if necessary.**

#### Returning from the coroutine using co_return

When reaches a co_return statement, it is translated into either a call to promise.return_void() or promise.return_value(\<expr>) followed by a goto FinalSuspend.

goto FinalSuspend causes all local variables with automatic storage duration to be destructed. Then evaluating co_await promise.final_suspend().

#### Handling exceptions that propagate out of the coroutine body

Exception propagates out of the coroutine body is caught and the promise.unhandled_exception() is called. Call the std::current_exception() to capture a copy of the exception and propose it.

#### The final-suspend point

The execution of  promise.final_suspend allows the coroutine to execute some logic, such as publishing a result, signalling completion or resuming a continuation. Also suspend the coroutine before it is destroyed.

#### How the compiler chooses the promise type

The type of the promise object is determined from the signature of the coroutine by using the **std::experimental::coroutine_traits** class. **The default definition of coroutine_traits template defines the promise_type by looking for a nested promise_type.**

+ For coroutine return-types that you have control over, just define a nested promise_type in the class.

+ For coroutine return-types that don't have control, specialize the coroutine_traits to define the promise type to use without without modifying the type.

  ```c++
  template<typename T, typename... Args>
  struct coroutine_traits<std::optional<T>, Args...>
  {
      using promise_type = optional_promise<T>;
  }
  ```

#### Identifying a specific coroutine activation frame

The coroutine handle is used to identify or refer to a particular coroutine frame. Ways to obtain the coroutine handle:

1. It is passed to the await_suspend() method during a co_await expression
2. If you have a reference to the coroutine's promise object, you can reconstruct its coroutine_handle using coroutine_handle\<Promise>::from_promise()

The coroutine_handle is not an RAII object. Generally use a higher level types that provide RAII semantics for coroutines 

**RAII is Resource Acquisition is Initialization.** 

#### Customizing the behavior of co_await

If the promise type have await_transform defined, the compiler will transform every co_await\<expr> into co_await promise.await_transform(\<expr>).

**It lets you enbale awaiting types that would not normally be awaitable**.

```c++
template<typneame T> 
class optional_promise{
    
    template<typename U>
    auto await_transform(std::optional<U>& value)
    {
        class awaiter
        {
            std::optional<U>& value;
         public:
            explicit awaiter(std::optional<U>& x) noexcept: value(x){}
            bool await_ready() noexcept{
                return value.has_value();
            }
            
            void await_transform(std::experimental::coroutine<>) noexcept{}
            U& await_resume() noexcept{return *value;}
        }
        return awaiter{value};
    }
}
```



#### Customizing the behavior of co_yield

The co_yield keyword is translated into co_await promise.yield_value(\<expr>). Customize the behavior of the co_yield by defining yield_value methods 







