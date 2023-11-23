//
//  main.cpp
//  snippets
//
//  Created by Mike Tyukanov on 09.02.2023.
//
#include <iostream>

#include "member_calls_ctor.hpp"
#include "cont_string.hpp"
#include "templates.hpp"
#include "lambdas.hpp"



int main(int argc, const char * argv[]) 
{
  test_member_calls_ctor();
  test_cont_string();
  test_swaps();
  test_lambdas();
  return 0;
}
