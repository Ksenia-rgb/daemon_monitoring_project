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
}

void CMDUI::updateServers(std::map< std::string, ServerInfo > servers)
{
  for (auto it = servers.begin(); it != servers.end(); ++it)
  {
    std::cout << "server: " << it->first << " with " << it->second.url << "\n";
  }
}