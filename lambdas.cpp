//
//  test_lambdas.cpp
//  snippets
//
//  Created by Mike Tyukanov on 20.03.2023.
//
#include <iostream>
#include <list>
#include <functional>
#include "lambdas.hpp"

using std::cout, std::cin, std::endl;

/*
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
*/
//NOTE: This is a buggy version, the correct version is the commented-out one above
//The following is buggy, which is more interesting
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

class Adder
{
public:
    Adder(const int &x_, const int& y_): x(x_), y(y_){}
    void operator()(){cout<<x<<"+"<<y<<"="<<x+y<<endl;}
    
private:
    const int &x;
    const int &y;
};

int addints(int lhs, int rhs=1)
{
    return lhs+rhs;
}

typedef int (*p_int_2int)(int, int);
//alternatively you can use "using", but in the function pointer
//case it's no better than typedef:
//using p_int_2int = int (*)(int, int);

void pointer_call(p_int_2int func)
{
    cout<<"\npointer_call (4,4): "<<func(4,4)<<endl;
}
//this doesn't compile
/*
void pointer_call_with_def_arg(p_int_2int func)
{
    cout<<"\ncalling func with just one arg: "<<func(5)<<endl;
}
*/

typedef int (*p_int_1int)(int);
void one_arg_pointer_call(p_int_1int func)
{
    cout<<"\none argument pointer call (5): "<<func(5)<<endl;
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
    //recursive lambdas section
    //this attempt to make a recursive lambda won't compile:
    /*
    auto fctor = [&fctor](int n) {
            return  n * fctor(n - 1);
        };
    */
//C++11 std::function solution:
    std::function<int64_t(int64_t)> fcl = [&fcl](int64_t n)->int64_t {
            return n>1?n*fcl(n-1):1;
        };
    for(int i=1;i<21;++i)
    {
        cout<<fcl(i)<<endl;
    }

    auto fctrl=[](int64_t n, auto& fctrl_ref)->int64_t {
        return n>1?n*fctrl_ref(n-1, fctrl_ref):1;
    };

    for(int i=1;i<21;++i)
    {
        cout<<fctrl(i, fctrl)<<endl;
    }

    cout<<"testing addints:\n"<<"Normal call (5): "<<addints(5);
    pointer_call(addints);
    //this doesn't compile either
    //one_arg_pointer_call(addints);
    return 0;
}
