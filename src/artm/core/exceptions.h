// Copyright 2014, Additive Regularization of Topic Models.

/*****************************************************************
// All exceptions in artm::core should be inherited from std::runtime_error,
// using DEFINE_EXCEPTION_TYPE (see below). Example of how to throw and handle an exception:
try {
  BOOST_THROW_EXCEPTION(NotImplementedException("This operation is not implemented"));
} catch (const std::runtime_error& e) {
  std::cerr << e.what() << std::endl;
  std::cerr << *boost::get_error_info<boost::throw_file>(e) << std::endl; 
  std::cerr << *boost::get_error_info<boost::throw_line>(e) << std::endl; 
  std::cerr << *boost::get_error_info<boost::throw_function>(e) << std::endl; 
}
*****************************************************************/

#ifndef SRC_ARTM_CORE_EXCEPTIONS_H_
#define SRC_ARTM_CORE_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

#include "boost/exception/get_error_info.hpp"
#include "boost/throw_exception.hpp"

namespace artm {
namespace core {

#define DEFINE_EXCEPTION_TYPE(Type, BaseType)          \
class Type : public BaseType { public:  /*NOLINT*/     \
  explicit Type(std::string what) : BaseType(what) {}  \
  explicit Type(const char* what) : BaseType(what) {}  \
};

DEFINE_EXCEPTION_TYPE(ArgumentOutOfRangeException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(UnsupportedReconfiguration, std::runtime_error);
DEFINE_EXCEPTION_TYPE(NotImplementedException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(NetworkException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(SerializationException, std::runtime_error);

#undef DEFINE_EXCEPTION_TYPE

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_EXCEPTIONS_H_
