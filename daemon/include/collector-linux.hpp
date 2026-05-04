#ifndef COLLCETOR_LINUX_HPP
#define COLLCETOR_LINUX_HPP
#include "collector.hpp"

class CollectorLinux final: public Collector
{
  virtual double getCpuUsage() const override;
  virtual double getMemoryUsage() const override;
  virtual double getDiskUsage() const override;
  virtual double getTemperature() const override;
};

#endif
