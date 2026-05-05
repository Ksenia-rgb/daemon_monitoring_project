#include "file-readers.hpp"

#include <set>

metric_value jsonToVariant(const json & j)
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
  throw std::runtime_error("unsupported type");
}

ConfigFile::ConfigFile(const std::string & filepath)
{
  loadFile(filepath);
}

void ConfigFile::loadFile(const std::string & filepath)
{
  try
  {
    std::ifstream raw_cfg(filepath);
    if (!raw_cfg)
    {
      throw std::runtime_error("cannot open config file");
    }
    auto cfg = json::parse(raw_cfg);

    std::map< std::string, ServerInfo > servers;

    for (const auto & serv: cfg["servers"])
    {
      ServerInfo opt
      {
        .name = serv["name"],
        .url = serv["url"]
      };
      for (const auto & [pc_part, pc_part_metrics]: serv["thresholds"].items())
      {
        std::map< std::string, Threshold > temp;
        for (const auto & metric: pc_part_metrics)
        {
          temp[metric["type"]] = {
            .type = metric["type"],
            .value = jsonToVariant(metric["value"]),
            .success_compare = metric["success_compare"],
          };
        }
        opt.thresholds[pc_part] = std::move(temp);
      }
      std::string server_name = serv["name"];
      if (servers.find(server_name) != servers.end())
      {
        throw std::runtime_error("duplicate server: " + server_name);
      }
      servers[server_name] = std::move(opt);
    }
    metrics_filepath_ = cfg.value("metrics_filepath", "client/metrics/");
    config_filepath_ = filepath;
    servers_ = std::move(servers);
  }
  catch (const std::exception & e)
  {
    throw std::runtime_error("load config file error: " + std::string{e.what()} + " in " + filepath);
  }
}

void ConfigFile::load(const std::string & filepath)
{
  load(filepath);
}

void ConfigFile::reload()
{
  load(config_filepath_);
}

const std::string & ConfigFile::getConfigFilePath() const noexcept
{
  return config_filepath_;
}

const std::string & ConfigFile::getMetricsFilePath() const noexcept
{
  return metrics_filepath_;
}

const std::map< std::string, ServerInfo > & ConfigFile::getServerInfoMap() const noexcept
{
  return servers_;
}

bool ConfigFile::hasServer(const std::string & server_name) const noexcept
{
  return servers_.find(server_name) != servers_.end();
}

const ServerInfo & ConfigFile::getServerInfo(const std::string & server_name) const
{
  return servers_.at(server_name);
}

MetricsFile MetricsPackage::loadFile(const std::string & filepath)
{
  try
  {
    std::ifstream raw_package(filepath);
    if (!raw_package)
    {
      throw std::runtime_error("cannot open metrics file");
    }
    auto package = json::parse(raw_package);

    MetricsFile current
    {
      .version = package["version"],
      .id = package["id"],
      .name = package["name"],
      .interval_value = package["interval_value"],
      .interval_units = package["interval_units"]
    };

    if (current.version != 1)
    {
      throw std::runtime_error("unsupported version");
    }

    if (server_name_.empty())
    {
      server_name_ = current.name;
    }
    else if (current.name != server_name_)
    {
      throw std::runtime_error("files should have same server name");
    }

    for (const auto & metric: package["metrics"])
    {
      std::tm tm = {};
      std::istringstream ss(metric["time"].get< std::string >());
      ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
      if (!ss)
      {
        throw std::runtime_error("cannot parse time");
      }
      std::time_t tt = std::mktime(&tm);
      auto tp = std::chrono::system_clock::from_time_t(tt);

      Metric opt
      {
        .time = tp
      };
      for (const auto & [pc_part, pc_part_metrics]: metric["data"].items())
      {
        std::map< std::string, metric_value > temp;
        for (const auto & [metric_key, metric_value]: pc_part_metrics.items())
        {
          temp[metric_key] = jsonToVariant(metric_value);
        }
        opt.data[pc_part] = std::move(temp);
      }
      current.metrics.push_back(std::move(opt));
    }
    return current;
  }
  catch (const std::exception & e)
  {
    throw std::runtime_error("load metrics file error: " + std::string{e.what()} + " in " + filepath);
  }
}

void MetricsPackage::load(const std::string & filepath_to_dir_or_file)
{
  if (!fs::exists(filepath_to_dir_or_file))
  {
    throw std::runtime_error("load metrics package error: cannot open file: " + filepath_to_dir_or_file);
  }
  if (!fs::is_directory(filepath_to_dir_or_file))
  {
    auto file_name = fs::path{filepath_to_dir_or_file}.filename();
    if (files_.find(file_name) == files_.end())
    {
      metrics_.push_back(loadFile(filepath_to_dir_or_file));
      files_.insert(file_name);
    }
    return;
  }
  bool was_error_metrics = false;
  metrics_dir_path_ = filepath_to_dir_or_file;
  for (const auto & entry: fs::directory_iterator(filepath_to_dir_or_file))
  {
    auto file_name = entry.path().filename();
    try
    {
      if (files_.find(file_name) == files_.end())
      {
        metrics_.push_back(loadFile(entry.path().string()));
        files_.insert(file_name);
      }
    }
    catch (const std::runtime_error & e)
    {
      was_error_metrics = true;
      errors_.push_back(e.what());
    }
  }
  if (was_error_metrics)
  {
    throw std::runtime_error("load metrics package error: any files invalid");
  }
}
void MetricsPackage::reload()
{
  load(metrics_dir_path_);
}
const std::string & MetricsPackage::getMetricsDirectory() const noexcept
{
  return metrics_dir_path_;
}
const std::string & MetricsPackage::getServerName() const noexcept
{
  return server_name_;
}
const std::vector< MetricsFile > & MetricsPackage::getServerMetrics() const noexcept
{
  return metrics_;
}
const std::vector< std::string > & MetricsPackage::traceErrors() const noexcept
{
  return errors_;
}
void MetricsPackage::clearErrors() noexcept
{
  errors_.clear();
}

