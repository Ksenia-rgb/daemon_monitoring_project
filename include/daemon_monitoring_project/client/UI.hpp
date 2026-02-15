#ifndef UI_HPP
#define UI_HPP

class UI
{
public:
  virtual ~UI() = default;
  virtual void executeUI() const = 0;
};

#endif