// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/memcached_service_impl.h"

namespace artm {
namespace memcached {

void MemcachedServiceImpl::UpdateKey(const UpdateKeyArgs& request,
                                     ::rpcz::reply<UpdateKeyResult> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  UpdateKeyResult result;

  KeyStore& key_store = data_[request.key_group()];
  ValueStore& value_store = key_store[request.key()];
  if (value_store.size() == 0) {
    value_store.reserve(request.value_size());
    for (int i = 0; i < request.value_size(); ++i) {
      value_store.push_back(request.value(i));
      result.add_value(value_store[i]);
    }

    result.set_error_code(kSuccess);
    response.send(result);
    return;
  }

  if (static_cast<int>(value_store.size()) == request.value_size()) {
    for (int i = 0; i < request.value_size(); ++i) {
      value_store[i] += request.value(i);
      result.add_value(value_store[i]);
    }

    result.set_error_code(kSuccess);
    response.send(result);
    return;
  }

  result.set_error_code(kInvalidArgument);
  response.send(result);
  return;
}

void MemcachedServiceImpl::RetrieveKey(const RetrieveKeyArgs& request,
                                       ::rpcz::reply<RetrieveKeyResult> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  RetrieveKeyResult result;

  GroupStore::iterator group_iter = data_.find(request.key_group());
  if (group_iter == data_.end()) {
    result.set_error_code(kKeyGroupNotFound);
    response.send(result);
    return;
  }

  KeyStore& key_store = group_iter->second;
  KeyStore::iterator key_iter = key_store.find(request.key());
  if (key_iter == key_store.end()) {
    result.set_error_code(kKeyNotFound);
    response.send(result);
    return;
  }

  ValueStore& value_store = key_iter->second;
  for (size_t i = 0; i < value_store.size(); ++i) {
    result.add_value(value_store[i]);
  }

  result.set_error_code(kSuccess);
  response.send(result);
  return;
}

void MemcachedServiceImpl::RetrieveGroup(const RetrieveGroupArgs& request,
                                         ::rpcz::reply<RetrieveGroupResult> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  RetrieveGroupResult result;

  GroupStore::iterator group_iter = data_.find(request.key_group());
  if (group_iter == data_.end()) {
    result.set_error_code(kKeyGroupNotFound);
    response.send(result);
    return;
  }

  KeyStore& key_store = group_iter->second;
  for (auto iter = key_store.begin(); iter != key_store.end(); ++iter) {
    result.add_key(iter->first);
  }

  result.set_error_code(kSuccess);
  response.send(result);
  return;
}

}  // namespace memcached
}  // namespace artm

