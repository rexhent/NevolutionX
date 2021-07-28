#include "installMenu.hpp"

#ifdef NXDK
#include <cstdio>
#include <utility>

#include <nxdk/format.h>
#include <winapi/fileapi.h>
#endif

static const std::string fPartition = "\\Device\\Harddisk0\\Partition6";
static const std::string gPartition = "\\Device\\Harddisk0\\Partition7";

installItem::installItem(std::string const& label, bool bigDrive) :
  MenuItem(label), bigDrive(bigDrive) {
}

void installItem::execute(Menu *) {
  std::array<std::string, 2> partitions = {
    "\\Device\\Harddisk0\\Partition2", // "C"
    "\\Device\\Harddisk0\\Partition1" // "E"
  };
  for (auto s: partitions) {
    nxFormatVolume(s.c_str(), 0);
  }
  if (!bigDrive) {
    nxFormatVolume(fPartition.c_str(), 0);
  }
  else {
    nxFormatVolume(fPartition.c_str(), 65536);
    nxFormatVolume(gPartition.c_str(), 65536);
  }
  CreateDirectory("E:\\UDATA", NULL);
  CreateDirectory("E:\\TDATA", NULL);

  std::array<std::pair<std::string, std::string>, 5> files = {
    std::make_pair("A:\\default.xbe", "C:\\xboxdash.xbe"),
    std::make_pair("A:\\720.png", "C:\\720.png"),
    std::make_pair("A:\\480.png", "C:\\480.png"),
    std::make_pair("A:\\config.json", "C:\\config.json"),
    std::make_pair("A:\\vegur.ttf", "C:\\vegur.ttf")};

  for (auto fname: files) {
    std::FILE *in = std::fopen(fname.first.c_str(), "rb");
    if (in) {
      std::FILE *out = std::fopen(fname.second.c_str(), "wb");
      if (out) {
        size_t cnt;
        char buffer[1024] = {0};
        while ((cnt = fread(buffer, sizeof(char), sizeof buffer, in))) {
          fwrite(buffer, sizeof(char), cnt, out);
        }
        fclose(out);
      }
    fclose(in);
    }
  }
}

/*******************************************************************************
 *                          InstallMenu
 *******************************************************************************/

installMenu::installMenu(MenuNode* parent, std::string const& label) :
  MenuNode(parent, label) {
  addNode(std::make_shared<MenuExec>("Warning: Installing wipes your harddrive!", [](Menu *){ return; }));
  addNode(std::make_shared<installItem>("Install (Small Drive)", false));
  addNode(std::make_shared<installItem>("Install (Big Drive)", true));
}
