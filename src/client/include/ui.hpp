#ifndef UI_HPP
#define UI_HPP

#include <vector>
#include <string>

struct MenuItem
{
  std::string name;
  std::function< bool() > action;
};

using MenuItems = std::vector< MenuItem >;

class UI
{
public:
  virtual ~UI() = default;
  virtual void run(const MenuItems & items) const = 0;
};

#endif
