# Lambdas

After our first introduction to lambdas, a number of questions arose, such as how do we return lambdas, can we store them in a container for later use, and why and when should we be careful with capturing values by reference?

Well, the answer to the latter is "anytime it is possible that it can be stored or returned". Your local function-level variables are located on stack (if not in a register), so references and pointers to them exist till the return from the function in which they were defined. So we must be sure that lambdas that have captures by reference are not used after that return.

To illustrate that, as well as both of the previous questions, let's make a simple lambda-based calculator that doesn't calculate and display results immediately, but stores lambdas with captured inputs to be run later.

```c++
#include <iostream>
#include <list>
#include "test_lambdas.hpp"

using std::cout, std::cin, std::endl;


std::function<void()> arithmet(int &x, int &y, char &op)
{
    auto print_head = [=](){cout<<x<<op<<y<<"=";};
    switch (op)
    {
        case '+':
            return [=]()->void {print_head(); cout<<x+y<<endl;};
        case '-':
            return [=]()->void {print_head(); cout<<x-y<<endl;};
        case '*':
            return [=]()->void {print_head(); cout<<x*y<<endl;};
        case '/':
            return [=]()->void {print_head(); cout<<x/y<<endl;};
        case '%':
            return [=]()->void {print_head(); cout<<x%y<<endl;};
        default:
            return [=]()->void {print_head(); cout<<"unknown operator "<<op<<endl; };
    }
}

int test_lambdas(void)
{

    std::list<std::function<void()> > funcs;
    int x=0;
    int y=0;
    char op=' ';
    cout<<"\n\nType arithmetic operations using + - * / % operators and integer numbers\n";
    cout<<"for example: 64*25 [enter] 78-1 [enter] 117%3 [enter\n";
    cout<<"Type EOF (Ctrl-Z in Windows, Ctrl-D in Linux and MacOS) or a wrong input to exit"
        <<endl;
    while(cin>>x>>op>>y)
    {
        funcs.push_back(arithmet(x,y,op));
    }
    cout<<"\n\n results:\n"<<endl;
    for(auto &func: funcs)
    {
        func();
    }
    
    return 0;
}
```

And as you can see, we can do pretty much with lambdas. Not only we return them and store into a container, our computational lambdas capture not only parameters of function (`x, y, op`), but also another lambda -- `print_head` (which before that had captured all those params for itself). 

An example of input and output:
> 118*4  
> 28/7  
> 3452+345  
> 134%3  
> 12345&435  
> q  
> 
> 
>  results:
> 
> 118*4=472  
> 28/7=4  
> 3452+345=3797  
> 134%3=2  
> 12345&435=unknown operator &  

Note that we captured everything (including lambda) by value. What if we capture everything by reference, replacing all `[=]` with `[&]`?

It compiles, but the same input will give everyone his own output, my is:
> 281314120@164416=0  
> 281314120\300164544=0  
> 281314120\330164056=435  
> 281314120@164672=0  
> 281314120\300164800=unknown operator &  

That's what happens with dangling references (and pointers). Our `print_head` lambdas are called using bad address stored on the dead stack of `arithmet` function. And that's first and main danger of capturing by reference: if we return or store a reference, it may dangle, and that's UB. 

But what if we capture `print_head` by value, but still capture `x, y` parameters by reference? These references don't dangle until we exit `test_lambdas`/`main`, so we are safe, aren't we?
We try it, but for debug purposes we capture them by value in `print_head` and by reference in calculating lambdas, and do our own printing of captured values there as well.

```c++
std::function<void()> arithmet(int &x, int &y, char &op)
{
    auto print_head = [=](){cout<<x<<op<<y<<"=";};
    switch (op)
    {
        case '+':
            return [&,print_head]()->void {
                print_head();
                cout<<"Captured "<<x<<"+"<<y<<'='<<x+y<<endl;};
        case '-':
            return [&,print_head]()->void {
                print_head();
                cout<<"Captured "<<x<<"-"<<y<<'='<<x-y<<endl;};
        case '*':
            return [&,print_head]()->void {
                print_head();
                cout<<"Captured "<<x<<"*"<<y<<'='<<x*y<<endl;};
        case '/':
            return [&,print_head]()->void {
                print_head();
                cout<<"Captured "<<x<<"/"<<y<<'='<<x/y<<endl;};
        case '%':
            return [&,print_head]()->void {
                print_head();
                cout<<"Captured "<<x<<"%"<<y<<'='<<x%y<<endl;};
        default:
            return [&,print_head]()->void {
                print_head();
                cout<<"unknown operator "<<op<<endl; };
    }
}
```
The same input gets the following output now:
> 118\*4=Captured 0*435=0  
> 28/7=Captured 0/435=0  
> 3452+345=Captured 0+435=435  
> 134%3=Captured 0%435=0  
> 12345&435=unknown operator &  

All our lambdas get 0 as x and 435 as y. Why? These are the last values set on our variables local in `test_lambdas`/`main`. Our penultimate input set x to 12345 and y to 435, and the last input was "q", which reset x to 0, since it's not a valid input. 

So despite our newest version having no dangling references, the result is still wrong, now purely logically, without UB. We shared our objects `x` and `y` between multiple different lambdas instead of storing their values at the moment of creation of each lambda. 

It's easy to forget about these dangers if we do not internalize what capturing by reference means. We often pass parameters to functions, including class constructors, by reference, but here we not only pass captures by reference, we **store** them by reference! Consider this functor analog to our lambdas (for simplicity it doesn't capture  `print_head` lambda):

