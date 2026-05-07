#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <json.hpp>

namespace models
{
  struct SharedStorageConfig
  {
    std::string storage_path;
    std::string client_endpoint;
    std::string daemon_endpoint;
    int port;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SharedStorageConfig, storage_path, client_endpoint, daemon_endpoint, port)
}
#endif
