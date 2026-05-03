#ifndef UI_HPP
#define UI_HPP

#include <vector>
#include <string>

using MenuItems = std::unordered_map< std::string, std::function< bool(void *) > >;

class UI
{
public:
  virtual ~UI() = default;
  virtual void run(const MenuItems & items) = 0;

  virtual void updateServerList(std::map< std::string, ServerInfo > servers) = 0;

 // virtual void updateServerMetrics(MetricsPackage) = 0;
};

#endif
