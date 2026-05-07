#include "client.hpp"
#include <httplib.h>

Client::Client(std::unique_ptr< UI > ui):
  ui_(std::move(ui))
{}

void Client::run()
{
  httplib::Client client("http://localhost:8080");

  ui_->registerCommand("update_servers",
      [this]()
      {
        loadConfig();
      });
  // ui_->registerCommand("refresh_metric_for",
  //     [this]()
  //     {
  //       int name_number = 0;
  //       std::cin >> name_number;
  //       std::string name;
  //       for (int i = 0; i < name_number; ++i)
  //       {
  //         std::cin >> name;
  //         refreshMetricsFor(name);
  //       }
  //     });
  ui_->registerCommand("get-time-metric",
      [this, &client]()
      {
        std::string server_name, time;
        std::cin >> server_name >> time;

        auto res = client.Get("/api/get?name=" + server_name + "&time=" + time);
        if (!res)
        {
          std::cerr << "error: " << httplib::to_string(res.error()) << std::endl;
          return;
        }
        if (res->status != 200)
        {
          std::cerr << "HTTP error: " << res->status << std::endl;
          return;
        }

        std::cout << res->body << std::endl;
      }
    );

    ui_->registerCommand("get-interval-metric",
        [this, &client]()
        {
          std::string server_name, time_begin, time_end;
          std::cin >> server_name >> time_begin >> time_end;

          auto res = client.Get("/api/get?name=" + server_name + "&begin=" + time_begin + "&end=" + time_end);
          if (!res)
          {
            std::cerr << "error: " << httplib::to_string(res.error()) << std::endl;
            return;
          }
          if (res->status != 200)
          {
            std::cerr << "HTTP error: " << res->status << std::endl;
            return;
          }

          std::cout << res->body << std::endl;
        }
      );

  ui_->run();
}

void Client::loadConfig()
{
  config_ = std::make_unique< ConfigFile >("client/config/init.json");
  ui_->updateServers(config_.get()->getServerInfoMap());
}

void Client::refreshAllMetrics()
{}

void Client::refreshMetricsFor(const std::string & server_name)
{
  MetricsPackage metrics;
  // todo: check for config existing
  metrics.load(config_.get()->getMetricsFilePath() + server_name);
  // todo: comparing with critical values
  auto temp = metrics.getServerMetrics();
  std::vector< std::pair< std::chrono::system_clock::time_point, metric_value > > result;
  for (size_t i = 0; i < temp.size(); ++i)
  {
    for (size_t j = 0; j < temp[0].metrics.size(); ++j)
    {
      // todo safety and smarter interface because this so complicated..
      result.push_back({temp[i].metrics[j].time, temp[i].metrics[j].data["gpu"]["usage"]});
    }
  }

  ui_->updateMetricGraph(server_name, "GPU USAGE", result);
}
