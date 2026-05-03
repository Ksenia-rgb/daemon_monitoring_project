#ifndef CMD_UI_HPP
#define CMD_UI_HPP

#include <string>

#include "client.hpp"

class CMDUI: public UI
{
public:
  void run(const MenuItems & items) override;

  void updateServerList(std::map< std::string, ServerInfo > servers);

 // void updateServerMetrics(MetricsPackage metrics);
};

#endif
