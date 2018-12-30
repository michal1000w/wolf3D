#ifndef Engine_H
#define Engine_H

#include <ncurses.h>

#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <math.h>

#include <iostream>
using namespace std;

//wymiary ekranu
const int nScreenWidth = 140;
const int nScreenHeight = 45;


class Engine{
  char buffer[nScreenHeight][nScreenWidth];
  //wchar_t buffer[nScreenHeight][nScreenWidth];
  char colorMap[nScreenHeight][nScreenWidth];
  wstring map;
  vector <string> wallTexture;
  //Gracz
  struct player {
    float X;
    float Y;
    float Angle;
    float FOV;
    float Speed;
  };
  player Player;
  float MaxRenderDistance;
  int wallRodzaj;
  //wymiary mapy
  int MapWidth;
  int MapHeight;
public:
  void Init();
  void InitMap();
  void MainLoop();
private:
  void FillEmpty();
  void DrawInfo(float);
  void DrawMap();
  void DrawBoard();
  char PlayerMove(float);
  void RayCasting(float &,float &,bool &,bool &,float &,float &, int&,int&);
  void Shading(float, int, int, bool, int, float);
};

#endif
