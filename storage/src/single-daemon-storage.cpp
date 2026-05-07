#include "single-daemon-storage.hpp"
#include <filesystem>
#include <iostream>
#include <time.h>
#include <chrono>

namespace
{
  std::time_t timegm_plt(std::tm* tm) {
    #ifdef _WIN32
        return _mkgmtime(tm);
    #else
        return ::timegm(tm);
    #endif
  }
  std::time_t parseTime(const std::string& time_str)
  {
    std::tm time{};
    std::istringstream iss(time_str);
    iss >> std::get_time(&time, "%Y-%m-%dT%H:%M:%SZ");
    return timegm_plt(&time);
  }
  std::string getFileName(const std::string& name, const std::string& storage_path)
  {
    if (storage_path.back() != '/')
    {
      return storage_path + "/" + name + "/metric.json";
    }
    return storage_path + name + "/metric.json";
  }
}

SingleDaemonStorage::SingleDaemonStorage(const std::string& name, const std::string& storage_path):
  name_(name),
  path_(getFileName(name, storage_path)),
  mtx_()
{
  namespace fs = std::filesystem;
  fs::path directory(storage_path);
  if (!fs::exists(directory / name))
  {
    fs::create_directory(directory / name);
  }
  fs::path file = directory / name / "metric.json";
}
bool SingleDaemonStorage::hasTimedMetric(const std::string& time) const
{
  using json = nlohmann::json;
  std::lock_guard< std::mutex > locker(mtx_);
  std::ifstream fin(path_);
  if (!fin.is_open())
  {
    return false;
  }
  
  std::string line;
  while (std::getline(fin, line))
  {
    if (line.empty())
    {
      continue;
    }
    try
    {
      json item = json::parse(line);
      if (item.contains("time") && item["time"] == time)
      {
        return true;
      }
    }
    catch (const std::exception& e)
    {
      continue;
    }
  }
  return false;
}
void SingleDaemonStorage::addMetric(const nlohmann::json& metric)
{
  using json = nlohmann::json;
  if (metric.contains("time") && hasTimedMetric(metric["time"]))
  {
    return;
  }
  std::lock_guard< std::mutex > locker(mtx_);
  std::ofstream fout(path_, std::ios::app);
  if (!fout.is_open())
  {
    return;
  }
  fout << metric.dump() << '\n';
}
void SingleDaemonStorage::addMetricBatch(const nlohmann::json& batch)
{
  using json = nlohmann::json;
  if (!batch.is_array())
  {
    return;
  }
  for (const json& item: batch)
  {
    addMetric(item);
  }
}
nlohmann::json SingleDaemonStorage::getTimedMetric(const std::string& time) const
{
  using json = nlohmann::json;
  std::lock_guard< std::mutex > locker(mtx_);
  std::ifstream fin(path_);
  if (!fin.is_open())
  {
    return {};
  }
  
  std::string line;
  while (std::getline(fin, line))
  {
    if (line.empty())
    {
      continue;
    }
    try
    {
      json item = json::parse(line);
      if (item.contains("time") && item["time"] == time)
      {
        return item;
      }
    }
    catch (const std::exception& e)
    {
      continue;
    }
  }
  return {};
}
nlohmann::json SingleDaemonStorage::getIntervalBatch(const std::string& begin, const std::string& end) const
{
  using json = nlohmann::json;

  time_t begin_tm = parseTime(begin);
  time_t end_tm = parseTime(end);
  json res = json::array();

  std::lock_guard< std::mutex > locker(mtx_);
  std::ifstream fin(path_, std::ios::app);
  if (!fin.is_open())
  {
    return res;
  }

  std::string line;
  while (std::getline(fin, line))
  {
    if (line.empty())
    {
      continue;
    }
    try
    {
      json item = json::parse(line);
      if (!item.contains("time"))
      {
        continue;
      }
      time_t item_tm = parseTime(item["time"]);
      if (item_tm >= begin_tm && item_tm <= end_tm)
      {
        res.push_back(item);
      }
    }
    catch (const std::exception&)
    {
      continue;
    }
  }
  return res;
}



