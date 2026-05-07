#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <atomic>
#include <json.hpp>
#include <map>
#include <mutex>
#include <queue>
#include <vector>
#include "models.hpp"

class Daemon
{
public:
  using MetricMap = std::map< std::string, float >;
  using MetricBatch = std::vector< nlohmann::json >;

  Daemon(const models::DaemonConfig & config);
  Daemon(const Daemon &) = delete;
  Daemon(Daemon &&) = delete;
  ~Daemon() = default;

  Daemon & operator=(const Daemon &) = delete;
  Daemon & operator=(Daemon &&) = delete;

  void start();
  void runCollect();
  void runSend();
  void runRepeat();
  void runEmergency();
  void stop();
  void remainingCases();

private:
  std::string name_;
  unsigned monitor_interval_;
  unsigned repeat_interval_;
  unsigned send_interval_;
  unsigned min_send_interval_;
  size_t max_batch_size_;
  models::SendStrategy send_strategy_;
  std::string log_path_;

  std::queue< nlohmann::json > send_buffer_;
  std::queue< nlohmann::json > repeat_buffer_;

  std::atomic< bool > running;

  mutable std::mutex buffer_mtx_;
  mutable std::mutex repeat_mtx_;
  mutable std::mutex send_mtx_;
  mutable std::mutex log_mtx_;

  std::pair< MetricMap, std::string > getMetrics() const;
  void pushMetric(const nlohmann::json & metric);
  MetricBatch collectMetricBatch();
  MetricBatch collectRepeatBatch();
  nlohmann::json makeJson(const MetricMap & metric, const std::string & time) const;
  nlohmann::json makeJsonBatch(const MetricBatch & metric_batch) const;

  void send(const nlohmann::json & batch) const;
  std::string formHostAddress() const;
  std::string formHttpPath() const;
};

#endif
