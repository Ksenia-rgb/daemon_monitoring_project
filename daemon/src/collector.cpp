#include "collector.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

Collector::MetricMap Collector::getMetrics() const
{
  MetricMap metrics;
  metrics.emplace("cpu_usage", getCpuUsage());
  metrics.emplace("memory_usage", getMemoryUsage());
  metrics.emplace("disk_usage", getDiskUsage());
  metrics.emplace("temperature", getTemperature());
  return metrics;
}
