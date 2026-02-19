#ifndef CLI_HPP
#define CLI_HPP

#include "client.hpp"

class CLI: public UI
{
public:
  void executeUI() const override;
};

#endif
