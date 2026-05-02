#include <iostream>
#include <cstring>
#include <memory>

#include "client.hpp"
#include "cmd-ui.hpp"
#include "managers.hpp"

int main(int argc, char ** argv)
{
  std::unique_ptr< UI > current_ui;
  try
  {
    if (argc > 1)
    {
      if (std::strcmp(argv[1], "--cmd") == 0)
      {
        current_ui = std::make_unique< CMDUI >();
      }
      else
      {
        std::cerr << "error: unknown flag " << argv[1] << ". use --cmd, --tui\n";
        return 1;
      }
    }
    else
    {
      current_ui = std::make_unique< CMDUI >();
    }

    auto client = std::make_unique< Client >(std::move(current_ui), "client/config/init.json");

    client->run();
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what();
    return 2;
  }
}
