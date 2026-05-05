#ifndef MANAGERS_HPP
#define MANAGERS_HPP

#include <filesystem>
#include <iostream>
#include <fstream>
#include <variant>
#include <chrono>
#include <map>

#include <nlohmann.hpp>

namespace fs = std::filesystem;

using json = nlohmann::json;
using metric_value = std::variant< int, float, bool, std::string >;

inline metric_value jsonToVariant(const json & j)
{
  if (j.is_number_integer())
  {
    return j.get< int >();
  }
  if (j.is_number_float())
  {
    return j.get< float >();
  }
  if (j.is_boolean())
  {
    return j.get< bool >();
  }
  if (j.is_string())
  {
    return j.get< std::string >();
  }
  throw std::runtime_error("Unsupported type");  //TODO exc
}

struct Threshold
{
  std::string type;
  metric_value value;
  int success_compare;
};

struct ServerInfo
{
  std::string name;
  std::string url;
  std::map< std::string, std::map< std::string, Threshold > > thresholds;
};

class ConfigManager
{
private:
  std::string config_filepath_;
  std::string metrics_filepath_;
  std::map< std::string, ServerInfo > servers_;

  void updateConfig(const std::string & filepath)
  {
    std::ifstream raw_cfg(filepath);
    if (!raw_cfg) {
      throw std::runtime_error("Cannot open config file: " + filepath); //TODO exc
    }
    auto cfg = json::parse(raw_cfg);

    metrics_filepath_ = cfg["metrics_filepath"];

    for (auto serv: cfg["servers"])
    {
      ServerInfo opt
      {
        .name = serv["name"],
        .url = serv["url"]
      };
      for (auto [pc_part, pc_part_metrics]: serv["thresholds"].items())
      {
        std::map< std::string, Threshold > temp;
        for (auto metric: pc_part_metrics)
        {
          temp[metric["type"]] = {
            .type = metric["type"],
            .value = jsonToVariant(metric["value"]),
            .success_compare = metric["success_compare"],
          };
        }
        opt.thresholds[pc_part] = std::move(temp);
      }
      servers_[serv["name"]] = std::move(opt);
    }
    config_filepath_ = filepath;
  }
public:
  ConfigManager(const std::string & filepath)
  {
    updateConfig(filepath);
  }

  void update()
  {
    updateConfig(config_filepath_);
  }

  void applyOther(const std::string & filepath)
  {
    updateConfig(filepath);
  }

  std::string getMetricsFilePath() const
  {
    return metrics_filepath_;
  }
  std::map< std::string, ServerInfo > getServerInfos() const
  {
    return servers_;
  }
  ServerInfo getServerInfo(const std::string & server) const
  {
    return servers_.at(server);
  }
};

struct Metric
{
  std::chrono::system_clock::time_point time;
  std::map< std::string, std::map< std::string, metric_value > > data;
};

struct ServerMetrics
{
  int version;
  int id;
  std::string name;
  size_t interval_value;
  std::string interval_units;
  std::vector< Metric > metrics;
};

class MetricsPackage
{
private:
  std::string server_name_;
  std::vector< ServerMetrics > metrics_;

  ServerMetrics readMetricsFromFile(const std::string & filepath) const
  {
    std::ifstream raw_package(filepath);
    if (!raw_package)
    {
      throw std::runtime_error("Cannot open config file");  //TODO exc
    }
    auto package = json::parse(raw_package);

    ServerMetrics current
    {
      .version = package["version"],
      .id = package["id"],
      .name = package["name"],
      .interval_value = package["interval_value"],
      .interval_units = package["interval_units"]
    };

    if (current.version != 1)
    {
      throw std::runtime_error("Unsupported version"); //TODO exc
    }

    if (current.name != server_name_)
    {
      throw std::runtime_error("Package should have same name"); //TODO exc
    }

    for (auto metric: package["metrics"])
    {
      std::tm tm = {};
      std::istringstream ss(metric["time"].get< std::string >());
      ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
      if (!ss)
      {
        throw std::runtime_error("Cannot parse time");  //TODO exc
      }
      std::time_t tt = std::mktime(&tm);
      auto tp = std::chrono::system_clock::from_time_t(tt);

      Metric opt
      {
        .time = std::move(tp)
      };
      for (auto [pc_part, pc_part_metrics]: metric["data"].items())
      {
        std::map< std::string, metric_value > temp;
        for (auto [metric_key, metric_value]: pc_part_metrics.items())
        {
          temp[metric_key] = jsonToVariant(metric_value);
        }
        opt.data[pc_part] = std::move(temp);
      }
      current.metrics.push_back(std::move(opt));
    }
    return current;
  }
public:
  MetricsPackage(const std::string & server_name, const std::string & filepath_to):
    server_name_(server_name)
  {
    bool was_error_package = false;
    if (fs::exists(filepath_to))
    {
      if (fs::is_directory(filepath_to))
      {
        for (const auto & entry: fs::directory_iterator(filepath_to))
        {
          try
          {
            metrics_.push_back(readMetricsFromFile(entry.path().string()));
          }
          catch (const std::runtime_error & e)
          {
            was_error_package = true;
            std::cerr << "Failed " << entry.path().string() << "\n";
          }
        }
      }
      else
      {
        metrics_.push_back(std::move(readMetricsFromFile(filepath_to)));
      }
    }
    if (was_error_package)
    {
      throw std::runtime_error("Some packages courrupted");
    }
  }
  std::vector< ServerMetrics > getServerMetrics() const
  {
    return metrics_;
  }
};

#endif
