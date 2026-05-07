#ifndef DAEMON_COLLECTOR_HPP
#define DAEMON_COLLECTOR_HPP

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) \
    || defined(__WINDOWS__)
  #define IS_WINDOWS 1
  #define IS_LINUX 0
#endif
#if defined(__linux__) || defined(linux) || defined(__linux) || defined(__unix__) || defined(__unix)
  #define IS_WINDOWS 0
  #define IS_LINUX 1
#endif

#include <map>
#include <string>

class Collector
{
public:
  using MetricMap = std::map< std::string, float >;

  virtual ~Collector() = default;
  MetricMap getMetrics() const;
protected:
  virtual float getCpuUsage() const = 0;
  virtual float getMemoryUsage() const = 0;
  virtual float getDiskUsage() const = 0;
  virtual float getTemperature() const = 0;
};

#endif
