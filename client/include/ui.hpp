#ifndef UI_HPP
#define UI_HPP

#include <vector>
#include <chrono>
#include <string>

#include "common-types.hpp"

class UI
{
public:
  using command_handler = std::function< void(const std::vector< std::string > &) >;
  virtual ~UI() = default;

  virtual void registerCommand(const std::string & name, command_handler handler) = 0;
  virtual void run() = 0;

  virtual void updateServers(std::map< std::string, ServerInfo > servers) = 0;

  virtual void updateMetricGraph
  (
    const std::string & name,
    const std::string & pc_part,
    std::vector< std::pair< std::chrono::system_clock::time_point, metric_value > > values
  ) = 0;
};

#endif
