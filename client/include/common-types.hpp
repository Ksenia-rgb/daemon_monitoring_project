#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <string>

struct ServerThresholds
{
  double cpu_usage;
  double disk_usage;
  double memory_usage;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerThresholds, cpu_usage, disk_usage, memory_usage)

struct GetStrategy
{
  std::string scheme;
  std::string host;
  std::string port;
  std::string endpoint;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GetStrategy, scheme, host, port, endpoint)

#endif