```c++
class Adder
{
public:
    Adder(const int &x_, const int& y_): x(x_), y(y_){}
    void operator()(){cout<<x<<"+"<<y<<"="<<x+y<<endl;}
    
private:
    const int &x;
    const int &y;
};
```
We almost never use references as class members, but we often use pointers in that role, and we remember that one of the things we must keep in mind is whether we are sharing the object that pointer points to with another pointer. Sometimes that's exactly what we need, and that's why we bothered with pointers in the first place. But sometimes we have other reasons to store our objects in the heap, and sharing can become a thing to avoid. The same is with captures by reference. 

Later, discussing lambdas in the chat, we've got an additional question: can lambdas be recursive? First attempts that our students have tried do not compile, even those that on first glance look OK:

```c++
auto fctor = [](int n) {
        return  n * fctor(n - 1);
    };
```
What's wrong with that (apart of the lack of handling of n==1)? Compiler says *Variable 'fctor' declared with deduced type 'auto' cannot appear in its own initializer*, so it's basically a kind of circular definition. We know how to handle circular definitions — indirection usually does the trick. But here it doesn't work neither when we try to capture `fctor` by value nor by reference, and we don't even have an idea how could we make a reference to it a parameter in C++11. 

```c++
//these do not compile either
auto fctor = [fctor](int n) {
        return  n * fctor(n - 1);
    };
auto fctor = [fctor&](int n) {
        return  n * fctor(n - 1);
    };
```

So, what prevents us from capturing lambda's self or using it is a parameter? It is type. Lambda has anonymous unique one-time type -- that's why we usually define lambda objects with `auto`, and when we cannot (like with our vector) we assign lambdas to an instantiation of `std::function` type. We can use this technique here:

```c++
    std::function<int64_t(int64_t)> fcl = [&fcl](int64_t n)->int64_t {
            return n>1?n*fcl(n-1):1;
        };
    for(int i=1;i<21;++i)
    {
        cout<<fcl(i)<<endl;
    }
```
Here we recurse via captured `std::function` object which holds our lambda. Looks nice and clean, but "clean code — horrible performance" works in this case, the combination of comparatively heavy `std::function` and passing through capture give us performance penalty that, while not so horrible with full optimizations, still significant enough to look for alternatives.  

What we do generally when we want performance from callables? We make them a template parameter, like in standard sorting algorithms. This way, we get performance together with the flexibility — the code, be it a plain old function, a callable objects' `operator()`, or a lambda body, would probably be inlined, so the speed would be the best of them all. But in C++11 we had no generic lambdas, they came with C++14 in the form of so-called *invented template parameter* when we specify a parameter type as `auto`. 

```c++
    auto fctrl=[](int64_t n, auto& fctrl_ref)->int64_t {
        return n>1?n*fctrl_ref(n-1, fctrl_ref):1;
    };

    for(int i=1;i<21;++i)
    {
        cout<<fctrl(i, fctrl)<<endl;
    }

```
This is quite fast and nice, but to understand how this code works we have to twist our brains quite a bit. Our lambda is essentially a template lambda that is templated on a reference to its own type. It instantiates when we use it, in the `fctrl(i, fctrl)` statement. 

The last question that arose by the end of our callable objects session was "can't we utilize the C++ ability to assign default values to function parameters to make function pointer calls more flexible?". No, it just doesn't work. 

Let's define a simple function taking a default argument:

```c++
int addints(int lhs, int rhs=1)
{
    return lhs+rhs;
}

typedef int (*p_int_2int)(int, int);
```
*Note: we usually use "using" instead of "typedef" nowadays, they are usually easier to read and understand. With function pointers, though, neither is better than the other.*

We can call it via function pointers accepting two arguments:

```c+++
void pointer_call(p_int_2int func)
{
    cout<<"\npointer_call (4,4): "<<func(4,4)<<endl;
}
```
We can't compile a similar call to it via the same function pointer:

```c++
//Won't compile:
void pointer_call_with_def_arg(p_int_2int func)
{
    cout<<"\ncalling func with just one arg: "<<func(5)<<endl;
}
```

We can try to do it via a function pointer with a single-parameter signature:

```c++
typedef int (*p_int_1int)(int);
void one_arg_pointer_call(p_int_1int func)
{
    cout<<"\none argument pointer call (5): "<<func(5)<<endl;
}
```
And that compiles, but when we try to pass our function to it in the `test_lambdas/main`, it fails:

```c++
	//these calls are ok:
    cout<<"testing addints:\n"<<"Normal call (5): "<<addints(5);
    pointer_call(addints);
    //that won't compile:
    //one_arg_pointer_call(addints);
```
it says
> Candidate function not viable: no known conversion from 'int (int, int)' to 'p\_int\_1int' (aka 'int (*)(int)') for 1st argument

So it sees our `addints` as a function with two parameters, not one. Why? We can call it directly with only one argument, why can't we do the same via function pointer?

The answer is obvious if we look at the assembler code for the normal call with one parameter `addints(5)` (you can do it in your IDE or on Godbolt's. For those who can't read assembler: it actually calls it with 2 arguments, 5 and 1. That "1" is added when we compile the call. But calls via function pointer happen in runtime. Compiler can't add the second parameter there. And, of course, it won't let you to call this function via one-parameter function pointer: the function is still two-parameter. 

So, to compare our callable objects — function pointers, bindings, callable classes aka functors, lambdas — aren't lambdas too complex? They can be, but so can be all the others. Actually, although lambdas came from the Functional Programming (FP) paradigm, they were widely accepted by C++ programmers of other ways, too. While complex lambdas can be difficult, they are simply great when you need a callback that is a one-liner. To declare a full-fledged functor or even a function and a pointer to it is often too much bother compared to lambda. On the other side, if you struggle with your lambda, maybe you should consider rewriting it into a function or a functor. 
