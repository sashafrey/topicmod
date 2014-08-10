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

enum ArtmErrorCodes {
    ARTM_SUCCESS = 0,
    ARTM_GENERAL_ERROR = -1,
    ARTM_OBJECT_NOT_FOUND = -2,
    ARTM_INVALID_MESSAGE = -3,
    ARTM_INVALID_OPERATION = -4,
    ARTM_NETWORK_ERROR = -5,
    ARTM_STILL_WORKING = -6
};

namespace artm {
namespace core {

#define DEFINE_EXCEPTION_TYPE(Type, BaseType)          \
class Type : public BaseType { public:  /*NOLINT*/     \
  explicit Type(std::string what) : BaseType(what) {}  \
  explicit Type(const char* what) : BaseType(what) {}  \
};

#define CATCH_EXCEPTIONS                                                       \
catch (const rpcz::rpc_error& e) {                                             \
  LOG(ERROR) << "rpc_error: " << e.what();                                     \
  error_message = "Rpc error :  " + std::string(e.what());                     \
  return ARTM_NETWORK_ERROR;                                                   \
} catch (const artm::core::NetworkException& e) {                              \
  LOG(ERROR) << "NetworkException: " << e.what();                              \
  error_message = "Network error :  " + std::string(e.what());                 \
  return ARTM_NETWORK_ERROR;                                                   \
}  catch (const artm::core::InvalidOperation& e) {                             \
  LOG(ERROR) << "InvalidOperation: " << e.what();                              \
  error_message = "Invalid Operation :  " + std::string(e.what());             \
  return ARTM_INVALID_OPERATION;                                               \
} catch (const std::runtime_error& e) {                                        \
  LOG(ERROR) << "runtime_error: " << e.what();                                 \
  error_message = "Runtime error :  " + std::string(e.what());                 \
  return ARTM_GENERAL_ERROR;                                                   \
} catch (...) {                                                                \
  LOG(ERROR) << "unknown error.";                                              \
  error_message = "Unknown error. ";                                           \
  return ARTM_GENERAL_ERROR;                                                   \
}

#define CATCH_EXCEPTIONS_AND_SEND_ERROR                     \
catch (const rpcz::rpc_error& e) {                          \
  LOG(ERROR) << "rpc_error: " << e.what();                  \
  response.Error(ARTM_NETWORK_ERROR);                       \
} catch (const artm::core::NetworkException& e) {           \
  LOG(ERROR) << "NetworkException: " << e.what();           \
  response.Error(ARTM_NETWORK_ERROR);                       \
}  catch (const artm::core::InvalidOperation& e) {          \
  LOG(ERROR) << "InvalidOperation: " << e.what();           \
  response.Error(ARTM_INVALID_OPERATION);                   \
} catch (const std::runtime_error& e) {                     \
  LOG(ERROR) << "runtime_error: " << e.what();              \
  response.Error(ARTM_GENERAL_ERROR);                       \
} catch (...) {                                             \
  LOG(ERROR) << "unknown error.";                           \
  response.Error(ARTM_GENERAL_ERROR);                       \
}

DEFINE_EXCEPTION_TYPE(ArgumentOutOfRangeException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidOperation, std::runtime_error);
DEFINE_EXCEPTION_TYPE(NotImplementedException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(NetworkException, std::runtime_error);
DEFINE_EXCEPTION_TYPE(SerializationException, std::runtime_error);

#undef DEFINE_EXCEPTION_TYPE

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_EXCEPTIONS_H_
