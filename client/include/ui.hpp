#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include <string>
#include "common-types.hpp"

class UI
{
public:
  using command_handler = std::function< void() >;

  virtual ~UI() = default;

  virtual void registerCommand(const std::string & name, command_handler handler) = 0;
  virtual void run() = 0;
};

#endif
