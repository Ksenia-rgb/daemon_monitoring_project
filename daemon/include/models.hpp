#ifndef MODELS_HPP
#define MODELS_HPP
#include <string>

struct SendStrategy
{
  std::string url;
  std::string endpoint;
};

struct MetricRecord
{
  std::string time;
  std::string name_server;
  double cpu_usage;
  double memory_usage;
  double disk_usage;
  double temperature;
};

#endif
