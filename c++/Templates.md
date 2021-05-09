原文链接：https://blog.csdn.net/harman_zjc/article/details/70255960



### 													模板

#### 模板特化和偏特化:(template specialization and partical specialization)

##### 隐式特化 (Implicit Specialization):

模板本身并不是一个类型或对象或其他任何实体。必须通过生成具体的类后才能使用，这个过程就是特化。特化就是根据模板实参来生成相应的，具体的类或者函数。

对于类模板而言，使用的时候所有的模板参数是必须明确指定的，当调用的是函数模板时，会根据传入的函数参数推导出模板实参。以上的方式是隐式特化

##### 显示特化 (Explicit Specialization)：

为某种类型做专门处理，既重新对某种类型定制一份新的定义，其他类型仍旧按照原有的类模板的定义来生成特化，这种方式就是模板的显示特化，或者叫全特化。

```c++
// 原模板
template<class T> 
struct PrimaryDef{
    void f(){std::cout<<1<<std::endl;}
    void g(){std::cout<<2<<std::endl;}
};

//针对bool类型的显示特化
template<>
struct PrimaryDef<bool>{
    void f(){std::cout<<"bool"<<std::endl;}
}
```

**显示特化就是全特化**。

##### 偏特化 （partial specialization):

用来部分特化，也就是特化一类情况或者模板参数列表中有部分参数可以被确定的时候，只能用于类的模板特化。

```c++
template<typename T>
class A{}

// partical specialization
template<typename T, int N> // 有内容，全特化后边是没有内容的
class A<T[N]>
{}

// 一种偏特化的应用
template<class> class function;  /*undefined*/
//一个偏特化版本的定义
template<class R, class... Args> class<R(Args...)>;
```

#### 实例化

类模板本身既不是一个类型也不是一个对象，必须要在实例化后才能真正使用它，实例化就是提供模板的全部参数，编译器根据所提供的参数生成具体的代码。**特化根据模板参数来确定具体的代码，实例化是把具体的代码编译进去。**

##### 显示实例化：

template class-name<template arguments>。主要作用是告知编译器将类模板的一个特化版本的代码编译进去。可实例化的部分有：class， Function， Member Function， Member classes， static data members of class templates.

##### 隐式实例化：

当代码中引用了一个模板，且需要一个完整定义的类型的时候，或者当类型的完整性影响到了代码，并且此时特定的类型没有被显示实例化，那么隐式实例化就会发生。

```c++
template<class T> struct Z{
	void f(){};
    void g(); // never defined
}

template struct Z<double>; // explicit instantiation.
Z<int> a;   // implicit instantiation of Z<int>
Z<char>* p; // nothing is instantiated here.
p->f();  // implicit instantiation of Z<char> and Z<char>::f() occurs here.
		// Z<char>::g is never needed and never instantiated here.
   
```

只有当用到模板类的时候才会实例化。**显示的特化总是伴随着实例化**。

##### 实例化的作用：

+ 模板的实现和定义分开在不同的文件中。当编译器在编译一个cpp文件的时候，用到一个类型只要是声明了，要么在自己里面定义，要么在别的地方定义，所以不会报错。链接的时候回查找相关的目标文件已找到所需的定义。如果一个头文件声明了template.h,定义在template.cpp中，使用在reference.cpp中。编译template.cpp的时候，由于没有在该文件中实例化它所以模板不会被编译进去。在reference.cpp中，由于看不到template.cpp定义，用到其模板如Test<int>的时候编译器认为是别处定义的，也不会实例化它，进而报错。所以模板的定义和实现都是放在头文件里面。若要分开，可以在reference.cpp中显示实例化，这样能保证编译进去。

+ 单独显示实例化一个函数的作用。

  模板编译的时候，按需编译。链接的时候有重复会进行合并。如果有A.cpp和B.cpp两个类，A中定义了一个Test<int>对象，将其传入到B的一个接受Test<int>类型的指针的函数时，对B而言不需要Test的构造和析构函数了，只需实例化testInterface即可。

  ```c++
  #include<iostream>
  template<typename T> 
  class Test
  {
   public:
      void testInterface()
      {
          std::cout<<1;
      }
  };
  //特化，伴随着实例化
  template<>
  void Test<int>::testInterface()
  {
      std::cout<<2;
  }
  ```

  

