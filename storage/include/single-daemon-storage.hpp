#ifndef SINGLE_DAEMON_STORAGE_HPP
#define SINGLE_DAEMON_STORAGE_HPP

#include <map>
#include <mutex>
#include <fstream>
#include <string>
#include <json.hpp>

class SingleDaemonStorage
{
public:
  SingleDaemonStorage(const std::string& name, const std::string& storage_path);
  bool hasTimedMetric(const std::string& time) const;
  void addMetric(const nlohmann::json& metric);
  void addMetricBatch(const nlohmann::json& batch);
  nlohmann::json getTimedMetric(const std::string& time) const;
  nlohmann::json getIntervalBatch(const std::string& begin, const std::string& end) const;
private:
  std::string name_;
  std::string path_;
  mutable std::mutex mtx_;
};

#endif
