#include "cmd-ui.hpp"

#include <iostream>

void CMDUI::run(const MenuItems & items)
{
  std::cout << "== commands ==\n";
  for (auto item: items)
  {
    std::cout << item.first << '\n';
  }
  std::cout << "> ";
  for (std::string command; std::cin >> command;)
  {
    try
    {
      bool exit = items.at(command)(nullptr);
      if (exit)
      {
        std::cout << "exiting..\n";
        return;
      }
    }
    catch(const std::out_of_range &)
    {
      std::cout << "There is no such command\n";
    }
    std::cout << "> ";
  }
}

void CMDUI::updateServerList(std::map< std::string, ServerInfo > servers)
{
  std::cout << "== servers info ==\n";
  size_t i = 1;
  for (auto it = servers.begin(); it != servers.end(); ++it)
  {
    std::cout << i++ << ". " << it->first << " with url: " << it->second.url << '\n';
  }
}
/*
void CMDUI::updateServerMetrics(MetricsPackage metrics)
{
  std::cout << "== metrics info ==\n";
  size_t i = 1;
  for (auto it = metrics.begin(); it != metrics.end(); ++it)
  {
    std::cout << i++ << ". " << it->first << " with url: " << it->second.url << '\n';
  }
}
*/