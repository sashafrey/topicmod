#ifndef __CALL_ON_DESTRUCTION_H
#define __CALL_ON_DESTRUCTION_H

#include <functional>

namespace artm {
namespace helpers {

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);             \
    void operator=(const TypeName&)        \

// An object that accepts a lambda expression end executes it in destructor
class call_on_destruction {
private:
    std::function<void()> f_;
    DISALLOW_COPY_AND_ASSIGN(call_on_destruction);
public:
    call_on_destruction(std::function<void()> f) : f_(f) {}
    ~call_on_destruction() { f_(); }
};

}} // namespace artm::helpers

#endif // __CALL_ON_DESTRUCTION_H