#include "../include/daemon.hpp"

#include <chrono>
#include <iostream>

#include "../include/collector.hpp"
#include "../include/collector-linux.hpp"
#include "../include/collector-win.hpp"


namespace
{
  Collector* getPlatformCollector()
  {
    #if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
      return new CollectorWin();
    #endif
    #if defined(__linux__) || defined(linux) || defined(__linux) || defined(__unix__) || defined(__unix)
      return new CollectorLinux();
    #endif
    return nullptr;
  }

  std::string getUtcTimestamp()
  {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto utc_time = std::gmtime(&now_time_t);
    
    std::ostringstream oss;
    oss << std::put_time(utc_time, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
  }
}

Daemon::Daemon(double monitor_interval, double repeat_interval, double send_interval,
    double min_send_strategy, SendStrategy send_strategy, double max_batch_size):
  monitor_interval_(monitor_interval),
  repeat_interval_(repeat_interval),
  send_interval_(send_interval),
  min_send_interval_(min_send_strategy),
  send_strategy_(send_strategy),
  max_batch_size_(max_batch_size),
  send_buffer(),
  repeat_send_buffer()
{}
void Daemon::run()
{
  /*
  1 thread: собирать метрики, записывать в send_buffer
  2 thread: забирать из send_buffer, отправлять, класть в repeat_buffer
  3 thread: забирать из repeat_buffer, отправлять повторно
  4 thread: возможно, контролировать метрики, вызывать отправку при превышении порога
  */

  
}
Daemon::MetricMap Daemon::getMetrics() const
{
  Collector * collector = getPlatformCollector();
  if (!collector)
  {
    throw std::logic_error("unsupported platform");
  }
  MetricMap res = collector->getMetrics();
  delete collector;
  return res;
}
Daemon::MetricBatch Daemon::formMetricBatch()
{
  if (send_buffer.size() < max_batch_size_) {
    return {};
  }
  MetricBatch batch;
  for (size_t i = 0; i < max_batch_size_; i++) {
    batch.push_back(send_buffer.front());
    repeat_send_buffer.push(send_buffer.front());
    send_buffer.pop();
  }
  return batch;
}
nlohmann::json Daemon::makeJson(const MetricMap& metric, const std::string& time) const
{
  nlohmann::json rec;
  rec["timestamp"] = time;
  for (auto iter = metric.begin(); iter != metric.end(); ++iter)
  {
    rec[iter->first] = iter->second;
  }
  return rec;
}
nlohmann::json Daemon::makeJsonBatch(const MetricBatch& metric) const
{
  nlohmann::json batch = nlohmann::json::array();
  for (auto iter = metric.begin(); iter != metric.end(); ++iter)
  {
    batch.push_back(*iter);
  }
  return batch;
}
void Daemon::send(const nlohmann::json& batch)
{
  std::cout << batch.dump(2) << '\n';
}
