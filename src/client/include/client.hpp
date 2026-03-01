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
    MenuItems items = {
      {"Get monitoring data", [this]() {getMonitoringData(); return false;}},
      {"Exit Application", [this]() {return true;}}
    };
    ui_->run(items);
  }
  void getMonitoringData() const
  {
    std::cout << "Monitoring!\n";
  };
protected:
  std::unique_ptr< UI > ui_;
};

#endif
