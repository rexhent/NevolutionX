#include "outputLine.h"
#include <cstdarg>
#include <cstdio>

#ifdef NXDK
#include <hal/debug.h>
#include <windows.h>
#else
#include <SDL.h>
#endif

#include "infoLog.h"

void outputLine(const char* format, ...) {
  char buffer[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);

  InfoLog::outputLine(buffer);
  if (!InfoLog::isOutputCaptured()) {
#ifdef NXDK
    debugPrint("%s", buffer);
    OutputDebugStringA(buffer);
#else
    printf("%s", buffer);
#endif
  }

  va_end(args);
}
