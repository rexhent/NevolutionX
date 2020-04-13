#include "outputLine.h"

#include "ftpServer.h"
#ifdef NXDK
#include "networking.h"
#include <hal/video.h>
#include <nxdk/mount.h>

extern "C" {
  extern uint8_t* _fb;
}
#endif

int main(void) {

#ifdef NXDK
  XVideoSetMode(640, 480, 32, 60);
  size_t fb_size = 640 * 480 * 4;
  _fb = (uint8_t*)MmAllocateContiguousMemoryEx(fb_size, 0, 0xFFFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memset(_fb, 0x00, fb_size);
#define _PCRTC_START				0xFD600800
  *(unsigned int*)(_PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;

  if (!nxMountDrive('C', "\\Device\\Harddisk0\\Partition2")) {
    outputLine("Mounting error: Could not mount drive C\n");
  }
  if (!nxMountDrive('E', "\\Device\\Harddisk0\\Partition1")) {
    outputLine("Mounting error: Could not mount drive E\n");
  }
  if (!nxMountDrive('F', "\\Device\\Harddisk0\\Partition6")) {
    outputLine("Mounting error: Could not mount drive F\n");
  }
  if (!nxMountDrive('G', "\\Device\\Harddisk0\\Partition7")) {
    outputLine("Mounting error: Could not mount drive G\n");
  }
  if (!nxMountDrive('X', "\\Device\\Harddisk0\\Partition3")) {
    outputLine("Mounting error: Could not mount drive X\n");
  }
  if (!nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4")) {
    outputLine("Mounting error: Could not mount drive Y\n");
  }
  if (!nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5")) {
    outputLine("Mounting error: Could not mount drive Z\n");
  }

  bool use_dhcp = true;
  setupNetwork(&use_dhcp);
#endif

  // Start FTP server
  ftpServer *s;
#ifdef NXDK
  s = new ftpServer(21);
#else
  s = new ftpServer(2121);
#endif
  s->init();
  s->run();
  delete s;
#ifdef NXDK
  closeNetwork();
#endif
  return 0;
}
