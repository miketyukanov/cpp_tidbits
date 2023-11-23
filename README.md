# C++ tidbits

## What is this
The C++ files in this collection were initially written to illustrate some questions that came within live Zoom sessions of C++ lessons and couldn't be properly explained in-session. As such they were useful only to the students who attended those sessions, and I decided to add corresponding texts so that any student of C++ could find something useful when stuck on that particular topic.

Note that it is not a C++ course, and never will be. It is "what's usually skipped for brevity". 

The code often contains examples of bugs, sometimes obvious, sometimes just a hidden UB, sometimes the code crashes, so never use a snippet from this collection without reading corresponding `.md` first — it could be an example of "what not to do". Even bug-free snippets are usually too contrived to be really useful. 

The topics are beginner-to-intermediate level, when we dive a little bit deeper, it should remain accessible. Programmers experienced in other languages would probably find the beginners' stuff obvious, but, since some details of C++ are quite different even from such similar languages as Java or C#, not always.

## How to build 
I provide both Makefile for gcc/clang and CMakeLists.txt for VSCode and other IDEs. Both are very minimalistic, since I couldn't test them thoroughly on all the possible configurations, and in my view, the less is written, the easier it is to customize. So feel free to add compiler options, directory structures etc., whatever works  better for you in your particular environment. 

For building with CLI gcc or clang, just go in the source directory and run `make`. Object files as well as the executable will appear right in the source directory, to delete them run `make clean`. That's all. 

With CMake it all depends on the toolchain that you use, but if your IDE works with CMake at all the default settings should work and you can build it in the IDE of your choice. It was tested mostly with Visual Studio Code, which is the most common editor/IDE for the students. If you prefer vim or other pure editor and g++/gdb, I'd recommend simply using `make` to complete the old-schoolness of it all.  

The best way to use this code, however, is not to download the full repo, but just the one particular topic you are interested to — that's why I've decided against separate directories for `.md`, `.cpp` and `hpp` files. Read an `.md` file first, and if you feel like playing with the corresponding code, download the .cpp file, go to the last function in the source (it is named like `int test_something(void)`), and rename it to `int main(void)`. You can either download the corresponding .hpp file or simply comment it out of your .cpp. Then compile it with your favorite compiler, debug with your favorite debugger, edit with your favorite editor or IDE, copy-paste it on godbolt — with a single-file .cpp it is usually much easier. But don't forget set the C++ standard to C++17 (`-std=c++17`) -- some snippets may compile with C++11 or even earlier versions, but they all are tested against this standard. Alternatively, you can play with C++ dialects taking this as an exercise. 

##Contents

`cont_string`   
For the very beginners. Strings and containers inititialization from other containers and container-like entities such as initialization lists. 
`member_calls_ctor`    
C++ rules for member function calls from constructors. For beginners and Java programmers.
`templates`   
Starts with the explanation of why `using namespace std;` is usually not recommended even on file level, continues with using the multiple `swap` functions to get a sense of lookup rules, touches init-lists, and ends with class template troubleshooting — for intermediate-level learners, who make their first steps into  templating. 
`lambdas`   
Callables in general, but mostly lambdas: easy ways to store them in a container and return them, the danger of capturing by reference with storing and returning, two ways to make recursive lambdas, and why can't we use default arguments when we call functions via  function pointer. 
