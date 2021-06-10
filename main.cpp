#include <vector>
#include "config.hpp"
#include "menu.hpp"
#include "langMenu.hpp"
#include "timeMenu.hpp"
#include "findXBE.h"
#include "font.h"
#include "outputLine.h"
#include "renderer.h"
#include "subsystems.h"

#include "ftpServer.h"

#include <memory>
#include <type_traits>
#include <thread>
#include <SDL.h>

#ifdef NXDK
#include "eeprom.hpp"
#include <hal/xbox.h>
#include <hal/video.h>
#include <windows.h>
#endif

#ifdef NXDK
#define SEPARATOR "\\"
#define HOME "A:" SEPARATOR

typedef struct launchdata1 {
  uint32_t reason;
  uint32_t context;
  uint32_t parameters[2];
  uint8_t padding[3056];
} launchdata1;
#else
#define SEPARATOR "/"
#define HOME "." SEPARATOR
#endif

int main(void) {
#ifdef NXDK
  mountHomeDir('A');
  if (LaunchDataPage == NULL)
    LaunchDataPage = (PLAUNCH_DATA_PAGE)MmAllocateContiguousMemory(0x1000);
#endif
  Config config;
  std::map<int, SDL_GameController*> controllers;

  int init = init_systems();
  ftpServer *s = nullptr;

  std::thread thrF;

  if (init <= 1) {
    bool running = true;

    // Open our GameController
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        controllers[i] = SDL_GameControllerOpen(i);
        if (!controllers[i]) {
          outputLine("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
          SDL_Delay(2000);
        }
      }
    }

    // Set a hint that we want to use our gamecontroller always
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (init == 0 && config.settings.ftp.getEnabled()) {
      s = new ftpServer(&config.settings.ftp);
      s->init();
      thrF = std::thread(thread_runner, s);
    }

    // Create render system
    Renderer r;
    r.init(HOME);

    // Load font
    // FIXME: Font path should be read from theme
    Font f(r, HOME "vegur.ttf");

    Menu menu(config, r);
    std::shared_ptr<MenuNode> lang = nullptr;
    std::shared_ptr<MenuNode> timeZone = nullptr;

    r.drawBackground();
    r.flip();

    SDL_Event event;

#ifdef NXDK
    if (LaunchDataPage->Header.dwLaunchDataType == 1) {
      launchdata1* ld1 = (launchdata1*)(LaunchDataPage->LaunchData);
      if (ld1->reason == 3)
      {
        if (ld1->parameters[0] & 0x2) {
          
#endif
          timeZone = std::make_shared<TimeMenu>(menu.getCurrentMenu(), "Timezone select");
          menu.setCurrentMenu(timeZone.get());
#ifdef NXDK
        }
        if (ld1->parameters[0] & 0x4) {
#endif
          lang = std::make_shared<LangMenu>(menu.getCurrentMenu(), "Language select");
          menu.setCurrentMenu(lang.get());
#ifdef NXDK
        }
      }
    }
    else if (LaunchDataPage->Header.dwLaunchDataType == 0x0FFF0FFF) {
      launchdata1* ld1 = (launchdata1*)(LaunchDataPage->LaunchData);
      menu.getCurrentMenu()->addNode(std::make_shared<MenuLaunch>("rsn " + std::to_string(ld1->reason), "asd"));
      menu.getCurrentMenu()->addNode(std::make_shared<MenuLaunch>("ctx " + std::to_string(ld1->context), "asd"));
      menu.getCurrentMenu()->addNode(std::make_shared<MenuLaunch>("pr1 " + std::to_string(ld1->parameters[0]), "asd"));
      menu.getCurrentMenu()->addNode(std::make_shared<MenuLaunch>("pr2 " + std::to_string(ld1->parameters[1]), "asd"));
    }      
    else {
      menu.getCurrentMenu()->addNode(std::make_shared<MenuLaunch>(std::to_string(LaunchDataPage->Header.dwLaunchDataType), "asd"));
    }
#endif

    while (running) {
      r.setDrawColor(0, 89, 0);
      r.clear();
      r.drawBackground();

      menu.render(f);
      r.flip();
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
          break;
        } else if (event.type == SDL_CONTROLLERDEVICEADDED) {
          controllers[event.cdevice.which] = SDL_GameControllerOpen(event.cdevice.which);
        } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
          SDL_GameControllerClose(controllers[event.cdevice.which]);
          controllers.erase(event.cdevice.which);
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
          if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
            menu.up();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
            menu.down();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            menu.execute();
          } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_B ||
                     event.cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
            menu.back();
          }
        }
      }
#ifdef NXDK
      // Let's not hog CPU for nothing.
      SwitchToThread();
#endif
    }
  }
  for (auto c: controllers) {
    SDL_GameControllerClose(c.second);
  }
  delete s;
  shutdown_systems(init);
  return init;
}
