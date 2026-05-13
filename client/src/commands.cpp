#include "commands.hpp"
#include <iostream>
#include <string>

void client_commands::loadClientConfig(Client & client)
{
  std::string config_path;
  std::cin >> config_path;

  client.updateConfig(config_path);

  std::cout << "new path: " << client.getConfig().getClientConfigFilePath() << '\n';
}

void client_commands::printTimeMetric(Client & client)
{
  std::string server_name, timestamp;
  std::cin >> server_name >> timestamp;

  std::string res;
  try
  {
    res = client.getTimeMetric(server_name, timestamp);
  }
  catch (const std::runtime_error & e)
  {
    std::cerr << "error: " << e.what() << '\n';
    return;
  }

  std::cout << res << '\n';
}

void client_commands::printIntervalMetrics(Client & client)
{
  std::string server_name, begin_timestamp, end_timestamp;
  std::cin >> server_name >> begin_timestamp >> end_timestamp;

  std::string res;
  try
  {
    res = client.getIntervalMetrics(server_name, begin_timestamp, end_timestamp);
  }
  catch (const std::runtime_error & e)
  {
    std::cerr << "error: " << e.what() << '\n';
    return;
  }

  std::cout << res << '\n';
}
