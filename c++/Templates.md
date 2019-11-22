### Templates and Generic Programming

The template parameters can be Nonetype Template parameters. It represents a value rather than a type. Nonetype Template parameters are specified by using a specific type name instead of the class or typename keyword. Values used must be constant expressions, which allows the compiler to instantiate the templates during compile time.

#### Template Compilation

When compiler sees the definition of a template, it does not generate code. It generates code only when we instantiate a specific instance of the templates. So to generate an template instantiation, the compiler needs to have the code that defines a function template or class template member function. As a result, **headers for templates typically include definitions as well as declarations, which is unlike the non-template function or class.**

#### Class Template

Class templates is differ from function templates that compiler can't deduce the template parameter types for a class template. So when using , we must specify the parameter like TemplateClass<TypeName> a to define a class instance. For a given member function, it is declared as

​			*ret_type* func_name(param-list)

The outside definition is:

​	template <typename T> ret_type className<T>::func_name(param_list)

**Note: The declaration and definition of the template function must in the same file.**

Sample code

```c++
#pragma once

template<typename T>
class MyTemp {
public:
	MyTemp();

	T f();

	T g()
	{
		return val + 3;
	}

private:
	T val;
};

template<typename T>
MyTemp<T>::MyTemp() :val(0) {}

template<typename T>
T MyTemp<T>::f() {
	return val * 4;
}
```

#### Template Friendship

##### One-to-One Friendship

```
template <typename> class BlobStr;
template <typename> class Blob;
template <typename T>
bool operator==(const Blob<T>&, const Blob<T>&);
template <typename T>
class Blob{
	friend class BlobPtr<T>;
	friend bool operator==<T>(const Blob<T>&, const Blob<T>&);
}
```

##### General and specific Template Friendship

```c++
template <typename T> class Pal;
class C{
	friend class Pal<C>; // Pal instantiated with class C is a friend of C
    
    // all instances of Pal2 are friends to C;
    // no forward declaration required when we befriend all instantiations
    template <typename T> friend class Pal2;
}

template<typename T> class C2{
    // each instantiation of C2 has the same instance of Pal as a friend
    // declaration needed
    friend class Pal<T>;
    
    // all instances of Pal2 are friends of each instance of C2, prior 			// declaration needed
    template<typename X> friend class Pal2;
    
    // Pal3 is a nontemplate class that is a friend of every instance of C2
    friend class Pal2;
}
```

