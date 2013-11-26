#include <iostream>
using namespace std;

class Foo {
public:
  Foo() {
    cout << "Constructor\n";
  }

  Foo(const Foo& other) {
    cout << "Copy constructor\n";
  }

  Foo(const Foo&& other) {
    cout << "Move constructor\n";
  }

  Foo& operator=(const Foo& other) {
    cout << "Operator=\n";
    return *this;
  }

  Foo& operator=(const Foo&& other) {
    cout << "Move operator=\n";
    return *this;
  }
};

Foo func() {
  cout << "";
  return Foo();
}

Foo func_move() {
  Foo foo;
  return std::move(foo);
}


class Bar {
public:
  typedef int BarInt;
};

void split() {
  static int i = 1;
  cout << "==== Example " << i++ << " ====\n";
}

int main() {
  //  Bar::BarInt 
  BarInt val2;
  split(); // 1
  Foo foo;

  split(); // 2
  Foo foo2(foo);

  split(); // 3
  foo2 = foo;

  split(); // 4
  foo2 = func();

  split(); // 5
  Foo foo3(func());

  split(); // 6
  Foo foo4(func_move());

  return 0;
}
