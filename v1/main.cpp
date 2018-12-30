#include "Engine.h"


int main(){
  Engine engine;

  engine.Init();
  engine.InitMap();
  engine.MainLoop();

  endwin();
  return 0;
}
