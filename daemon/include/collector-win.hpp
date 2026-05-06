#ifndef COLLCETOR_WIN_HPP
#define COLLCETOR_WIN_HPP
#include "collector.hpp"

class CollectorWin final: public Collector
{
  virtual float getCpuUsage() const override;
  virtual float getMemoryUsage() const override;
  virtual float getDiskUsage() const override;
  virtual float getTemperature() const override;
};

#endif
