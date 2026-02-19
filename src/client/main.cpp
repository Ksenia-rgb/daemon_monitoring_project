#include <iostream>
#include <cstring>

#include "client.hpp"
#include "CLI.hpp"

int main(int argc, char ** argv)
{
  UI * current = nullptr;
  Client * cl = nullptr;

  try
  {
    if (argc > 1)
    {
      if (std::strcmp(argv[1], "--cmd") == 0)
      {
        current = new CLI;
      }
      else
      {
        std::cerr << "error: unknown flag " << argv[1] << ". use --cmd\n";
        return 1;
      }
    }
    else
    {
      current = new CLI; //default
    }
    
    Client * cl = new Client(current);

    cl->useUI();

    delete cl;
    delete current;
  }
  catch (...)
  {
    std::cerr << "unexpected error\n";

    delete cl;
    delete current;

    return 2;
  }
}