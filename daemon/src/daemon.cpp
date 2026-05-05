#include "daemon.hpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <thread>
#include "collector.hpp"
#include "collector-linux.hpp"
#include "collector-win.hpp"

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

Daemon::Daemon(const models::DaemonConfig& config):
  monitor_interval_(config.monitor_interval),
  repeat_interval_(config.repeat_interval),
  send_interval_(config.send_interval),
  min_send_interval_(config.min_send_interval),
  send_strategy_(config.send_strategy),
  max_batch_size_(config.max_batch_size),
  send_buffer_(),
  repeat_buffer_(),
  running(false),
  buffer_mtx_(),
  repeat_mtx_(),
  send_mtx_()
{}

void Daemon::start()
{
  running = true;
}
void Daemon::runCollect()
{
  std::chrono::seconds interval(monitor_interval_);

  while (running)
  {
    std::pair< MetricMap, std::string > metric = getMetrics();
    nlohmann::json json_metric = makeJson(metric.first, metric.second);
    pushMetric(json_metric);

    std::this_thread::sleep_for(interval);
  }
}
void Daemon::runSend()
{
  std::chrono::seconds interval(send_interval_);

  while (running)
  {
    MetricBatch batch = collectMetricBatch();
    if (!batch.empty())
    {
      nlohmann::json json_batch = makeJsonBatch(batch);
      send(json_batch);
    }

    std::this_thread::sleep_for(interval);
  }
}
void Daemon::runRepeat()
{
  std::chrono::seconds interval(repeat_interval_);
 
  while (running)
  {
    MetricBatch batch = collectRepeatBatch();
    if (!batch.empty())
    {
      nlohmann::json json_batch = makeJsonBatch(batch);
      send(json_batch);
    }

    std::this_thread::sleep_for(interval);
  }
}
void Daemon::stop()
{
  running = false;
}
void Daemon::remainingCases()
{
  while (!send_buffer_.empty())
  {
    MetricBatch last_batch = collectMetricBatch();
    if (!last_batch.empty())
    {
      nlohmann::json json_batch = makeJsonBatch(last_batch);
      send(json_batch);
    }
  }
  while (!repeat_buffer_.empty())
  {
    MetricBatch last_batch = collectRepeatBatch();
    if (!last_batch.empty())
    {
      nlohmann::json json_batch = makeJsonBatch(last_batch);
      send(json_batch);
    }
  }
}
std::pair< Daemon::MetricMap, std::string > Daemon::getMetrics() const
{
  Collector * collector = getPlatformCollector();
  if (!collector)
  {
    throw std::logic_error("<Error> unsupported platform");
  }
  MetricMap res = collector->getMetrics();
  delete collector;
  return {res, getUtcTimestamp()};
}
Daemon::MetricBatch Daemon::collectMetricBatch()
{
  if (send_buffer_.empty())
  {
    return {};
  }
  MetricBatch batch;

  std::lock_guard< std::mutex > buffer_locker(buffer_mtx_);
  std::lock_guard< std::mutex > repeat_locker(repeat_mtx_);

  while (batch.size() < max_batch_size_ && !send_buffer_.empty())
  {
    batch.push_back(send_buffer_.front());
    repeat_buffer_.push(send_buffer_.front());
    send_buffer_.pop();
  }
  return batch;
}
Daemon::MetricBatch Daemon::collectRepeatBatch()
{
  if (repeat_buffer_.empty())
  {
    return {};
  }
  MetricBatch batch;

  std::lock_guard< std::mutex > repeat_locker(repeat_mtx_);

  while (batch.size() < max_batch_size_ && !repeat_buffer_.empty())
  { 
    batch.push_back(repeat_buffer_.front());
    repeat_buffer_.pop();
  }
  return batch;
}
void Daemon::pushMetric(const nlohmann::json& metric)
{
  std::lock_guard< std::mutex > locker(buffer_mtx_);
  send_buffer_.push(metric);
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
  std::lock_guard< std::mutex > locker(send_mtx_);
  std::ofstream fout(send_strategy_.url + send_strategy_.endpoint, std::ios::app);
  if (!fout.is_open())
  {
    return;
  }
  fout << batch.dump(2) << '\n';
}
