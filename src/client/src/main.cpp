#include <iostream>
#include <cstring>
#include <memory>

#include <client.hpp>
#include <cl-ui.hpp>

int main(int argc, char ** argv)
{
  std::unique_ptr< UI > current_ui;
  try
  {
    if (argc > 1)
    {
      if (std::strcmp(argv[1], "--cmd") == 0)
      {
        current_ui = std::make_unique< CLI >();
      }
      else
      {
        std::cerr << "error: unknown flag " << argv[1] << ". use --cmd\n";
        return 1;
      }
    }
    else
    {
      current_ui = std::make_unique< CLI >();
    }
    
    auto client = std::make_unique< Client >(std::move(current_ui));
    client->run();
  }
  catch (...)
  {
    std::cerr << "unexpected error\n";
    return 2;
  }
}
