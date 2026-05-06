#include <filesystem>
#include <httplib.h>
#include <iostream>
#include <json.hpp>
#include <variant>

int main()
{
  namespace fs = std::filesystem;
  using json = nlohmann::json;
  using ordered_json = nlohmann::ordered_json;

  httplib::Server svr;

  fs::path metrics_dir = "./storage/metrics";
  svr.Get("/get_metric",
      [&metrics_dir](const httplib::Request & req, httplib::Response & res)
      {
        if (!req.has_param("name") || !req.has_param("time"))
        {
          json err = {{"error", "parameters 'name' and 'time' are required"}};
          res.status = 400;
          res.set_content(err.dump(2), "application/json");

          return;
        }

        std::string query_server_name = req.get_param_value("name");
        std::string query_metric_timestamp = req.get_param_value("time");

        fs::path dir_path = metrics_dir / query_server_name;
        for (const auto & entry : fs::directory_iterator(dir_path))
        {
          std::ifstream fin(entry.path().string());
          if (!fin)
          {
            json err = {{"error", "invalid metrics directory set for server"}};
            res.status = 500;
            res.set_content(err.dump(2), "application/json");

            return;
          }
          ordered_json jin = json::parse(fin);
          ordered_json jres;
          jres["version"] = jin["version"];
          jres["id"] = jin["id"];
          jres["name"] = jin["name"];
          jres["interval_value"] = jin["interval_value"];
          jres["interval_units"] = jin["interval_units"];
          for (auto & metric_item : jin["metrics"])
          {
            std::string timestamp = metric_item["time"];
            if (timestamp == query_metric_timestamp)
            {
              jres["time"] = metric_item["time"];
              jres["data"] = metric_item["data"];

              break;
            }
          }

          res.set_content(jres.dump(2), "application/json");

          return;
        }

        json err = {{"error", "no matching file found"}};
        res.status = 404;
        res.set_content(err.dump(2), "application/json");
      });

  std::cout << "Listening on port 8080..." << std::endl;
  svr.listen("0.0.0.0", 8080);
}
