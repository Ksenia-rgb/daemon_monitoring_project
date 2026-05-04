#ifndef DAEMON_HPP
#define DAEMON_HPP
#include "../nlohmann.hpp"
#include <map>
#include <vector>
#include <queue>

#include "models.hpp"

class Daemon
{
public:
  using MetricMap = std::map< std::string, double >;
  using MetricBatch = std::vector< nlohmann::json >;

  Daemon(double monitor_interval, double repeat_interval, double send_interval,
    double min_send_strategy, SendStrategy send_strategy, double max_batch_size);
  Daemon(const Daemon&) = delete;
  Daemon(Daemon&&) = delete;
  ~Daemon() = default;

  Daemon& operator=(const Daemon&) = delete;
  Daemon& operator=(Daemon&&) = delete;

  void run();
  
private:
  double monitor_interval_;
  double repeat_interval_;
  double send_interval_;
  double min_send_interval_;
  SendStrategy send_strategy_;
  double max_batch_size_;

  std::queue< nlohmann::json > send_buffer;
  std::queue< nlohmann::json > repeat_send_buffer;

  MetricMap getMetrics() const;
  MetricBatch formMetricBatch();
  nlohmann::json makeJson(const MetricMap& metric, const std::string& time) const;
  nlohmann::json makeJsonBatch(const MetricBatch& metric_batch) const;

  void send(const nlohmann::json& batch);
};

#endif
