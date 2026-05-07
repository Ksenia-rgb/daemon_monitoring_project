#include "shared-storage.hpp"
#include <iostream>

SharedStorage::SharedStorage(const models::SharedStorageConfig& config):
  storage_path_(config.storage_path),
  client_endpoint_(config.client_endpoint),
  daemon_endpoint_(config.daemon_endpoint),
  port_(config.port),
  daemon_storage_(),
  map_mtx_()
{
  namespace fs = std::filesystem;
  fs::path directory(storage_path_);
  if (!fs::exists(directory))
  {
    fs::create_directory(directory);
  }
}
bool SharedStorage::hasMetric(const std::string& name, const std::string& time) const
{
  std::lock_guard< std::mutex > locker(map_mtx_);
  if (daemon_storage_.find(name) == daemon_storage_.end())
  {
    return false;
  }
  return daemon_storage_.find(name)->second->hasTimedMetric(time);
}
void SharedStorage::addMetric(const std::string& name, const nlohmann::json& metric)
{
  std::lock_guard< std::mutex > locker(map_mtx_);
  if (daemon_storage_.find(name) == daemon_storage_.end())
  {
    daemon_storage_.emplace(name, std::make_unique< SingleDaemonStorage >(name, storage_path_));
  }
  daemon_storage_.find(name)->second->addMetric(metric);
}
void SharedStorage::addMetricBatch(const std::string& name, const nlohmann::json& metric)
{
  std::lock_guard< std::mutex > locker(map_mtx_);
  if (daemon_storage_.find(name) == daemon_storage_.end())
  {
    daemon_storage_.emplace(name, std::make_unique< SingleDaemonStorage >(name, storage_path_));
  }
  daemon_storage_.find(name)->second->addMetricBatch(metric);
}
nlohmann::json SharedStorage::getTimedMetric(const std::string& name, const std::string& time) const
{
  std::lock_guard< std::mutex > locker(map_mtx_);
  if (daemon_storage_.find(name) == daemon_storage_.cend())
  {
    return {};
  }
  return daemon_storage_.find(name)->second->getTimedMetric(time);
}
nlohmann::json SharedStorage::getIntervalBatch(const std::string& name,
  const std::string& begin, const std::string& end) const
{
  std::lock_guard< std::mutex > locker(map_mtx_);
  if (daemon_storage_.find(name) == daemon_storage_.cend())
  {
    return {};
  }
  return daemon_storage_.find(name)->second->getIntervalBatch(begin, end);
}
