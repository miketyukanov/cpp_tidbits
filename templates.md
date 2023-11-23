# Introduction to the simplicity and difficulty of template programming. 

On our first introduction of templates we have for the first time met bugs invited by `using namespace std;` when we tried to define our own `swap()` template function. 

We used one of the obvious solutions: removing that file-level using-directive and replacing it with separate using-declarations such as `using std::cout;` (the easiest one would be to qualify our template calls as `::swap(first, second);` the most compliant to usual industrial coding standards would be prepending each `cout`, `endl` etc. with `std::` ).

In a curious coincidence `swap()` is exactly the same function that is most frequently used as an example for intentional `using namespace std;` : [the copy-and-swap idiom](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom), [Argument-depended (Koenig) lookup](https://en.cppreference.com/w/cpp/language/adl), and the C++ Primer 13.3 all talk about the same thing, but even the last, most beginner-friendly, explanation may sound too advanced for those who are currently much earlier into that book. So let's do something more basic that hopefully helps to understand that (and some other) issues. 

We start where we finished (our final file is `test_templates.cpp`, but it doesn't contain many intermediate stages, so you can just create a new one to get all the errors we had had there): 

```c++
#include <iostream>
using namespace std;

```

here we do two things we usually don't: place templates in a `.cpp` file and do `using namespace std;` at file level. That's just for this exercise, mostly the former makes no sense, and the latter is frowned upon. 

```c++
template<typename T>
void swap(T& l, T&r)
{
    T tmp=l;
    l=r;
    r=tmp;
    cout<<"Local swap was called"<<endl;
}
```
The usual techniques to make sense of what calls what is either running the program in a debugger, or tracing, or both. Here our `cout<<"Local swap was called"` does tracing, and you can use the debugger of your choice to venture into the library code and other places we can't trace. 

```c++
int test_swaps(void)
{
    
    int i1 = 1, i2=2;
    cout<<"ints before swap: \n" << "i1="<<i1<<",i2="<<i2<<endl;
    //swap(i1,i2);
    ::swap(i1,i2);
    cout<<"ints after ::swap(): \n" << "i1="<<i1<<",i2="<<i2<<endl;
    std::swap(i1,i2);
    cout<<"ints after std::swap(): \n" << "i1="<<i1<<",i2="<<i2<<endl<<endl;

    string sOne="StrOne";
    string sTwo="StrTwo";
    cout<<"strings before swap: "<<sOne<<" "<<sTwo<<endl;
    swap(sOne,sTwo);
    cout<<"after std::swap: "<<sOne<<" "<<sTwo<<endl;
    ::swap(sOne,sTwo);
    cout<<"after ::swap: "<<sOne<<" "<<sTwo<<endl;
    sOne.swap(sTwo);
    cout<<"after member-funct swap: "<<sOne<<" "<<sTwo<<endl<<endl;
    return 0;
}

```

As you recall, if we uncomment the line 6 (`swap(i1,i2);`), we get "call to swap is ambiguous" error message. So we have to qualify calls to swap, saying explicitly ::swap to get our function called or std::swap to get the standard library one. But why the same unqualified call `swap(sOne,sTwo);` on line 15 doesn't produce the same "ambiguous" error?

Look what our example prints: 
> strings before swap: StrOne StrTwo  
> after std::swap: StrTwo StrOne

As we see, there is no "Local swap called" between "before"
and "after" messages. So it is the standard library swap that is called here, not ours. Why is it preferred?
 
The short answer is there is a specialization of `std::swap` defined in the `string` header. It's full form is too heavily templated, but in a simplified form it would look like

```c++
void swap(string& lhs, string& rhs) 
{
	lhs.swap(rhs);
}

```

which in turn calls member function `swap`, defined roughly like this: 

```c++
void string::swap(string& other) 
{
	using namespace std;
	swap(size, other.size);
	swap(capacity, other.capacity);
	swap(buffer, other.buffer)
}
```   

(in reality, all the heavy lifting is done by the class `Allocator` that is one of the main template parameters of `basic_string` and it is the default allocator that does real swapping, too).

But what happens if we define our own non-template function for swapping strings?

```c++
void swap(string& lhs, string& rhs)
{
    lhs.swap(rhs);
    cout<<"Local no-template string-specialized swap was called"<<endl;
}
```

and what we get is 
> strings before swap: StrOne StrTwo  
> Local no-template string-specialized swap was called  
> after std::swap: StrTwo StrOne  

which means that our plain old non-templated function has priority over the templated specialization in `std::`, but that function, being a specialized template has priority over our non-specialized template. But what about `int`'s? There our function and `std::swap` are totally equal, none is more specialized than another. As a result, we get an ambiguity.  

Now let's try various swaps with classes, and, since we are into templates, let's return to your first attempt to make a class template and add an `operator<<` to simplify tracing, a freestanding `swap`, more specialized than our other local `swap` since it requires ourclass for arguments, but not completely specialized, since ourclass is itself a template. As with library containers, strings etc. we also add a `swap` member function and define a freestanding `swap` through the member.

```c++
template<typename T>
class ourclass
{
public:
    T getbyIndex(int index);
    void swap(ourclass& other)
    {
        cout<<"ourclass embedded swap"<<endl;
        std::swap(a, other.a);
    }

    T a[3];
};

template<typename T>
T ourclass<T>::getbyIndex(int index)
{return a[index];}

template<typename T>
ostream& operator<<(ostream& os, ourclass<T>  & ocs)
{
    os<<'{';
    for (auto &s : ocs.a)
    {
        os << s << " ";
    }
    os<<'}';
    
    return os;
}

```
With freestanding functions (`operator<<` and `swap`) we have to specify that it is a template, just as we earlier did it for our `getbyIndex` member function. But if we implement a member function inside the class template, we don't need that additional `template<typename T>`'s.
 
Now we add this to our `main` or `test_swaps` function to actually test it in work with different kinds of template arguments:

```c++
    ourclass<int> oci{1,2,3}, oci2{11,22,33};
    cout<<"ourclass<int> bef swap:\n\t"<<oci<<oci2<<endl;
    swap(oci, oci2);
    cout<<"ourclass<int> after swap:\n\t"<<oci<<oci2<<endl;
    std::swap(oci,oci2);
    cout<<"ourclass<int> after std::swap:\n\t"<<oci<<oci2<<endl<<endl;

    ourclass<string> ocs = {"Hi", "Bye", "End"};
    ourclass<string> ocs2 = {"Moin", "Tschüss", "Aus"};
    cout<<"ourclass<string> bef swap:\n\t"<<ocs<<ocs2<<endl;
    swap(ocs, ocs2);
    cout<<"ourclass<string> after swap:\n\t"<<ocs<<ocs2<<endl;
    std::swap(ocs,ocs2);
    cout<<"ourclass<string> after std::swap:\n\t"<<ocs<<ocs2<<endl<<endl;
  
``` 

results are: 
> ourclass<int> bef swap:  
> 	{1 2 3 }{11 22 33 }  
> ourclass embedded swap  
> Specialized swap for ourclass was called  
> ourclass<int> after swap:  
> 	{11 22 33 }{1 2 3 }  
> ourclass<int> after std::swap:  
> 	{1 2 3 }{11 22 33 }  
> 
> ourclass<string> bef swap:  
> 	{Hi Bye End }{Moin Tschüss Aus }  
> ourclass embedded swap   
> Specialized swap for ourclass was called  
> ourclass<string> after swap:  
> 	{Moin Tschüss Aus }{Hi Bye End }  
> ourclass<string> after std::swap:  
> 	{Hi Bye End }{Moin Tschüss Aus }  
> 

What do we see here? First, our class template works. Second, the unqualified call to `swap` calls our partially specialized version which in turn calls the member version of it. Third, when we explicitly call `std::swap`, it doesn't use our member function version of `swap`. 

Anyway, it looks like template programming is actually not that  hard. Just replace a concrete type name with a template parameter — and it works! Really easy! Just one last touch: out data is still public, but in the real world we usually incapsulate data making it private. So we add `private` before `T a[3];`...

...and everything stops to compile, of course. The first error we get is pretty simple and the diagnostics is obvious: "No matching constructor fo initialization of ourclass<int>" means that we cannot just initialize our array now when it's private, we have to provide a constructor. The easiest (and right in our case of fixed array) way would be just to provide a constructor with three arguments for three array fields, but that would better be a homework. Besides, we probably would replace array with `vector<T>` (another homework, eh?), in which case initializing lists would be a way more convenient than variadic arguments. So let's do that — make a constructor using initializer list. Insert this into `ourclass` declaration (inlining) 

```c++
    ourclass(const std::initializer_list<T> &init_list)
    {
        cout<<"init from init_list"<<endl;
        if(init_list.size()>3)
            throw std::out_of_range("Initializer list is too big");
        std::copy(init_list.begin(),init_list.end(),a);
    }
```
and add try-catch blocks in the `main`/`test_swaps` function to handle the exception.

When we add this constructor, compiler stops complaining about construction, but our `operator<<` starts demanding access to the now-private array `a`. That's a no-brainer: we just make it a friend of ourclass, adding this friend-declaration to it: 

```c++
    friend ostream& operator<<(ostream& os, ourclass<T> & ocs);
```

Aaaand... the file compiles! But linker refuses to link, giving a very cryptic error:

> Undefined symbols for architecture x86_64:  
>   "operator<<(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, ourclass<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > >&)", referenced from:  
>       test_swaps() in test_templates.o
>   "operator<<(std::__1::basic_ostream<char, std::__1::char_traits<char> >&, ourclass<int>&)", referenced from:
>       test_swaps() in test_templates.o  
> ld: symbol(s) not found for architecture x86_64  
> clang: error: linker command failed with exit code 1 (use -v to see invocation)  

What does it mean? Well, let's google it, it should very easy to find what leads to that error... But it isn't. Browsing through Stackoverflow and lots of forums and other c++-related sites we can find scores of conditions that give this error, but nothing even remotely relevant to our case. Of course, if you already know what's wrong with the code, even such a useless error message would be of use. But suppose you don't see any mistakes, how to find them?

One way is similar to what we do when a computer starts, but doesn't show anything on the monitor: we connect the monitor to a working computer — does it work? OK, now let's replace the video card — now is it working? Still no? Well, now let's see what we've got with the MB and CPU... We have two suspects: friend declarations and templates. First, we can comment out both friend declaration and `operator<<` and, of course, its use, all those `cout<<"ourclass<int> bef swap:\n\t` etc. Neither compiler nor linker gives us any hard time now, so part of the problem is friend declaration. But it seems to be OK at first glance, how we narrow our search further? Let's make a very similar class, but without templating it:

```c++
class NoTemplate
{
public:
    NoTemplate(const std::initializer_list<string> &init_list)
    {
        cout<<"init from init_list"<<endl;
        if(init_list.size()>3)
            throw std::out_of_range("Initializer list is too big");
        std::copy(init_list.begin(),init_list.end(),a);
    }

    friend ostream& operator<<(ostream& os, NoTemplate  & ntl);
private:
    string a[3];
};

ostream& operator<<(ostream& os, NoTemplate  & ntl)
{
    os<<'{';
    for (auto &s : ntl.a)
    {
        os << s << " ";
    }
    os<<'}';
    return os;
}

``` 
and add this to the `main`/`test_swaps`:

```c++
NoTemplate ntl1{"One", "two", "three"}, ntl2{"Eins", "zwei", "drei"};
cout<<"Notemplate before swap:\n\t"<<ntl1<<ntl2<<endl;
::swap(ntl1,ntl2);
cout<<"Notemplate after swap:\n\t"<<ntl1<<ntl2<<endl<<endl;
```

(we needed to qualify call to swap as `::swap` since we didn't bother to define the specialized swap-function).

It works perfectly. So, while we still have no answer, we narrowed our search down to "friends of class templates". Now we can reread about it paying attention to details, and first we find that in the friend declarations, unlike member function declarations, we must specify that it is a template, i.e., instead of `friend ostream& operator<<(ostream& os, ourclass<T> & ocs);` we do `friend ostream& operator<< <T>(ostream& os, ourclass<T> & ocs);` or simply `friend ostream& operator<< <>(ostream& os, ourclass<T> & ocs);` That is enough to get rid of the dreaded "Undefined symbols" error, but another follows, this time quite comprehensible, and, since we reread everything about templates and friends, it's quite easy to find that with templates for our friend declaration to work we need to define our `operator<<` before the class definition. Since `operator<<` gets  ourclass as a function argument, we have to pre-declare ourclass before that, so our final version of ourclass would be:

```c++
#include <iostream>
#include "test_templates.hpp"

using namespace std;

template<typename T> class ourclass; //pre-declaration for the friend operator<<

//the friend of template must be defined before the template itself
template<typename T>
ostream& operator<<(ostream& os, ourclass<T>  & ocs)
{
   os<<'{';
   for (auto &s : ocs.a)
   {
       os << s << " ";
   }
   os<<'}';
   return os;
}

template<typename T>
class ourclass
{
public:
    ourclass(const std::initializer_list<T> &init_list)
    {
        cout<<"init from init_list"<<endl;
        if(init_list.size()>3)
            throw std::out_of_range("Initializer list is too big");
        std::copy(init_list.begin(),init_list.end(),a);
    }

    T getbyIndex(int index);
    void swap(ourclass& other) noexcept
    {
        cout<<"ourclass embedded swap"<<endl;
        std::swap(a, other.a);
    }
    //here the additional <> indicate templateness of friend
    friend ostream& operator<< <>(ostream& os, ourclass<T> & ocs);
private:
    T a[3];
};

template<typename T>
T ourclass<T>::getbyIndex(int index)
{return a[index];}

template<typename T>
inline void swap(ourclass<T> &l, ourclass<T> &r)
{
    l.swap(r);
    cout<<"Specialized swap for ourclass was called"<<endl;
}

```
Phew, that was something. Our earlier statement that template programming is easy certainly didn't age well. Indeed, cryptic error messages still can pop up unexpectedly at times. Template specialization is not always as easy as in our first attempt, too, there are many nuances that we've not met yet. So is it easy or hard? Well, it's neither that hard as it seems when you see the standard library's source code for the first time, nor as easy as when you write your first simple template classes and functions. It's doable. If you can understand pointers to pointers — you certainly can understand templates (but some obscure aspects of them will probably keep their mystery. It's OK, very few C++ professionals know it completely).
