#ifndef UI_HPP
#define UI_HPP

#include <unordered_map>
#include <string>
/*
struct MenuItem
{
  std::string name;
  std::function< bool() > action;
};
*/
using MenuItems = std::unordered_map< const char *, std::function< bool(void *) > >;

class UI
{
public:
  virtual ~UI() = default;
  virtual void run(const MenuItems & items) const = 0;
};

#endif
