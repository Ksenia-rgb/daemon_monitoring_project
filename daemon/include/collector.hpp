#ifndef DAEMON_COLLECTOR_HPP
#define DAEMON_COLLECTOR_HPP
#include <map>
#include <string>

class Collector
{
public:
  using MetricMap = std::map< std::string, double >;

  virtual ~Collector() = default;
  MetricMap getMetrics() const;
protected:
  virtual double getCpuUsage() const = 0;
  virtual double getMemoryUsage() const = 0;
  virtual double getDiskUsage() const = 0;
  virtual double getTemperature() const = 0;
};

#endif
