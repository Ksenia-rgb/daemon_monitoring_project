#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "UI.hpp"

class Client
{
public:
  Client(UI * ui_to_use):
    ui_(ui_to_use)
  {}
  void useUI() const
  {
    ui_->executeUI();
  }
  
protected:
  UI * ui_;
};

#endif
