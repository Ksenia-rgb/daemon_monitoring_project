#ifndef MODELS_HPP
#define MODELS_HPP
#include <json.hpp>
#include <string>

namespace models
{
  struct SendStrategy
  {
    std::string url;
    std::string endpoint;
    std::string port;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SendStrategy, url, endpoint, port)

  struct MetricRecord
  {
    std::string server_name;
    std::string time;
    double cpu_usage;
    double memory_usage;
    double disk_usage;
    double temperature;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MetricRecord, server_name, time,
    cpu_usage, memory_usage, disk_usage, temperature)

  struct DaemonConfig
  {
    std::string name;
    unsigned monitor_interval;
    unsigned repeat_interval;
    unsigned send_interval;
    unsigned min_send_interval;
    size_t max_batch_size;
    SendStrategy send_strategy;
    std::string log_path;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DaemonConfig, name, monitor_interval, repeat_interval, send_interval,
      min_send_interval, send_strategy, max_batch_size, log_path)
}

#endif
