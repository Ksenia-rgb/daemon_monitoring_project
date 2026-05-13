#include <cstring>
#include <iostream>
#include "client.hpp"
#include "cmd-ui.hpp"

int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cerr << "Error: incorrect args count, need config path\n";
    return 1;
  }

  try
  {
    std::unique_ptr< UI > ui = std::make_unique< CMDUI >();
    ClientConfig config;
    config.load(argv[1]);
    Client client(config, std::move(ui));

    client.run();
  }
  catch (const std::exception & e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 2;
  }
}
