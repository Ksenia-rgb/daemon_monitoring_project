#ifndef CLIENT_CONFIG_HPP
#define CLIENT_CONFIG_HPP

#include <filesystem>
#include <iostream>
#include <json.hpp>
#include "common-types.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

class ClientConfig
{
public:
  void load(const std::string & filepath);
  const std::string & getClientConfigFilePath() const noexcept;
  const std::unordered_map< std::string, ServerThresholds > & getServers() const noexcept;
  const GetStrategy & getGetStrategy() const noexcept;
  bool hasServer(const std::string & server_name) const noexcept;
  const ServerThresholds & getServerThresholds(const std::string & server_name) const;

private:
  std::string config_filepath_;
  std::unordered_map< std::string, ServerThresholds > servers_;
  GetStrategy get_strategy_;
};

#endif
