#include "cmd-ui.hpp"
#include <iostream>

void CMDUI::registerCommand(const std::string & name, command_handler handler)
{
  commands_[name] = handler;
}

void CMDUI::run()
{
  std::cout << "== commands ==\n";
  for (auto item : commands_)
  {
    std::cout << item.first << '\n';
  }

  std::cout << "> ";
  for (std::string command; !(std::cin >> command).eof();)
  {
    try
    {
      commands_.at(command)();
    }
    catch (const std::out_of_range &)
    {
      std::cout << "error: invalid command\n";
      if (std::cin.fail())
      {
        std::cin.clear(std::cin.rdstate() ^ std::ios::failbit);
      }
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    catch (const std::exception & e)
    {
      std::cout << "error: " << e.what() << '\n';
      if (std::cin.fail())
      {
        std::cin.clear(std::cin.rdstate() ^ std::ios::failbit);
      }
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
    std::cout << "> ";
  }
}
