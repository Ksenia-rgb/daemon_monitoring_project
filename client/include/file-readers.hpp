#ifndef FILE_READERS_HPP
#define FILE_READERS_HPP

#include <unordered_set>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

#include "common-types.hpp"

namespace fs = std::filesystem;

using json = nlohmann::json;

metric_value jsonToVariant(const json & j);

class ConfigFile
{
private:
  std::string config_filepath_;
  std::string metrics_filepath_;
  std::map< std::string, ServerInfo > servers_;

  void loadFile(const std::string & filepath);
public:
  ConfigFile() = default;
  ConfigFile(const std::string & filepath);

  void load(const std::string & filepath);
  void reload();

  const std::string & getConfigFilePath() const noexcept;
  const std::string & getMetricsFilePath() const noexcept;

  bool hasServer(const std::string & server_name) const noexcept;
  const ServerInfo & getServerInfo(const std::string & server_name) const;
  const std::map< std::string, ServerInfo > & getServerInfoMap() const noexcept;
};

class MetricsPackage
{
private:
  std::string server_name_ = "";
  std::string metrics_dir_path_ = "";
  std::vector< MetricsFile > metrics_;
  std::unordered_set< std::string > files_;
  std::vector< std::string > errors_;

  MetricsFile loadFile(const std::string & filepath);
public:
  MetricsPackage() = default;
  void load(const std::string & filepath_to_dir_or_file);
  void reload();
  const std::string & getMetricsDirectory() const noexcept;
  const std::string & getServerName() const noexcept;
  const std::vector< MetricsFile > & getServerMetrics() const noexcept;

  const std::vector< std::string > & traceErrors() const noexcept;
  void clearErrors() noexcept;
};

#endif
