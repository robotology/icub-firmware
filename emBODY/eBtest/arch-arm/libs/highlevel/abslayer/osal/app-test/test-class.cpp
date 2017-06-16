/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// https://developer.arm.com/docs/dui0472/l/c-and-c-implementation-details/c11-supported-features

/*

C++11 supported features

Home » C and C++ Implementation Details » C++11 supported features

10.13 C++11 supported features
ARM Compiler supports a large subset of the language features of C++11.

Fully supported C++11 features
ARM Compiler fully supports the following language features as defined by the C++11 language standard:
auto can be used as a type specifier in declaration of a variable or reference.
constexpr.
Trailing return types are allowed in top-level function declarators.
Variadic templates.
Alias and alias template declarations such as using X = int.
Support for double right angle bracket token >> interpreted as template argument list termination.
static_assert.
Scoped enumerations with enum classes.
Unrestricted unions.
Extended friend class syntax extensions.
noexcept operator and specifier.
Non-static data member initializers.
Local and unnamed types can be used for template type arguments.
Use of extern keyword to suppress an explicit template instantiation.
Keyword final on class types and virtual functions.
Keyword override can be used on virtual functions.
Generation of move constructor and move assignment operator special member functions.
Functions can be deleted with =delete.
Raw and UTF-8 string literals.
Support for char16_t and char32_t character types and u and U string literals.
C99 language features accepted by the C++11 standard.
Type conversion functions can be marked explicit.
Inline namespaces.
Support for expressions in template deduction contexts.


Partially supported C++11 features with restrictions

ARM Compiler partially supports the following language features. You can use the feature, but restrictions might apply.
nullptr.
ARM Compiler supports the keyword nullptr. However, the standard library header file does not contain a definition of std::nullptr_t. It can be defined manually using:
namespace std
{
	typedef decltype(nullptr) nullptr_t;
}
Rvalue references.
ARM Compiler supports rvalue references and reference qualified member functions. However, the standard library provided with ARM Compiler does not come with an implementation of std::move or std::forward.
Both std::move and std::forward are a static_cast with the target type deduced via template argument deduction. An example implementation is as follows:
namespace std 
{
	template< class T > struct remove_reference      {typedef T type;};
	template< class T > struct remove_reference<T&>  {typedef T type;};
	template< class T > struct remove_reference<T&&> {typedef T type;};
						
	template<class T> 
	typename remove_reference<T>::type&&
	move(T&& a) noexcept
	{
		typedef typename remove_reference<T>::type&& RvalRef;
		return static_cast<RvalRef>(a);
	} 
	template<class T> 
	T&&
	forward(typename remove_reference<T>::type& a) noexcept
	{
		return static_cast<T&&>(a);
	} 
}
ARM Compiler does not implement the C++11 value categories such as prvalue, xvalue and glvalue as described in A Taxonomy of Expression Value Categories. Instead it implements the draft C++0x definitions of lvalue and rvalue. In rare cases this may result in some differences in behavior from the C++11 standard when returning rvalue references from functions.
Initializer lists and uniform initialization.
ARM Compiler supports initializer lists and uniform initialization, but the standard library does not provide an implementation of std::initializer_list. With a user-supplied implementation of std::initializer_list initializer lists and uniform initialization can be used.
Lambda functions
ARM Compiler supports lambda functions. The standard library provided with the ARM Compiler does not provide an implementation of std::function. This means that lambda functions can only be used when type deduction is used.
Within a function auto can be used to store the generated lambda function, a lambda can also be passed as a parameter to a function template, for example:
#include <iostream>		
template<typename T> void call_lambda(T lambda)
{
	lambda();
}
void function()
{
	auto lambda = [] () { std::cout << “Hello World”; };
	call_lambda(lambda);
}
Range-based for loops.
ARM Compiler supports range-based for loops. However an implementation of std::initializer_list is required for range-based for loops with braced initializers, for example:
for(auto x : {1,2,3}) { std::cout << x << std::endl; }
Decltype
The decltype operator is supported, but does not include the C++11 extensions N3049 and N3276. This means that decltype cannot be used in all places allowed by the standard. In summary the following uses of decltype are not supported:
As a name qualifier, for example decltype(x)::count.
In destructor calls, for example p->~decltype(x)();.
As a base specifier, for example class X : decltype(Y) {};.
decltype construct cannot be a call to a function that has an incomplete type.
C++11 attribute syntax
ARM Compiler supports the [[noreturn]] attribute.
ARM Compiler ignores the [[carries_dependency]] attribute.
Delegating constructors
Delegating constructors are supported by the compiler. However when exceptions are enabled you must link against up-to-date C++ runtime libraries.
Support of =default for special member functions
Special member functions can be explicitly given default implementation with =default. ARM Compiler does not support this for the move constructor or move assignment operator special member functions. All other special member functions are supported. For example:
struct X 
{
	// The constructor, destructor, copy constructor 
	// and copy assignment operator are supported
	X() = default;
	~X() = default;
	X(const X&) = default;
	X& operator=(const X&) = default;
	
	// The move constructor and move assignment operator are not supported
	X(const X&&) = default; 
	X& operator=(const X&&) = default;
};


Unsupported C++11 features

The following language features are not supported in any way by ARM Compiler:
C++11 memory model guarantees.
The alignof operator and alignas specifier.
Inheriting constructors.
Thread local storage keyword thread_local.
User-defined literals.
Smart pointers.
Additions to the C++11 standard template library. ARM® Compiler 5 includes only the C++03 standard template library.

*/

