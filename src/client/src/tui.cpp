#include <tui.hpp>

TUI::TUI()
{
  notcurses_options ncopts{};
  nc_ = notcurses_init(&ncopts, nullptr);
  if (!nc_)
  {
    throw std::runtime_error("notcurses_init failed");
  }
  stdplane_ = notcurses_stddim_yx(nc_, &dy_, &dx_);
  notcurses_mice_enable(nc_, NCMICE_BUTTON_EVENT);
}

TUI::~TUI()
{
  if (nc_)
  {
    notcurses_stop(nc_);
  }
}

int run_menu(struct notcurses* nc, struct ncmenu* ncm){
  ncplane_options nopts = {
    .y = 10,
    .x = NCALIGN_CENTER,
    .rows = 3,
    .cols = 40,
    .flags = NCPLANE_OPTION_HORALIGNED,
  };
  ncplane* selplane = ncplane_create(notcurses_stdplane(nc), &nopts);
  if (!selplane) return -1;

  ncplane_set_fg_rgb(selplane, 0x0);
  ncplane_set_bg_rgb(selplane, 0xdddddd);
  uint64_t channels = 0;
  ncchannels_set_fg_rgb(&channels, 0x000088);
  ncchannels_set_bg_rgb(&channels, 0x88aa00);
  if (ncplane_set_base(selplane, " ", 0, channels) < 0) {
    ncplane_destroy(selplane);
    return -1;
  }

  uint32_t keypress;
  ncinput ni;
  notcurses_render(nc);
  while ((keypress = notcurses_get_blocking(nc, &ni)) != (uint32_t)-1) {
    if (!ncmenu_offer_input(ncm, &ni)) {
      const char* sel;
      if ((sel = ncmenu_mouse_selected(ncm, &ni, nullptr))) {
        if (sel && !strcmp(sel, "Quit")) {
          ncmenu_destroy(ncm);
          ncplane_destroy(selplane);
          return 0;
        }
      } else if (ni.evtype == NCTYPE_RELEASE) {
        continue;
      } else if (keypress == 'q') {
        ncmenu_destroy(ncm);
        ncplane_destroy(selplane);
        return 0;
      } else if (keypress == NCKEY_ENTER) {
        if ((sel = ncmenu_selected(ncm, &ni))) {
          if (strcmp(sel, "Quit") == 0) {
            ncmenu_destroy(ncm);
            ncplane_destroy(selplane);
            return 0;
          }
        }
      }
    }
    ncplane_erase(selplane);
    ncinput selni;
    const char* selitem = ncmenu_selected(ncm, &selni);
    ncplane_putstr_aligned(selplane, 1, NCALIGN_CENTER, selitem ? selitem : "");
    notcurses_render(nc);
  }
  ncmenu_destroy(ncm);
  ncplane_destroy(selplane);
  return 0;
}

void TUI::run(const MenuItems & items) const
{
  struct ncmenu_item demo_items[] = {
    { .desc = "Restart", .shortcut = { .id = 'r', .modifiers = NCKEY_MOD_CTRL, }, },
    { .desc = "Derp", .shortcut = { .id = 'd', }, },
  };
  struct ncmenu_item file_items[] = {
    { .desc = "New", .shortcut = { .id = 'n', .modifiers = NCKEY_MOD_CTRL, }, },
    { .desc = "Open", .shortcut = { .id = 'o', .modifiers = NCKEY_MOD_CTRL, }, },
    { .desc = "Close", .shortcut = { .id = 'c', .modifiers = NCKEY_MOD_CTRL, }, },
    { .desc = NULL, },
    { .desc = "Quit", .shortcut = { .id = 'q', }, },
  };
  struct ncmenu_item help_items[] = {
    { .desc = "About", .shortcut = { .id = 'a', .modifiers = NCKEY_MOD_CTRL, }, },
  };

  struct ncmenu_section sections[] = {
    { .name = "Idiot",
      .itemcount = sizeof(demo_items) / sizeof(*demo_items),
      .items = demo_items,
      .shortcut = { .id = 0x00e4, .modifiers = NCKEY_MOD_ALT, }, },
    { .name = "File",
      .itemcount = sizeof(file_items) / sizeof(*file_items),
      .items = file_items,
      .shortcut = { .id = 'f', .modifiers = NCKEY_MOD_ALT, }, },
    { .name = NULL, .itemcount = 0, .items = NULL, },
    { .name = "Help",
      .itemcount = sizeof(help_items) / sizeof(*help_items),
      .items = help_items,
      .shortcut = { .id = 'h', .modifiers = NCKEY_MOD_ALT, }, },
  };

  ncmenu_options mopts;
  memset(&mopts, 0, sizeof(mopts));
  mopts.sections = sections;
  mopts.sectioncount = sizeof(sections) / sizeof(*sections);
  ncchannels_set_fg_rgb(&mopts.headerchannels, 0x00ff00);
  ncchannels_set_bg_rgb(&mopts.headerchannels, 0x440000);
  ncchannels_set_fg_rgb(&mopts.sectionchannels, 0xb0d700);
  ncchannels_set_bg_rgb(&mopts.sectionchannels, 0x002000);
  unsigned dimy, dimx;

  ncmenu * top = ncmenu_create(stdplane_, &mopts);

  if (top == NULL)
  {
    throw std::runtime_error("ncmenu_create failed");
  }
  if (ncmenu_item_set_status(top, "Idiot", "Restart", false))
  {
    throw std::runtime_error("ncmenu_item_set_status failed");
  }
  if (ncmenu_item_set_status(top, "File", "Open", false))
  {
    throw std::runtime_error("ncmenu_item_set_status failed");
  }
  uint64_t channels = 0;
  ncchannels_set_fg_rgb(&channels, 0x88aa00);
  ncchannels_set_bg_rgb(&channels, 0x000088);
  if (ncplane_set_base(stdplane_, "x", 0, channels) < 0)
  {
    throw std::runtime_error("ncmenu_item_set_status failed");
  }

  ncplane_set_fg_rgb(stdplane_, 0x00dddd);
  if (ncplane_putstr_aligned(stdplane_, dimy - 1, NCALIGN_RIGHT, " -=+ menu poc. press q to exit +=- ") < 0)
  {
    throw std::runtime_error("ncplane_putstr_aligned failed");
  }

  notcurses_render(nc_);

  run_menu(nc_, top);
}
