#include "daemon.hpp"
#include <chrono>
#include <fstream>
#include <httplib.h>
#include <iostream>
#include <thread>
#include "collector-linux.hpp"
#include "collector-win.hpp"
#include "collector.hpp"

namespace
{
  Collector * getPlatformCollector()
  {
    #if IS_WINDOWS
      return new CollectorWin();
    #endif
    #if IS_LINUX
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

Daemon::Daemon(const models::DaemonConfig & config):
  name_(config.name),
  monitor_interval_(config.monitor_interval),
  repeat_interval_(config.repeat_interval),
  send_interval_(config.send_interval),
  min_send_interval_(config.min_send_interval),
  max_batch_size_(config.max_batch_size),
  send_strategy_(config.send_strategy),
  log_path_(config.log_path),
  send_buffer_(),
  repeat_buffer_(),
  running(false),
  buffer_mtx_(),
  repeat_mtx_(),
  send_mtx_(),
  log_mtx_()
{}

void Daemon::start()
{
  running.store(true);
}
void Daemon::runCollect()
{
  std::chrono::seconds interval(monitor_interval_);

  while (running.load())
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

  while (running.load())
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

  while (running.load())
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
  running.store(false);
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
    return {};
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
void Daemon::pushMetric(const nlohmann::json & metric)
{
  std::lock_guard< std::mutex > locker(buffer_mtx_);
  send_buffer_.push(metric);
}
nlohmann::json Daemon::makeJson(const MetricMap & metric, const std::string & time) const
{
  nlohmann::json rec;
  rec["time"] = time;
  for (auto iter = metric.begin(); iter != metric.end(); ++iter)
  {
    rec[iter->first] = iter->second;
  }
  return rec;
}
nlohmann::json Daemon::makeJsonBatch(const MetricBatch & metric) const
{
  nlohmann::json batch = nlohmann::json::array();
  for (auto iter = metric.begin(); iter != metric.end(); ++iter)
  {
    batch.push_back(*iter);
  }
  return batch;
}
void Daemon::send(const nlohmann::json & batch) const
{
  std::lock_guard< std::mutex > locker(send_mtx_);
  httplib::Client client(formHostAddress());
  auto response = client.Post(formHttpPath(), batch.dump(), "application/json");
  if (response)
  {
    std::lock_guard< std::mutex > locker(log_mtx_);
    std::ofstream fout(log_path_, std::ios::app);
    if (fout.is_open())
    {
      fout << response->body << ' ' << response->status << '\n';
    }
  }
}
std::string Daemon::formHostAddress() const
{
  std::string res = send_strategy_.url;
  if (res.back() != ':')
  {
    res += ":";
  }
  res += send_strategy_.port;
  return res;
}
std::string Daemon::formHttpPath() const
{
  std::string res;
  if (send_strategy_.endpoint.front() != '/')
  {
    res += "/";
  }
  res += send_strategy_.endpoint;
  res += "?name=" + name_;
  return res;
}
