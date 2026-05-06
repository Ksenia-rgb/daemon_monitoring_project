#ifndef COLLCETOR_LINUX_HPP
#define COLLCETOR_LINUX_HPP
#include "collector.hpp"

class CollectorLinux final: public Collector
{
  virtual float getCpuUsage() const override;
  virtual float getMemoryUsage() const override;
  virtual float getDiskUsage() const override;
  virtual float getTemperature() const override;
};

#endif
