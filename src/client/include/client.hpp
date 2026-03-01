#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <functional>
#include <iostream>
#include <memory>

#include "ui.hpp"

class Client
{
public:
  explicit Client(std::unique_ptr< UI > ui_to_use):
    ui_(std::move(ui_to_use))
  {}
  void run() const
  {
    MenuItems items;
    items["get_monitoring_data"] = [this](void * url) {getMonitoringData(reinterpret_cast<char *>(url)); return false;};
    items["quit"] = [this](void *) {return true;};
    ui_->run(items);
  }
  void getMonitoringData(char * url) const
  {
    std::cout << "Monitoring!\n";
  };
protected:
  std::unique_ptr< UI > ui_;
};

#endif
