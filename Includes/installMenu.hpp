#ifndef __INSTALL_MENU
#define __INSTALL_MENU

#include "menu.hpp"

#include <string>

class installItem : public MenuItem {
  bool bigDrive = false;
public:
  installItem(std::string const& label, bool bigDrive);
  void execute(Menu *);
};

class installMenu : public MenuNode {
public:
  installMenu(MenuNode *parent, std::string const& label);
};

#endif
