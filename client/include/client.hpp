#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <httplib.h>
#include <memory>
#include "client-config.hpp"
#include "ui.hpp"

class Client
{
public:
  Client(const ClientConfig & config, std::unique_ptr< UI > ui);

  Client(const Client &) = delete;
  Client(Client &&) = delete;
  ~Client() = default;

  Client & operator=(const Client &) = delete;
  Client & operator=(Client &&) = delete;

  void run();
  const ClientConfig & getConfig() const noexcept;
  void updateConfig(const std::string & config_path);
  std::string getTimeMetric(const std::string & server_name, const std::string & timestamp) const;
  std::string getIntervalMetrics(
      const std::string & server_name, const std::string & begin_timestamp, const std::string & end_timestamp) const;

protected:
  ClientConfig config_;
  std::unique_ptr< UI > ui_;

  std::string get(const std::string & query) const;
};

#endif
