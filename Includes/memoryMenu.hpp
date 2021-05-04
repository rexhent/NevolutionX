#ifndef __MEMORYMENU_HPP
#define __MEMORYMENU_HPP

#include "menu.hpp"
#include <string>

class gameSaveData : public MenuItem {
  size_t size;
  // xbx_texture icon;
public:
  gameSaveData(std::string const& path);
  const std::string_view getLabel();
}

class gameSave : public MenuNode {
  // xbx_texture icon;
public:
  gameSave(std::string const& path);
};

#endif
