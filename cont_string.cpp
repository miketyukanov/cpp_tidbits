//
//  cont_string.cpp
//  cont_string
//
//  Created by Mike Tyukanov on 29.01.2023.
//

#include "cont_string.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <deque>
#include <forward_list>


//normally templates are defined in headers, but as it's pure demo, let's
//keep header free

//since C++20 you can use concepts to reasonably constrain T
//to container types, resulting in much better diagnostics. Since we
//work in C++17 and less, we don't use it yet. Traits also can
//be used here, but let's keep it simple
template <typename T> std::string cont_2_string(const T& cont)
{
    return std::string(std::begin(cont), std::end(cont));
}


int test_cont_string(void)
{
/*
 It was mentioned that there is not direct initialization/conversion from a vector<char> to a string.
 While such conversion would be indeed rarely needed (what is a string if not an improved vector<char>, who and why would first manipulate a vector only to convert it later, if we can do everything vector can directly in the string?), it's hard to foresee everything. And the rarer the usage, the easier to make too many wrong mistakes (C) Yoggi Berra. Such as, seeing that an array has a data() method, attempt to use it this way:
*/
    std::vector<char> vc {'W','e','l','l'};
    std::string sVeryBad(vc.data());
    std::cout<<"A very bad string: "<<sVeryBad<<std::endl;
/*
 As you can see, not only everything compiles just fine, it probably runs just fine, doing what it supposed to. And that's the worst: since the vector's memory management zeroed the excessive memory, it worked for me every time. But it’s called “Undefined Behavior" for reason. It’s not just undefined, it’s completely unpredictable. It could stop to work any time, just as some completely unrelated code was entered somewhere else. Or something. Bet it would stop to work in the worst possible moment, like on the presentation of a shiny new release.
*/
    
    
    
/*
 Fortunately, such a piece of code as above is pretty contrived. Probably, if we want to convert a vector to a string, we had  manipulated it as a vector heavily somewhere previously. In this case the bug would be more obvious. Let’s imitate some vector manipulation:
*/
    vc.push_back('n'); vc.push_back('e'); vc.push_back('s'); vc.push_back('s');
    vc.pop_back(); vc.pop_back(); vc.pop_back(); vc.pop_back();

    std::string sBad(vc.data());
    std::cout<<"An obviously bad string: "<<sBad<<std::endl;

    /*
     An obviously bad string: Wellness\254j
     My last result was "An obviously bad string: Wellness\254j", but sometimes there were some more fanciful and even more obviously corrupt strings. Not always, sometimes the result was looking as innocent as in our first version, but at least we get the obvious corruption reasonably frequently, so we can find and fix it.
          
     How? Two versions in brief:
   */
    
    //If we'd like to use data(), we use it with size():
    std::string sWell(vc.data(), vc.size());
    std::cout<<"A data/size well-formed string: "<<sWell<<std::endl;
    
    
    //Alternatively, we can use iterators:
    std::string sIterWell(vc.begin(),vc.end());
    std::cout<<"An iterator well-formed string: "<<sIterWell<<std::endl;
    
    /*
     Why iterators? It is much more universal. Yes, a vector has data() and size()
     members, but not every container does. On the contrary, every container (and some
     more container-like entities, like, well, strings) has iterators and begin() and
     end() members. What about good ole C-style arrays? They do not have member
     functions, but library has just, well, functions begin() and end() for them --
     and, of course, for containers and other begin() and end() member-possessing
     classes. With free-standing begin() and end() we can achieve universal approach:
     */
    
    char arr[] {'A','r','r','a','y'};
    std::vector<char> vec {'V','e','c','t','o','r'};
    std::list<char> lst {'L','i','s','t'};
    
    std::string sFromArr(std::begin(arr),std::end(arr));
    std::string sFromVec(std::begin(vec),std::end(vec));
    std::string sFromList(std::begin(lst),std::end(lst));
    
    std::cout<<"\nInitialized via generic std::begin() and std::end() containers:"<<std::endl;
    std::cout<<sFromArr<<" "<<sFromVec<<" "<<sFromList<<std::endl;

    /*
     This universal interface is especially useful when we write templates.
     The cont_2_string template function defined above is trivial and pretty useless, but we can use the same approach in really sophisticated cases as well.
     Now let's see how it works in our trivial one:
     */
    
    std::deque<char> deq {'D','e','q','u','e'};
    std::forward_list<char> flst {'F','l','i','s','t'};
    auto initlist = {'I','n','i','t','l','i','s','t'};
    
    std::cout<<"\nInitialized thru generic function:"<<std::endl;
    std::cout<<cont_2_string(deq)<<" "<<cont_2_string(arr)<<" "
        <<cont_2_string(flst)<<" "<<cont_2_string(initlist)<<std::endl;
    
    /*
     So, if the iterator-based approach is so fine, why does data()/size() even exist?
     Why are there multiple ways to do the same thing?
     
     Each way has its strengths. Iterators and begin()/end() functions give conformity, but data()/size() give speed.
     
     By default the iterators will copy the data byte by byte,
     while with data()/size() the constructor calls memcpy() --
     "std::memcpy is meant to be the fastest library routine for memory-to-memory copy"
     as it said in its description and we can imagine what precise processor-dependent
     optimization it can do in addition to the brutality of copying "machine word-by machine word" instead of "byte-by-byte".
     Initializations from zero-ended C-strings, btw, are slower too, because they need
     to find the length of the string by strlen(), and only then they can call memcpy.
     We can use similar initialization with a C-style array, btw, and that array doesn't
     need to be zero-ended:
     */
    
    std::cout<<"From array via sizeof:\n"<<std::string(arr, sizeof(arr))<<std::endl;
    
    /*
     But, as you can see, even here we have a little bit different interface, using arr, sizeof(arr) instead of vec.data(), vec.size(). It is harder to template. Since C++17 we can use more universal free-standing data() and size() functions, that at least allows us the unification between arrays, containers that have data() member functions and initializer lists. Still, we cannot use them with data()-less containers. So it's still speed vs convenience.
    */
    
    std::cout<<"Via std::data() and std::size() :"
        <<"\nArray: \t" <<std::string(std::data(arr), std::size(arr))
        <<"\nInitlist: \t" <<std::string(std::data(initlist), std::size(initlist))
        <<"\nVector: \t" <<std::string(std::data(vc), std::size(vc))
        <<std::endl;

    return 0;
 }
