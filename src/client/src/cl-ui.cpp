#include <cl-ui.hpp>

#include <string>

#include <ncurses.h>

CLI::~CLI()
{
  endwin();
}

void CLI::run(const MenuItems & items) const
{
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();

  printw("Welcome to monitoring daemon!\n");
  printw("Select options:\n");

  size_t row = 2;
  size_t number_of_options = items.size();

  for (auto it = items.begin(); it != items.end(); ++it)
  {
    printw("[ ] ");
    printw("%s", it->first);
    printw("\n");
  }

  size_t select = 0;

  attron(A_BOLD);
  mvprintw(row + select, 1, "0");
  move(number_of_options + 2, 0);

  for (int ch = getch(); ch != 'q'; ch = getch())
  {
    mvprintw(row + select, 1, " ");
    if (ch == KEY_UP)
    {
      select != 0 ? --select : select;
    }
    else if (ch == KEY_DOWN)
    {
      select != number_of_options - 1 ? ++select : select;
    }
    mvprintw(row + select, 1, "0");
    move(number_of_options + 2, 0);
  }

  attroff(A_BOLD);

  refresh();
}
