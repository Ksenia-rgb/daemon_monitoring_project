#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <functional>
#include <fstream>
#include <memory>

#include "managers.hpp"
#include "ui.hpp"

class Client
{
public:
  explicit Client(std::unique_ptr< UI > ui, std::string cfg_filepath):
    ui_(std::move(ui)),
    si_(cfg_filepath)
  {}

  void run();

  //void fetchMonitorDataFromURL(char * url)
  //void getMonitorData(char * server);

  void showServerInfoList();
  void refreshServerInfoList();

  void showServerMetrics(const std::string & server_name);
  void showServerMetricsList();

  void refreshServerMetrics(const std::string & server_name);
  void refreshServerMetricsList();

protected:
  std::unique_ptr< UI > ui_;
  ConfigManager si_;
};

#endif
