#include "outputLine.h"
#include "subsystems.h"

#include "ftpServer.h"
#include "pvTester.h"

int main(void) {
  int init = init_systems();
  // Start FTP server
  ftpServer *s;
  if (init == 0) {
#ifdef NXDK
    s = new ftpServer(21);
#else
    s = new ftpServer(2121);
#endif
    s->init();
    
    thread_runner(s);
  }

  delete s;
  shutdown_systems(init);
  return init;
}
