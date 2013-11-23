#include "foo.h"

#include <iostream>
using namespace std;

void Foo::foo(X x) {
  cout << "foo\n";
  x.x();
}
