#include "cmd-ui.hpp"

#include <iostream>

void CMDUI::registerCommand(const std::string & name, command_handler handler)
{
  commands_[name] = handler;
}

void CMDUI::run()
{
  std::cout << "I'm Alive...\n";
  commands_.at("update_servers")({});
  commands_.at("refresh_metric_for")({"server-a"});
}

void CMDUI::updateServers(std::map< std::string, ServerInfo > servers)
{
  for (auto it = servers.begin(); it != servers.end(); ++it)
  {
    std::cout << "server: " << it->first << " with " << it->second.url << "\n";
  }
}

void CMDUI::updateMetricGraph
(
  const std::string & name,
  const std::string & pc_part,
  std::vector< std::pair< std::chrono::system_clock::time_point, metric_value > > values
)
{
  std::cout << "metrics from " << name << ":\n";
  std::cout << "== " << pc_part << " ==\n";
  for (const auto & metric: values)
  {
    auto time_t = std::chrono::system_clock::to_time_t(metric.first);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    //LMAO C++ FUCKIN SUCKS
    std::cout << ss.str() << " | " << metric.second << '\n';
  }
}
