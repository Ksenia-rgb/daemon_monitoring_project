#include "client.hpp"

void Client::run()
{
  MenuItems items;
  items["refresh_servers"] = { [this](void *)
  {
    refreshServerInfoList();
    return false;
  }};
  //items["get_metrics"] = { [this](void * metrics)
  //{
  //  refreshServerMetrics(metrics);
  //  return false;
  //}};
    //{"get_monitoring_data", [this](void * url) {getMonitoringData(reinterpret_cast< char * >(url)); return false;}},
  items["quit"] = { [this](void *)
  {
    return true;
  }};
  ui_->run(items);
}

void Client::showServerInfoList()
{
  ui_->updateServerList(si_.getServerInfos());
}

void Client::refreshServerInfoList()
{
  si_.update();
  ui_->updateServerList(si_.getServerInfos());
}

void Client::showServerMetrics(const std::string & server_name)
{

}
void Client::showServerMetricsList()
{

}

void Client::refreshServerMetrics(const std::string & server_name)
{
  MetricsPackage metrics{server_name, si_.getMetricsFilePath() + server_name};
  //ui_->updateServerMetrics(metrics);
}

void Client::refreshServerMetricsList()
{
  
}

