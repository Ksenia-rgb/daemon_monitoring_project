#ifndef CL_UI_HPP
#define CL_UI_HPP

#include "client.hpp"

class CLI: public UI
{
public:
  ~CLI();
  void run(const MenuItems & items) const override;
};

#endif
