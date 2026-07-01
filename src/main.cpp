#include "app/app.h"
#include "utils/crash_handler.h"
#include <iostream>

int main() {
  Engine::installCrashHandler();
  try {
    Engine::Application app({.width = 1280, .height = 720, .title = "Necros"});
    app.run();
  } catch (const std::exception &e) {
    std::cerr << "[FATAL] " << e.what() << "\n";
    return 1;
  }
  return 0;
}
