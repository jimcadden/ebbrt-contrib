//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Poly.h"

void test2(ebbrt::EbbRef<ebbrt::PolyFoo> foopoly) {
  foopoly->Foobar();
}

class Bar;
void test(ebbrt::EbbRef<ebbrt::Poly> foo) {
  foo->Foo();
  foo->Bar();
  //test2(foo);
  //test2(static_cast<ebbrt::EbbRef<ebbrt::PolyFoo>>(ebbrt::EbbRef<Bar>()));
  test2(static_cast<ebbrt::EbbRef<ebbrt::PolyFoo>>(foo));
}

void AppMain() { 

  auto fooby = ebbrt::PolyFoo::Create();
  fooby->Foo();
  fooby->Bar();
  fooby->Foobar();
  test(fooby);
}

