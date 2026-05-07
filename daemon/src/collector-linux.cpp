#include "collector-linux.hpp"

#if IS_LINUX
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/statvfs.h>

float CollectorLinux::getCpuUsage() const
{
  static unsigned long user_time_prev = 0, system_time_prev = 0, idle_time_prev = 0;
  std::ifstream fin("/proc/stat");
  if (!fin.is_open())
  {
    return -1.0;
  }
  std::string cpu_stat;
  std::getline(fin, cpu_stat);
  cpu_stat = std::string(cpu_stat.begin() + cpu_stat.find(" "), cpu_stat.end());

  std::stringstream sin;
  unsigned long user_time = 0, nice_time = 0, system_time = 0, idle_time = 0;
  sin << cpu_stat;
  sin >> user_time >> nice_time >> system_time >> idle_time;

  unsigned long user_diff = user_time + nice_time - user_time_prev;
  unsigned long system_diff = system_time - system_time_prev;
  unsigned long idle_diff = idle_time - idle_time_prev;
  user_time_prev = user_time + nice_time;
  system_time_prev = system_time;
  idle_time_prev = idle_time;

  float total = static_cast< float >(user_diff + system_diff);
  float idle = static_cast< float >(idle_diff);

  return total ? std::ceil((1.0 - idle / (total + idle)) * 100.0) : -1.0;
}
float CollectorLinux::getMemoryUsage() const
{
  std::ifstream fin("/proc/meminfo");
  if (!fin.is_open())
  {
    return -1.0;
  }
  std::string mem_total;
  std::string mem_free;
  std::getline(fin, mem_total);
  std::getline(fin, mem_free);
  mem_total = std::string(mem_total.begin() + mem_total.find(" "), mem_total.end());
  mem_free = std::string(mem_free.begin() + mem_free.find(" "), mem_free.end());

  std::stringstream sin;
  std::string dummy;
  float mem_total_val = 0, mem_free_val = 0;

  sin << mem_total << mem_free;
  sin >> mem_total_val >> dummy >> mem_free_val;
  return mem_total_val ? std::ceil((1.0 - mem_free_val / mem_total_val) * 100.0) : -1.0;
}
float CollectorLinux::getDiskUsage() const
{
  struct statvfs stat;
  if (statvfs("/", &stat))
  {
    return -1.0;
  }
  float total = stat.f_blocks * stat.f_frsize;
  float free = stat.f_bavail * stat.f_frsize;
  return total ? std::ceil((1.0 - free / total) * 100.0) : -1.0;
}
float CollectorLinux::getTemperature() const
{
  return -1.0;
}

#endif