/*
overriding new /  delete etc.

http://stackoverflow.com/questions/583003/overloading-new-delete

*/ 

//C++98/03: __cplusplus is 199711L
 
    
//C++11: __cplusplus is 201103L.

#if(__cplusplus == 199711L)
     #warning we are compiling in c++03 mode (--cpp)
#elif(__cplusplus == 201103L)
    #warning we are compiling in c++11 mode (--cpp11)
#else
    #warning we are compiling in unknown c++ mode
#endif


#include "test-class.h"


#include "stdlib.h" 
#include "stdint.h" 
#include "osal.h"

#include <cstring>

#include <vector>

#include <new>

// see: http://stackoverflow.com/questions/583003/overloading-new-delete
// also: http://www.cplusplus.com/reference/new/operator%20new/
// also: http://www.cplusplus.com/reference/new/operator%20delete[]


// about new and delete:
// 1. armcc uses an old \include\new file which is c++98 with the following 
//    normal, nothrow and placement versions of operators new and delete:
//
//    void *operator new (std::size_t size) throw(std::bad_alloc);
//    void *operator new(std::size_t, const std::nothrow_t&) throw();
//    inline void *operator new(std::size_t, void* __ptr) throw() { return __ptr; }
//
//    void operator delete(void*) throw ();  
//    void operator delete(void*, const std::nothrow_t&) throw();
//    inline void operator delete(void*, void*) throw() { /* nothing */ }
// 
// 2. examples of use are the following
//    MyClass * p2 = new MyClass;
//    MyClass * p3 = new (std::nothrow) MyClass;
//    the placement version is used ... to construct an object on memory that is already allocated
//    char *buf  = new char[sizeof(string)]; // pre-allocated buffer
//    string *p = new (buf) string("hi");    // placement new
 
//
// 3. the c++11 versions are not available. sic!
//    
//    
// 4. it is enough to redefine the normal versions of new and delete plus the nothrow version of new    
//    - we DONT and CANT redefine the placement version.
//    - the new[] and delete[] eventually calls the normal new and delete with proper values.
//    - the nothrow version of delete cannot be called by operator delete.


void *operator new(std::size_t size) throw(std::bad_alloc)
{
    void* ptr = osal_base_memory_new(size);
    return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t& nothrow_value) throw()
{
    void* ptr = osal_base_memory_new(size);
    return ptr;    
}


////void* operator new[] (std::size_t size)
//void *operator new[](std::size_t size) throw(std::bad_alloc)
//{
//    void* ptr = osal_base_memory_new(size);
//    return ptr;    
//}

void operator delete(void* mem) throw ()
{
    osal_base_memory_del(mem);
}

//void operator delete (void* mem, const std::nothrow_t& nothrow_constant) throw()
//{
//    osal_base_memory_del(mem);
//}

//void operator delete[] (void* mem)
//{
//    osal_base_memory_del(mem);
//}


//namespace std
//{
//    typedef decltype(nullptr) nullptr_t;
//}


// - class Widget1

Widget1::Widget1()
{   
    one = 1;
    two = 2;
}

Widget1::~Widget1()
{   
    one = 0;
    two = 0;
}


void Widget1::initialise(void)
{ 
    one = __cplusplus;
    two = 0;
}    

void Widget1::increment(void)
{ 
    one++;
}  

void Widget1::decrement(void)
{ 
    if(one > 0)
    {
        one--;
    }
}  

uint32_t Widget1::read(void)
{ 
    return one;
} 

// - class Widget2

struct Widget2::Impl
{
    uint32_t one;
    uint32_t two; 
    std::vector<uint8_t> v8;
//    uint32_t* p32 = NULL;
//    uint8_t* p8 = nullptr;
    Impl() 
    {
        one = 1;
        two = 2;
        v8.reserve(16);
    }
    ~Impl()
    {
        v8.clear();
    }
};

Widget2::Widget2()
: pImpl(new Impl)
{   

}

Widget2::~Widget2()
{   
    delete pImpl;
}


void Widget2::initialise(void)
{ 
    pImpl->one = 0;
    pImpl->two = 0;
}    

void Widget2::increment(void)
{ 
    pImpl->one++;
    pImpl->v8.push_back(pImpl->one);
}  

void Widget2::decrement(void)
{ 
    if(pImpl->one > 0)
    {
        pImpl->one--;
        pImpl->v8.pop_back();
    }
}  

uint32_t Widget2::read(void)
{ 
    return pImpl->one;
} 



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






