#include "client.hpp"

void Client::run()
{
  ui_->registerCommand("update_servers", [this](const std::vector< std::string > &)
  {
    loadConfig();
  });
  ui_->registerCommand("refresh_metric_for", [this](const std::vector< std::string > & name)
  {
    if (name.size())
    {
      refreshMetricsFor(name[0]);
    }
  });
  ui_->run();
}

void Client::loadConfig()
{
  config_ = std::make_unique< ConfigFile >("/home/faxryzen/projects/daemon_monitoring_project/client/config/init.json");
  ui_->updateServers(config_.get()->getServerInfoMap());
}

void Client::refreshAllMetrics()
{
  
}

void Client::refreshMetricsFor(const std::string & server_name)
{
  MetricsPackage metrics;
  //todo: check for config existing
  metrics.load(config_.get()->getMetricsFilePath() + server_name);
  //todo: comparing with critical values
  auto temp = metrics.getServerMetrics();
  std::vector< std::pair< std::chrono::system_clock::time_point, metric_value > > result;
  for (size_t i = 0; i < temp.size(); ++i)
  {
    for (size_t j = 0; j < temp[0].metrics.size(); ++j)
    {
      //todo safety and smarter blyat interface because this so complicated..
      result.push_back({temp[i].metrics[j].time, temp[i].metrics[j].data["gpu"]["usage"]});
    }
  }

  ui_->updateMetricGraph(server_name, "GPU USAGE", result);
}

