#include <fstream>
#include <iostream>
#include <thread>
#include "daemon.hpp"

namespace
{
  models::DaemonConfig parseConfig(const std::string& path)
  {
    std::ifstream fin(path);
    if (!fin.is_open()) {
      throw std::logic_error("<Error>: not found config file");
    }
    nlohmann::json json_config = nlohmann::json::parse(fin);
    std::cout << json_config.dump(2) << '\n';
    return json_config.get< models::DaemonConfig >();
  }
}
int main(int argc, char** argv)
{
  /*
  1 thread: собирает метрики, записывает в send_buffer
  2 thread: забирает из send_buffer, отправляет, кладет в repeat_buffer
  3 thread: забирает из repeat_buffer, отправляет повторно
  4 thread: забирает из emergency_buffer, отправляет с минимальным интервалом
            если метрики превышают порог, thread 1 закидывает их в другой буфер
  */

  if (argc != 2) {
    std::cerr << "<Error>: incorrect args count, need config path\n";
    return 1;
  }
  models::DaemonConfig config = parseConfig(argv[1]);
  Daemon daemon{config};
  daemon.start();

  std::thread collect_thread(&Daemon::runCollect, &daemon);
  std::thread send_thread(&Daemon::runSend, &daemon);
  std::thread repeat_thread(&Daemon::runRepeat, &daemon);

  std::string stop;
  std::cin >> stop;
  daemon.stop();

  if (collect_thread.joinable())
  {
    collect_thread.join();
    std::cout << "collect join\n";
  }
  if (send_thread.joinable())
  {
    send_thread.join();
    std::cout << "send join\n";
  }
  if (repeat_thread.joinable())
  {
    repeat_thread.join();
    std::cout << "repeat join\n";
  }
  daemon.remainingCases();
}
