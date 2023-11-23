#Member calls from constructors

In the last session it was mentioned that we can often hear "Don't call virtual functions from the constructor" or even just "don't call member functions from a constructor". Do such advices make sense, and if it is, why?

Technically, you can call any member functions from constructors, but you can be surprised when you call virtual functions or you call plain non-virtual member functions that call virtuals (I think the latter case is even more insidious, since we do not expect plain members making surprises). 

As a starter we take a pretty usual two-class hierarchy illustrating one of the uses of virtual functions:

```c++
#include <iostream>

class Father
{
public:
    void sayAsUsual(void) {std::cout<<"I usually say:"<<std::endl; say();}
    virtual ~Father() = default;
protected:
    virtual void say(void) {std::cout << "I am Your Father" << std::endl;}
};

class Son: public Father
{
public:
    virtual ~Son() = default;
protected:
    virtual void say(void) override {std::cout << "\tNooooooo" << std::endl;}
};
```

as usual, we create a main test function for our snippet (see README.md for their usage).

```c++

int test_member_calls_ctor(void) {
    Father *p_son=new Son;
    p_son->sayAsUsual();
    delete p_son;
    
    return 0;
}
```
And the output is exactly what we would expect:

> I usually say:
> Nooooooo

That's classic polymorphic behavior: the non-virtual **sayAsUsual** calls the virtual **say** according to the actual object type. 
Since our pointer p_son is initialized with a pointer to an object of **Son** class, no wonder that it's the **Son's** version of **say** that's called. 

OK, now let's try to do that virtual call from the constructor. Our new classes are:

```c++
class Vader
{
public:
    Vader() {say();}
    virtual ~Vader() = default;
    void sayAsUsual(void) {std::cout<<"I usually say:"<<std::endl; say();}
protected:
    virtual void say(void) {std::cout << "I am Your Father" << std::endl;}

};

class Luke: public Vader
{
protected:
    virtual void say(void) {std::cout << "\tNooooooo" << std::endl;}
};
```
And our main function is now

```c++
int test_member_calls_ctor(void) {
    Father *p_son=new Son;
    p_son->sayAsUsual();
    delete p_son;

    Vader *p_luke=new Luke;
    p_luke->sayAsUsual();
    delete p_luke;

    return 0;
}
```

So, when we create an object of Luke class, it says "I'm your father". Something's gone really wrong.
But when we call the old **sayAsUsual**, it says "Nooooooo", so our old call of virtual from non-virtual still works.

Maybe it would be better to call that nonvirtual **sayAsUsual** from the ctor?

To make more clear from where which call comes, we will additionally make our classes more talkative. Now when we are in constructor, it prints this fact before calling other functions. 

```c++
class TalkVader
{
public:
    TalkVader() {std::cout<<"I'm Vader, in constructor ";sayAsUsual();}
    virtual ~TalkVader() = default;
    void sayAsUsual(void) {std::cout<<"I usually say:"<<std::endl; say();}
protected:
    virtual void say(void) {std::cout << "I am Your Father" << std::endl;}

};
class TalkLuke: public TalkVader
{
public:
    TalkLuke() {std::cout<<"I'm Luke, in constructor ";sayAsUsual();}

protected:
    virtual void say(void) {std::cout << "\tNooooooo" << std::endl;}
};

```

And our final main function will look like this:

```c++
int test_member_calls_ctor(void) {
 
    Father *p_son=new Son;
    p_son->sayAsUsual();
    delete p_son;

    Vader *p_luke=new Luke;
     p_luke->sayAsUsual();
    delete p_luke;
 
    TalkVader *p_talkLuke=new TalkLuke;
    p_talkLuke->sayAsUsual();
    delete p_talkLuke;
 
    return 0;
}
```

And that final talkative Luke says:
> 
> I'm Vader, in constructor I usually say:
> 	I am Your Father
> 
> I'm Luke, in constructor I usually say:
> 	Nooooooo
> 
> I usually say:
> 	Nooooooo

Now we can clearly see what happens here. Before Luke constructs, Vader does, so before Luke's constructor starts, Vader's must be completed, so it runs before Luke's. And while Vader's constructor runs there is no Luke yet, so the pointer to the Virtual Method Table is still pointing to Vader's VMT. And all virtual calls, direct or via non-virtuals, resolve to Vader's versions of virtual functions. Only before the start of Luke's constructor the VMT pointer is reset to Luke's VMT.

To inspect how virtual mechanism is usually implemented more thoroughly, see `c_structs_ptr` and `c_oop` family of snippets. There we start with pretty naive and simple plain C code to work with different types of structures and then we try and  implement virtual functions in plain C in the manner they are implemented behind the scene in C++.

But right now we can simply answer the question from the beginning: should we avoid calling functions, especially virtual, from the constructors? 

Well, obviously we can do it. In particular, you can freely do it with non-virtual functions that do not call virtuals. In fact, many initialization idioms are based on that (they are less needed since C++11 got delegating constructors, but many use them anyway).   With virtual functions, if you don’t have background in languages such as Java, in which the pointer to VMT is set to the final destination even before the very first constructor in the inheritance hierarchy gets started, it may be that C++ behavior will seem more intuitive for you, it makes sense when you think about it, doesn’t it? When you are in the constructor of a base class, the derived class doesn’t exist yet, so virtual functions are properly belong to the base. Still, it may be confusing to others, especially those whose native language is not C++, so it is better to avoid them unless they are necessary — in which  case we should leave a comment  noting the virtual call, especially if this call is indirect, via a non-virtual. 

