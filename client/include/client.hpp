#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <functional>
#include <fstream>
#include <memory>

#include "file-readers.hpp"
#include "ui.hpp"

class Client
{
public:
  explicit Client(std::unique_ptr< UI > ui):
    ui_(std::move(ui))
  {}

  void run();

  void loadConfig();

  void refreshAllMetrics();
  void refreshMetricsFor(const std::string & server_name);

protected:
  std::unique_ptr< UI > ui_;
  std::unique_ptr< ConfigFile > config_;
  //std::unique_ptr< MetricsPackage > config_;
};

#endif
