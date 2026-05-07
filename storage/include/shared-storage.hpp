#ifndef SHARED_STORAGE_HPP
#define SHARED_STORAGE_HPP
#include "single-daemon-storage.hpp"
#include "models.hpp"

class SharedStorage
{
public:
  SharedStorage(const models::SharedStorageConfig& config);
  bool hasMetric(const std::string& name, const std::string& time) const;
  void addMetric(const std::string& name, const nlohmann::json& metric);
  void addMetricBatch(const std::string& name, const nlohmann::json& metric);
  nlohmann::json getTimedMetric(const std::string& name, const std::string& time) const;
  nlohmann::json getIntervalBatch(const std::string& name, const std::string& begin, const std::string& end) const;
private:
  std::string storage_path_;
  std::string client_endpoint_;
  std::string daemon_endpoint_;
  int port_;
  std::map< std::string, std::unique_ptr< SingleDaemonStorage > > daemon_storage_;
  mutable std::mutex map_mtx_;
};

#endif