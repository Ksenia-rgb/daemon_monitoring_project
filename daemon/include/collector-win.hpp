#ifndef COLLCETOR_WIN_HPP
#define COLLCETOR_WIN_HPP
#include "collector.hpp"

class CollectorWin final: public Collector
{
  virtual double getCpuUsage() const override;
  virtual double getMemoryUsage() const override;
  virtual double getDiskUsage() const override;
  virtual double getTemperature() const override;
};

#endif