#### 模板参数的三种形式：

```c++
template<typename T> className
```

上面的定义中，T就是一个模板参数。使用模板的时候，需要指定该模板的所有参数以生成模板的一个特化。指定的方法可以不是显示指定，但必须可推导。

+ 类型参数：模板参数是一个类型。使用类型参数时，不能用它做没有定义的工作。

  ```c++
  template<class T>
  class Test
  {
  public:
      T tObj;
      T* tPtr;
      std::list<T> l;
      T* createInsideObj();
  }
  
  class A
  {
      A(){}
  }
  ```

  对于类A来说，其默认构造函数是隐式的，所以模板编译不会通过。如果去掉tObj,tPtr的定义，只要不调用createInsideObject，就不会报错，因为不会实例化。

+ 非类型模板参数：可用作非类型参数的有

  + integral
  + pointers to objects
  + pointers to functions
  + lvalue reference paramters
  + pointer to members
  + enumeration tpe
  + nullptr

  ```c++
  int ai[5];
  template<const int* pci> struct X{}
  X<ai> xi; // conversion occures.
  
  struct Y{
      void func(){};
  }
  
  struct YY{
      static Y y;
  }
  
  Y YY::y;
  
  template<const Y* b> struct Z{}
  Y y;
  Z<&YY::y> z;
  
  tempalte<int (&pa)[5]> struct W{}
  W<ai> w; // no conversion
  
  void f(char);
  void f(int);
  
  template<void (*pf)(int)> struct RR{}
  RR<&f> a;   // select f(int);
  
  template<void (Y::*pf)()> struct RD{}
  RD<&Y::func> rd;
  
  ```

  对于指针类型和引用类型，用来赋值的对象必须是有链接的。

+ 模板参数

  ```c++
  template<class T, int a> class A{}
  
  template<template<typename, int> class V>
  class C
  {
   public:
      V<int, 5> y;
  }
  
  int main()
  {
      C<A> aa;
  }
  ```

##### 函数模板

不同的模板参数对应了不同的模板实例，而模板本身是否使用这些模板参数完全就是需求上的问题了，对于模板来讲如果没有在定义中用到模板参数那么每个版本的定义时是没有差异的，但是他们是不同的实例，而且实例间毫无关系。

##### 函数模板的实例化

函数模板也存在现实实例化和隐式实例化。现实实例化语法：

```c++
// declaration of template function
template<typename T, int N>
void function()
{}

// explicit instantiation
template void function<int, 10>();

// second type
template <typename T>
void func(T arg)
{}
// instantiation
template void func(int);
template void func<>(int);
```

对于第二种实例化方式，函数模板中其模板参数T在函数中给函数参数做了类型，所以只要函数类型确定了，模板参数也就可以确定，所以同样能实例化，小括号<>要不要都可以。

**函数模板不支持偏特化**，如果在使用上遇到这样的情况通过**函数重载**来解决。

##### 函数模板的实参演绎（Template argument deduction）

In order to instantiate a function tempalte, every template argument must be known, but not every template has to be sepcified. When possible, the compiler will deduce the missing template argument from the function arguments. This occurs when a function call is attemped, when an address of a function template is taken, and in some other contexts.

```c++
template<typename To, typename From> To convert(From f);

void g(double d)
{
    int i = convert<int>(d); // deduce To from int, From to double
    char c = convert<char>(d);
    int(*ptr)(float) = convert;
}
```

实参演绎中如果推导出来的结果间有矛盾会出现匹配失败的情况，**匹配时没有隐式转型**。匹配失败并不会被看成一个错误。

对于函数的模板是一个函数指针类型时，如果传入的实参是一组重载函数，那么如果仅有一个函数可以最佳配得参数类型的时候就会以此函数版本作为参数调用，否则会报错。

```c++
template<class T> int f(T(*p)(T));
int g(int);
int g(char);

f(g); // the first is used
```

在进行模板参数演绎以前，编译器通常会做一些工作让匹配更容易。当函数Parameter不是一个引用类型时：

+ Argument是一个数组类型，那么Argument被替换为该数组成员类型的指针类型。
+ Argument是一个函数类型，将被替换为该函数的指针类型。
+ Argument是CV限定类型(const或volatile)，忽略其cv属性。

