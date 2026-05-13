#include "client.hpp"
#include "commands.hpp"

Client::Client(const ClientConfig & config, std::unique_ptr< UI > ui):
  config_(config),
  ui_(std::move(ui))
{}

void Client::run()
{
  ui_->registerCommand("load-config", std::bind(client_commands::loadClientConfig, std::ref(*this)));
  ui_->registerCommand("print-time-metric", std::bind(client_commands::printTimeMetric, std::ref(*this)));
  ui_->registerCommand("print-interval-metrics", std::bind(client_commands::printIntervalMetrics, std::ref(*this)));

  ui_->run();
}

const ClientConfig & Client::getConfig() const noexcept
{
  return config_;
}

void Client::updateConfig(const std::string & config_path)
{
  ClientConfig newConfig;
  newConfig.load(config_path);
  config_ = newConfig;
}

std::string Client::getTimeMetric(const std::string & server_name, const std::string & timestamp) const
{
  if (config_.getServers().find(server_name) == config_.getServers().end())
  {
    throw std::invalid_argument("invalid server name");
  }

  return get(config_.getGetStrategy().endpoint + "?name=" + server_name + "&time=" + timestamp);
}

std::string Client::getIntervalMetrics(
    const std::string & server_name, const std::string & begin_timestamp, const std::string & end_timestamp) const
{
  if (config_.getServers().find(server_name) == config_.getServers().end())
  {
    throw std::invalid_argument("invalid server name");
  }

  return get(config_.getGetStrategy().endpoint + "?name=" + server_name + "&begin=" + begin_timestamp
      + "&end=" + end_timestamp);
}

std::string Client::get(const std::string & query) const
{
  GetStrategy get_strategy = config_.getGetStrategy();
  httplib::Client client(get_strategy.scheme + "://" + get_strategy.host + ':' + get_strategy.port);

  auto res = client.Get(query);
  if (!res)
  {
    throw std::runtime_error(httplib::to_string(res.error()));
  }
  if (res->status != 200)
  {
    throw std::runtime_error("HTTP error: " + res->status);
  }

  return res->body;
}
