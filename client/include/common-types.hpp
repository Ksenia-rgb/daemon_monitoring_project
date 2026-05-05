#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <chrono>
#include <map>
#include <string>
#include <variant>
#include <vector>

using metric_value = std::variant< int, float, bool, std::string >;
inline std::ostream & operator<<(std::ostream & os, const metric_value & val)
{
  std::visit([& os](const auto & v){ os << v; }, val);
  return os;
}

struct Threshold
{
  std::string type;
  metric_value value;
  int success_compare;
};
struct ServerInfo
{
  std::string name;
  std::string url;
  std::map< std::string, std::map< std::string, Threshold > > thresholds;
};

struct Metric
{
  std::chrono::system_clock::time_point time;
  //example: data["cpu"]["usage"] == 60
  std::map< std::string, std::map< std::string, metric_value > > data;
};
struct MetricsFile
{
  int version;
  int id;
  std::string name;
  size_t interval_value;
  std::string interval_units;
  std::vector< Metric > metrics;
};

#endif
