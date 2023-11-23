# Notes on the initialization of containers and strings from other containers and strings. 

It was mentioned that there is not direct initialization/conversion from a `vector<char>` to a `string`.
While such conversion would be indeed rarely needed (what is a string if not just an improved vector<char>, who and why would first manipulate a vector only to convert it later, if string can do everything vector can and more?), it's hard to foresee everything.

And the rarer the usage, the easier to make too many wrong mistakes (C) Yoggi Berra. Such as, seeing that an array has a data() method, attempt to use it this way:

```c++
std::vector<char> vc {'W','e','l','l'};
std::string sVeryBad(vc.data());
std::cout<<"A very bad string: "<<sVeryBad<<std::endl;
```

As you can see, not only everything compiles just fine, it probably runs just fine, doing what it supposed to. And that's the worst: since the vector's memory management zeroed the excessive memory, it worked for me every time, although the bug is there, and it is a bug of UB type. UB is called “Undefined Behavior" for reason. It’s not just undefined, it’s completely unpredictable. It could stop to work any time, just as some completely unrelated code was entered somewhere else. Or something. Bet it would stop to work in the worst possible moment, like on the presentation of a shiny new release.

Fortunately, such a piece of code as above is pretty contrived. Probably, if we want to convert a vector to a string, we had  manipulated it as a vector heavily somewhere previously. In this case the bug would be more obvious. Let’s imitate some vector manipulation:

```c++
vc.push_back('n'); vc.push_back('e'); vc.push_back('s'); vc.push_back('s');
vc.pop_back(); vc.pop_back(); vc.pop_back(); vc.pop_back();

std::string sBad(vc.data());
std::cout<<"An obviously bad string: "<<sBad<<std::endl;
```

My last result was 
> Wellness\254j

, but sometimes there were some more fanciful and even more obviously corrupt strings. Not always, sometimes the result was looking as innocent as in our first version, but at least we get the obvious corruption reasonably frequently, so we can find and fix it.

But what exactly is this bug? If you haven't already caught it -- `vc.data()` returns `char *`, which is indistinguishable from a C-style string or a string literal. `std::string` has a constructor for literals and C strings, and naturally it copies to our new string everything from the start of our vector's underlying data to the first zero it finds in the memory. That's why it is so unpredictable: we can't know where that first zero would be. Maybe it would just after that buffer for the unencrypted bank password, and you will duly copy it into the string that is intended to be posted on your homepage. 
     
How we fix it? First, we can simply add a zero as the last element of our vector. Plain `vc.push_back(0);` will do the job. But that interferes with our vector, and whatever reasons we had to use it in the first place may prevent us from modifying it there. 

More appropriate solution is to use other constructors. The easiest (and fastest) way would be simply to add `size()` as the second parameter

```c++
std::string sWell(vc.data(), vc.size());
std::cout<<"A data/size well-formed string: "<<sWell<<std::endl;
```

And it works perfect in our case. It allocates `size()` or more bytes to the internal buffer of the string and then calls `memcpy` (specified as the fastest library routing for memory-to-memory copy). That's nice, so why do other methods exist? Such as

```c++
std::string sIterWell(vc.begin(),vc.end());
std::cout<<"An iterator well-formed string: "<<sIterWell<<std::endl;

```

Because while `(data, size)` type of constructor works with strings and vectors, and with some modifications with C arrays as well as C++ `std::array`, it simply cannot work with other containers, in which the elements are not stored in the contiguous memory suitable for `memcpy` (and therefore have no `data()` method). That's where the universal iterator interface shines. It works with all containers as well as with some other similar entities, such as arrays. Of course, C-style arrays cannot have member functions, but there are free-standing `begin()` and `end()` functions that can work with arrays as well as with any containers:

```c++
char arr[] {'A','r','r','a','y'};
std::vector<char> vec {'V','e','c','t','o','r'};
std::list<char> lst {'L','i','s','t'};
    
std::string sFromArr(std::begin(arr),std::end(arr));
std::string sFromVec(std::begin(vec),std::end(vec));
std::string sFromList(std::begin(lst),std::end(lst));
    
std::cout<<"\nInitialized via generic std::begin() and std::end() containers:"<<std::endl;
std::cout<<sFromArr<<" "<<sFromVec<<" "<<sFromList<<std::endl;


```

This universal interface is especially useful when we write templates. The `cont_2_string` template function defined below is trivial and pretty useless, but we can use the same approach in really sophisticated cases as well.

```c++
template <typename T> std::string cont_2_string(const T& cont)
{
    return std::string(std::begin(cont), std::end(cont));
}

```
Now let's see how it works in our trivial case:

```c++
std::deque<char> deq {'D','e','q','u','e'};
std::forward_list<char> flst {'F','l','i','s','t'};
auto initlist = {'I','n','i','t','l','i','s','t'};
    
std::cout<<"\nInitialized thru generic function:"<<std::endl;
std::cout<<cont_2_string(deq)<<" "<<cont_2_string(arr)<<" "
        <<cont_2_string(flst)<<" "<<cont_2_string(initlist)<<std::endl;
    
```

So, if the iterator-based approach is so fine, why does `data()/size()` even exist? Why are there multiple ways to do the same thing?
     
Each way has its strengths. Iterators and `begin()/end()` functions give conformity, but `data()/size()` give speed, especially when we work with plain data such as char, int , etc.
     
By default the iterators will copy the data byte by byte, while with `data()/size()` the constructor calls memcpy().

Initializations from zero-ended C-strings and literals, btw, are slower too, because they need to find the length of the string by `strlen()`, and only then they can call memcpy. 

We can use similar initialization with a C-style array:

```c++
std::cout<<"From array via sizeof:\n"<<std::string(arr, sizeof(arr))<<std::endl;
```

But, as you can see, even here we have a little bit different interface, using `arr`, `sizeof(arr)` instead of `vec.data()`, `vec.size()`. It is harder to template. Since C++17 we can use more universal free-standing data() and size() functions, that at least allows us the unification between arrays, containers that have `data()` member functions and initializer lists. 

```c++
std::cout<<"Via std::data() and std::size() :"
    <<"\nArray: \t" <<std::string(std::data(arr), std::size(arr))
    <<"\nInitlist: \t" <<std::string(std::data(initlist), std::size(initlist))
    <<"\nVector: \t" <<std::string(std::data(vc), std::size(vc))
    <<std::endl;

```

Still, we cannot use them with `data()`-less containers. So our old speed vs convenience choice holds. Note that we can only be sure that we will really gain performance with the plain data. Suppose we initialize a vector with an array of class types, in this case we need call constructors of each object anyway, no simple memcpy. So there would be no real difference between two approaches unless library writers don't provide some tricky optimizations (but who said they couldn't do tricks with iterators?). 

