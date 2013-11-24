#include "instance_manager.h"

namespace topicmd {
  Instance::Instance(int id, const InstanceConfig& config) : 
    id_(id),
    instance_config_(new InstanceConfig(config))
  {
  }
}
