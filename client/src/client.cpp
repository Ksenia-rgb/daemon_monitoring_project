#include "client.hpp"

void Client::run()
{
  ui_->registerCommand("update_servers", [this](const std::vector< std::string > &)
  {
    loadConfig();
  });
  ui_->run();
}

void Client::loadConfig()
{
  config_ = std::make_unique< ConfigFile >("client/config/init.json");
  ui_->updateServers(config_.get()->getServerInfoMap());
}

void Client::refreshAllMetrics()
{

}

void Client::refreshMetricsFor(const std::string & server_name)
{

}

