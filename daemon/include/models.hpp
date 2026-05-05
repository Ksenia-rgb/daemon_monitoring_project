#ifndef MODELS_HPP
#define MODELS_HPP
#include <string>
#include <nlohmann.hpp>

namespace models
{
  struct SendStrategy
  {
    std::string url;
    std::string endpoint;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SendStrategy, url, endpoint)

  struct MetricRecord
  {
    std::string server_name;
    std::string time;
    std::string name_server;
    double cpu_usage;
    double memory_usage;
    double disk_usage;
    double temperature;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetricRecord, server_name, time, name_server,
      cpu_usage, memory_usage, disk_usage, temperature)

  struct DaemonConfig
  {
    std::string server_name;
    unsigned monitor_interval;
    unsigned repeat_interval;
    unsigned send_interval;
    unsigned min_send_interval;
    SendStrategy send_strategy;
    size_t max_batch_size;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DaemonConfig, server_name, monitor_interval, repeat_interval,
      send_interval, min_send_interval, send_strategy, max_batch_size)
}

#endif