```c++
template<class T> void f(T);
int a[3];
f(a);  // adjusted to int*, deduce T = int*

const int b = 13;
f(b); // adjust to int, deduce T = int, ignore CV

void g(int);
f(g); // adjust to void (*)(int), deduce T = void(*)(int)
```

+ 如果模板parameter是一个CV限定类型，则忽略其CV属性

+ 如果模板parameter是一个引用类型，P将被修正为其引用的类型。

+ 如果模板parameter是一个非CV限定的右值引用，而传入的实参Argument是一个左值，那么A将被修正为该左值类型的左值引用

  ```c++
  template<class T>
  int f(T&&);  // P is an rvalue reference to cv-unqualified T
  
  template<class T>
  int g(const T&&);  // P is an rvalue reference to cv-qualified T
  
  int main()
  {
      int i;
      int n1 = f(i);  // argument is lvalue: calls f<int&>(int&);
      int n2 = f(0);  // argument is not lvalue: calls f<int&>(int&&);
      
      // int n3 = g(i); // error: deduces to g<int>(const int&&), which can not bind an rvalue reference to an lvalue.
  }
  ```

+ 推倒后的类型可以比实参类型A多顶层的CV限定

  ```c++
  template<typename T> void f(const T& t);
  bool a = false;
  f(a); // P = const T&, adjusted to const T, A = bool; deduced T = bool, deduced A = const bool, deduced A is more cv-qualified that A
  ```

+ 如果传递的实参类型可以通过限定转型（qualifier conversion）转换成推导后的类型，也是允许的

  ```c++
  template<typename T> void f(const T*);
  int *p;
  f(p); // P = const T*, A = int*; deduced T = int, deduced A = const int*
  		// qualification conversion applies.(from int* to const int*)
  ```

+ 如果P是一个模板类型的引用或者直接引用，而传递的A是P的子类模板类型，同样可以。

  ```c++
  template<class T> struct B{};
  template<classs T> struct D: public B<T>{};
  template<class T> void f(B<T>&){}
  
  int main()
  {
      D<int> d;
      f(d); //子类转换
  }
  ```

+ 如果P是一个嵌套名说明符(nested-name-specifier)，那么其所含的模板参数是不可推导的。嵌套名称说明符就是用来限定作用域的::符号左边部分，比如std::cout中的std就是嵌套名说明符。

  ```c++
  template<typename T> struct identity {typedef T type;};
  template<typename T> void bad(std::vector<T> x, T value=1);
  template<typename T> void good(std::vector<T> x, typename identity<T>::type value = 1);
  
  std::vector<std::complex<double>> x;
  bad(x, 1.2); // P1 = std::vector<T>, A1 = std::vector<std::complex<double>>
  			// deduce T = std::complex<double>
  			// P2 / A2: deduced T = double
  			// error: deduction fails, T is ambigulous
  
  good(x, 1.2);  // P1 / A1: deduced T = std::vector<std::complex<double>>
  			   // P2 = identity<T>::type, A2 = double,
  // P2 / A2: used T deduced by P1 / A1 because T is to the left of :: in P2
  // OK, T = std::complex<double>
  ```

+ 如果非类型模板参数存在于子表达式中，则模板参数不可推导

  ```c++
  template<std::size_t N> void f(std::array<int, 2 * N> a);
  std::array<int, 10> a;
  f(a); // P = std::array<int, 2*N> a = std::array<int, 10>
  	// 2 * N is non-deduced context, N cannot be deduced
  	// note: f(std::array<int, N> a) would be able to deduce N
  ```

+ 函数的默认参数不能用来推导T

  ```c++
  template<typename T, typename F>
  void f(const std::vector<T> &v, const F&comp = std::less<T>());
  std::vector<std::string> v(3);
  f(v); // P2 is non-deduced context for F;
  ```

+ 如果A是一组重载函数，且存在多于一个的匹配或不存在匹配时，则T不可推导

  ```c++
  template<typename T> void out(const T &value){}
  out("123"); // P = const T&, A = const char[4] lvalue, deduced T = char[4]
  out(std::endl);  // A is function template, T is non-deduced context.
  ```

