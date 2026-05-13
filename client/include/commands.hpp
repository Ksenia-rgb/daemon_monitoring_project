#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "client.hpp"

namespace client_commands
{
  void loadClientConfig(Client & client);
  void printTimeMetric(Client & client);
  void printIntervalMetrics(Client & client);
}

#endif
