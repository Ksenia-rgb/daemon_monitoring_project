#ifndef CMD_UI_HPP
#define CMD_UI_HPP

#include <string>
#include "client.hpp"

class CMDUI final: public UI
{
public:
  void registerCommand(const std::string & name, command_handler handler) override;
  void run() override;

private:
  std::unordered_map< std::string, command_handler > commands_;
};

#endif
