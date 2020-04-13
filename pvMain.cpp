#include "outputLine.h"

#include "pvTester.h"
#ifdef NXDK
#include "networking.h"
#include <hal/video.h>

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

  bool use_dhcp = true;
  setupNetwork(&use_dhcp);
#endif

  pvTester(nullptr);

#ifdef NXDK
  closeNetwork();
#endif
  return 0;
}
