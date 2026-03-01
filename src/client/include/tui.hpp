#ifndef TUI_HPP
#define TUI_HPP

#include <string>

#include <stdio.h>
#include <notcurses/notcurses.h>
/*
  in the end, i use the pure c version of notcurses,
  because the c++ wrappers are implemented so SHIT,
  I T S   J U S T   I  M  P  O  S  S  I  B  L  E
*/
#include "client.hpp"

class TUI: public UI
{
public:
  TUI();
  ~TUI() override;

  TUI(const TUI &) = delete;
  TUI & operator=(const TUI &) = delete;

  void run(const MenuItems & items) const override;
private:
  notcurses * nc_ = nullptr;
  ncplane * stdplane_ = nullptr;
  unsigned dx_, dy_;
};

#endif