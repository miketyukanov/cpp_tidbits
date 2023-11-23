//
//  test_templates.cpp
//  cont_string
//
//  Created by Mike Tyukanov on 05.02.2023.
//
#include <iostream>

#include "templates.hpp"

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
    //here the additional <> indicate
    friend ostream& operator<< <>(ostream& os, ourclass<T> & ocs);
private:
    T a[3];

};


/*
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
    
 //   friend ostream& operator<<(ostream& os, ourclass<T> & ocs);
private:
    T a[3];

};

*/
/*
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
private:
    T a[3];
};

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
*/


template<typename T>
T ourclass<T>::getbyIndex(int index)
{return a[index];}


template<typename T>
inline void swap(ourclass<T> &l, ourclass<T> &r)
{
    l.swap(r);
    cout<<"Specialized swap for ourclass was called"<<endl;
}

template<typename T>
void swap(T& l, T&r)
{
    T tmp=l;
    l=r;
    r=tmp;
    cout<<"Local swap was called"<<endl;
}

void swap(string& lhs, string& rhs)
{
    lhs.swap(rhs);
    cout<<"Local no-template string-specialized swap was called"<<endl;
}

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


int test_swaps(void)
{
    try
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
      
        NoTemplate ntl1{"One", "two", "three"}, ntl2{"Eins", "zwei", "drei"};
        cout<<"Notemplate before swap:\n\t"<<ntl1<<ntl2<<endl;
        ::swap(ntl1,ntl2);
        cout<<"Notemplate after swap:\n\t"<<ntl1<<ntl2<<endl<<endl;
    }
    catch(std::exception &ex)
    {
        cerr<<ex.what()<<endl;
        return 1;
    }
    catch(...)
    {
        cerr<<"Unknown exception"<<endl;
    }
    
    return 0;
}