+ 如果A是一个初始化列表，而P不是std::initializer_list类型或其引用类型，则P中的T不可推导

  ```c++
  #include <vector>
  #include <initializer_list>
  
  template<class T> void g1(std::vector<T>);
  template<class T> void g2(std::vector<T>, Tx);
  template<class T> void g3(std::initializer_list<T>);
  
  g1({2,2,3}); // p = std::vector<T>, A = {1, 2, 4}: T is non-deduced context, error here
  
  g2({2,2,4}, 10); // T can be deduced from A1, but can be deduced from A2, passed compile
   
  g3({1, 2, 3}) // P is std::initializer_list<T>, deduced T = int
     
  ```

+ 如果P中包含一个模板参数列表，且有参数扩展包，如果参数扩展包位置不在P的模板参数列表的最后，则不可推导

  ```c++
  template<int...> struct T{}
  
  template<int... Ts1, int N, int... Ts2>
  void good(const T<N, Ts...>& arg1, const T<N, Ts...>&);
  
  template<int... Ts1, int N, int... Ts2>
  void bad(const T<Ts1..., N>& arg1, const T<Ts2..., N>&);
  
  T<1, 2> t1;
  T<1, -1, 0> t2;
  
  good(t1, t2);  // P1 is const T<N, Ts1...>&, A1 is T<1, 2>: deduced N is 1, 				deduced Ts1 = [2],
  				// p2 is const<N, Ts2...>&, A2 is T<1, -1, 0>: deduced N is 					1, Ts2 is [-1, 0]
  
  bad(t1, t2);  // p1 = const T<Ts1..., N>&, A1 = T<1, 2>, pcak extension is not in the last of the parameters, so can't be deduced here.
  
  ```

+ 如果P是一个非引用数组类型，则其主边界不可推导

  ```c++
  template<int i> void f1(int a[10][i]);
  template<int i> void f2(int a[i][20]); 
  template<int i> void f3(int (&a)[i][20]); // p = int(&)[i][20], reference to array
  
  void g()
  {
      int a[10][20];
      f1(a); // ok: deduced i = 20
      f1<20>(a);  // 0k
      f2(a);  // error: i is non-deduced here
      f2<10>(a); // 0K
      f3(a);  // ok, deduced i = 10;
      f3<10>(a); // ok
  }
  ```






### 									可变参数模板

```c++
template<typename T>
T adder(T v)
{
    return v;
}

template<typename T, typename... Args>
T adder(T first, Args... args)
{
    return first + adder(args...);
}
```

typename... Args是**模板参数包**，而Args... args被称为**函数参数包**。可变参数模板以编写递归代码的方式编写：一个基本类型(adder(T v))，以及一个“递归”的通用情形。递归发生在调用adder(args...)中。每次调用，参数包缩短一个参数，最终遇到基本情形。

##### 性能：

可变参数模板在编译时刻预先生成一系列函数，所以最终得到的跟循环没什么两样，所以没有性能问题。

##### 类型安全的可变参数函数

若使用时传入的参数不对，将不能通过编译，所以不会在运行时发生错误。

##### 数目可变域数据结构

定制的数据结构（struct和class）具有编译时刻定义的域，可以表示在运行时增长的类型（std::vector)，如果添加新的域，就必须是编译器看到的东西。可变参数模板使得定义具有任意数目域并且在使用时配置这个数目的数据结构成为可能。例子如下：

```c++
template<class... Ts>
struct tuple{}

template<class T, class... Ts>
struct tuple<T, Ts...> : tuple<Ts...>{
    T tail;
    tuple(T t, Ts.. ts): tuple<ts...>, tail(t){}
}
```

首先定义了一个名为tuple的空的类定义。后跟该定义的一个特化版本，该版本从参数包剥除第一个类型并定义该类型的tail成员，它还派生自用参数包余下部分实例化的类型。

```c++
// 例子
tuple<double, uint64_t, const char*> t1(12.2, 42, "big");
// 上边结构体创建的记录为
struct tuple<double, uint64_t, const char*>: tuple<uint64_t, const char*>
{
    double tail;
}

struct tuple<uint64_t, const char*> : tuple<const char*>
{
    uint64_t tail;
}

struct tuple<const char*> : tuple
{
    const char* tail;
}

struct tuple{}
```

