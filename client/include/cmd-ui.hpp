#ifndef CMD_UI_HPP
#define CMD_UI_HPP

#include <string>

#include "client.hpp"

class CMDUI: public UI
{
  std::map< std::string, command_handler > commands_;
public:
  void registerCommand(const std::string & name, command_handler handler) override;
  void run() override;

  void updateServers(std::map< std::string, ServerInfo > servers) override;
  void updateMetricGraph
  (
    const std::string & name,
    const std::string & pc_part,
    std::vector< std::pair< std::chrono::system_clock::time_point, metric_value > > values
  ) override;
};

#endif
