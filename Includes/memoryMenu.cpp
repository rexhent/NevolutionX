#include "memoryMenu.hpp"

#include "outputLine.h"

#include <vector>
#include <fstream>
#include <windows.h>

static const std::string gametitlemetaname = "TitleMeta.xbx";
static const std::string gamesavemetaname = "SaveMeta.xbx";

static
std::string extractName(std::string const& path, std::string const& file, bool secondLine = false) {
  std::ifstream f(path + "\\" + file, std::ios::binary);
  if (!f.is_open()) {
    return "";
  }
  std::string buf;
  if (secondLine) {
    std::getline(f, buf);
  }
  std::getline(f, buf);
  return buf;  
}

static
std::vector<std::string> findSubFolders(std::string const& path) {
  std::vector<std::string> ret;
#ifdef NXDK
  std::string searchmask = path + "\\*";
  WIN32_FIND_DATAA fData;
  HANDLE fHandle = FindFirstFileA(searchmask.c_str(), &fData);
  if (fHandle == INVALID_HANDLE_VALUE) {
    ret.push_back("<Empty>");
    return ret;
  }

  do {
    if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      ret.push_back(path + "\\" + fData.cFileName);
    }
  } while (FindNextFile(fHandle, &fData) != 0);
  FindClose(fHandle);
#else
  for (int i = 0; i < 7; ++i) {
    ret.push_back("");
  }
#endif
  return ret;
}

/*******************************************************************************
 *                             gameSaveData
 ******************************************************************************/
gameSaveData::gameSaveData(std::string const& label, std::string const& path) :
  MenuItem(label), path(path) {
  size = 192;
}

// std::string_view gameSaveData::getLabel() const {
//   return label + " (" + std::to_string(size) + ")";
// }

void gameSaveData::execute(Menu *) {
  for (auto e: findSubFolders("E:\\UDATA")) {
    outputLine("%s \n", e.c_str());
  }

}

/*******************************************************************************
 *                               gameSave
 ******************************************************************************/
gameSave::gameSave(MenuNode *parent,
                   std::string const& label,
                   std::string const& path) :
  MenuNode(parent, label) {
  std::vector<std::string> subs = findSubFolders(path);
  for (std::string &e : subs) {
    std::string buf = extractName(e, gamesavemetaname);
    if (buf.length() == 0) {
      buf = "Corrupted Save";
    }
    addNode(std::make_shared<gameSaveData>(gameSaveData(buf, e)));
  }
}

/*******************************************************************************
 *                             gameSaveMenu
 ******************************************************************************/
gameSaveMenu::gameSaveMenu(MenuNode *parent, std::string const& label) :
  MenuNode(parent, label) {
  std::vector<std::string> games = findSubFolders("E:\\UDATA");
  for (std::string &game : games) {
    std::string name = extractName(game, gametitlemetaname);
    if (name.length() == 0) {
      name = "Unknown Game";
    }
    addNode(std::make_shared<gameSave>(gameSave(this, name, game)));
  }
}
