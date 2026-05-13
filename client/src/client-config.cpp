#include "client-config.hpp"
#include <fstream>

void ClientConfig::load(const std::string & filepath)
{
  std::ifstream raw_cfg(filepath);
  if (!raw_cfg)
  {
    throw std::invalid_argument("cannot open config file");
  }
  auto cfg = json::parse(raw_cfg);

  std::unordered_map< std::string, ServerThresholds > servers;
  for (const auto & threshold : cfg["thresholds"].items())
  {
    std::string server_name = threshold.key();
    if (servers.find(server_name) != servers.end())
    {
      throw std::runtime_error("duplicate server: " + server_name);
    }
    servers[server_name] = threshold.value();
  }

  config_filepath_ = filepath;
  servers_ = std::move(servers);
  get_strategy_ = cfg["get_strategy"];
}

const std::string & ClientConfig::getClientConfigFilePath() const noexcept
{
  return config_filepath_;
}

const std::unordered_map< std::string, ServerThresholds > & ClientConfig::getServers() const noexcept
{
  return servers_;
}

const GetStrategy & ClientConfig::getGetStrategy() const noexcept
{
  return get_strategy_;
}

bool ClientConfig::hasServer(const std::string & server_name) const noexcept
{
  return servers_.find(server_name) != servers_.end();
}

const ServerThresholds & ClientConfig::getServerThresholds(const std::string & server_name) const
{
  return servers_.at(server_name);
}