根据c++类成员的分布情况，数据成员的布局将会是 [const char*, uint64_t, double]

##### 访问辅助类的定义

```c++
template<size_t, class> struct elem_type_holder;

// first secialization
template<class T, class... Ts>
struct elem_type_holder<0, tuple<T, Ts...>>{
    typedef T type;
}

// second specilization
template<size_t k, class T, class... Ts>
struct elem_type_holder<k, tuple<T, Ts...>>
{
    typedef typename elem_type_holder<k-1, tuple<Ts...>>::type type
}
```

elem_type_holder是一个可变参数模板，接受一个数字k以及我们感兴趣的tuple类型作为模板参数。这是一个编译时模板元编程构造，它作用在常量及类型上，而不是运行时对象。对一个elem_type_holder<2, some_tuple_type>,扩展为

```c++
struct elem_type_holder<2, tuple<T, Ts...>>{
    typedef typename elem_type_holder<1, tuple<Ts...>::type type
}

struct elem_type_holder<1, tuple<T, Ts...>>{
    typedef typename elem_type_holder<0, tuple<Ts...>>::type type;
}

struct elem_type_holder<0, tuple<T, Ts...>>{
    typedef T type;
}
```

elem_type_holder<2, some_tuple_type>从元组的开头剥除两个类型，并将其类型设置为第三个的类型。

##### get的实现

```c++
template<size_t k, class... Ts>
typename std::enable_if<k==0, typename elem_type_holder<0, tuple<Ts...>::type&>::type
get(tuple<Ts...>& t)
{
    return t.tail;
}

template<size_t k, class T, class... Ts>
typename std::enable_if<k!=0, typename elem_type_holder<K, tuple<T, Ts...>>::type&>::type
get(tuple<T, Ts...>& t)
{
    tuple<Ts...> &base = t;
    return get<k-1>(base);
}
```

enable_if 用于在get的两个模板重载间选择，一个用于看为0时，一个用于剥除第一个类型并递归通用情形。

#### 用于catch-all的可变参数模板例子：

编写一个可以打印标准库容器的函数，能够工作在任意容器上。第一种做法：

```c++
template<template<typename, typename> class ContainerType, typename ValueType, typename AllocType>
void print_contaner(ContainerType<ValueType, AllocType> &c)
{
    for(auto &v : c)
    {
        std::cout<<c<<std::endl;
    }
}
```

对于那些可以通过值类型与分配器类型参数化的模板，vector，list等，可以通过如上方式打印。但是对于map，set类型的模板，其模板参数多余2个，就会出现问题，所以用可变参数模板来实现更好一些。

```c++
template<template<typename, typename...> class ContainerType, typename ValueType, typename... Args>
void print_container(Container<ValueType, Args...>& c)
{
    for(auto &v : c)
    	std::cout<<c<<std::endl;
}
```





### 							SFINAE and enable_if

#### SFINAE

```c++
int negate(int i){
    return -i;
}

template<typename T>
typename T::value_type negate(const T& t)
{
    return -T(t);
}
```

When compiler looks at overload candidates that are templates, it has to actually perform substitution of explicitly specified or deduced types into the template arguments.  For negate(42), when looking for the best overload, all candidates have to be considered, When the compiler considers the templated negate, it substitute the deduced argument type of the call into the template and generates

```c++
int::value_type negate(const int &t)
```

**In c++ 11, it states that when a substitution failure occures, such as the one shown above, type deduction for this particular type fails. There's no error involved. The compiler simply ignores this condidate and looks at the others.** 

If we want to write template that only make sense for some types, we must make it fail deduction for invalid types right in declaration, to cause substitution failure.

#### enable_if - a compile-time switch for templates

```c++
template<bool, typename T = void>
struct enable_if
{};

template<typename T>
struct enable_if<true, T>
{
    typedef T type;
}

// c++ 14 added type alias for convenience
template<bool B, class T>
using enable_if_t = typename enable_if<B, T>::type;

// example
template<class T, typename enable_if_t<std::is_integral<T>::value>*=nullptr>
void do_stuff(T &t)
{
    // an implementation for integral types(int, char, etc..)
}

template<class T, typename enable_if_t<std::is_class<T>::value>*=nullptr>
void do_stuff(T &t)
{
    // an implementation for class types
}
```

