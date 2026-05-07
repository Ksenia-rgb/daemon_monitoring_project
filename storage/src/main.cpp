#include <iostream>
#include <filesystem>
#include <httplib.h>
#include <json.hpp>

#include "shared-storage.hpp"
#include "models.hpp"

namespace
{
  models::SharedStorageConfig parseConfig(const std::string& path)
  {
    std::ifstream fin(path);
    if (!fin.is_open())
    {
      throw std::logic_error("<Error>: not found config file");
    }
    nlohmann::ordered_json json_config = nlohmann::ordered_json::parse(fin);
    std::cout << json_config.dump(2) << '\n';
    return json_config.get< models::SharedStorageConfig >();
  }
  void daemonRun(SharedStorage& storage, const httplib::Request& req, httplib::Response& res)
  {
    using json = nlohmann::json;
    if (!req.has_param("name"))
    {
      json err = {{"error", "parametere 'name' is required"}};
      res.set_content(err.dump(), "application/json");
      res.status = 400;
      return;
    }
    json data;
    try
    {
      data = json::parse(req.body);
    }
    catch (const std::exception& e)
    {
      json err = {{"error", "json body is incorrect"}};
      res.set_content(err.dump(), "application/json");
      res.status = 400;
      return;
    }
    storage.addMetricBatch(req.get_param_value("name"), data);
    json success = {{"ok", "metrics addes"}};
    res.set_content(success.dump(), "application/json");
    res.status = 200;
  }
  void clientRun(const SharedStorage& storage, const httplib::Request& req, httplib::Response& res)
  {
    using json = nlohmann::json;
    bool flag_name = req.has_param("name");
    bool flag_time = req.has_param("time");
    bool flag_interval = req.has_param("begin") && req.has_param("end");
    if (!flag_name || !(flag_time ^ flag_interval))
    {
      json err = {{"error", "parameteres 'name' and 'time' or 'begin, end' are required"}};
      res.set_content(err.dump(2), "application/json");
      res.status = 400;
      return;
    }
    json data;
    json response;
    if (flag_time)
    {
      data = storage.getTimedMetric(req.get_param_value("name"), req.get_param_value("time"));
      response["time"] = req.get_param_value("time");
    }
    else
    {
      data = storage.getIntervalBatch(req.get_param_value("name"), req.get_param_value("begin"), req.get_param_value("end"));
      response["begin"] = req.get_param_value("end");
    }
    response["name"] = req.get_param_value("name");
    response["time_units"] = "s";
    response["metrics"] = data;
    res.set_content(response.dump(2), "application/json");
    res.status = 200;
  }
}

int main(int argc, char** argv)
{
  namespace fs = std::filesystem;
  using json = nlohmann::json;
  using ordered_json = nlohmann::ordered_json;
  using namespace std::placeholders;

  if (argc != 2) {
    std::cerr << "<Error>: incorrect args count, need config path\n";
    return 1;
  }
  models::SharedStorageConfig config = parseConfig(argv[1]);
  SharedStorage storage(config);

  httplib::Server server;
  server.Post(config.daemon_endpoint, httplib::Server::Handler(std::bind(daemonRun, std::ref(storage), _1, _2)));
  server.Get(config.client_endpoint, httplib::Server::Handler(std::bind(clientRun, std::cref(storage), _1, _2)));
  server.listen("0.0.0.0", config.port);
  std::cout << "Server is listening port " << config.port << '\n';
}
