#include "Engine.h"

void Engine::Init(){
  setlocale(LC_ALL, "UTF-8");

  initscr();
  noecho();
  keypad(stdscr, true);
  //HideCursor(true);

  Player.X = 14.7f;
  Player.Y = 5.09f;
  Player.Angle = 0.0f;
  Player.FOV = 3.1415 / 4.0f;
  Player.Speed = 45.0f;

  FillEmpty();

  MaxRenderDistance = 16.0f;
  MapWidth = 16;
  MapHeight = 16;
}

void Engine::InitMap(){
  map += L"################";
  map += L"#..............#";
  map += L"#..............#";
  map += L"########.......#";
  map += L"#..............#";
  map += L"#........#.....#";
  map += L"#........#.....#";
  map += L"#..............#";
  map += L"#..............#";
  map += L"#........###..##";
  map += L"#........#.....#";
  map += L"#........#.....#";
  map += L"#..............#";
  map += L"#........#######";
  map += L"#..............#";
  map += L"################";

  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("WWWWWWWWWWWWWWWW");
  wallTexture.push_back("WRRRRRWRRRRRWRRR");
  wallTexture.push_back("WRRRRRWRRRRRWRRR");
  wallTexture.push_back("WWWWWWWWWWWWWWWW");
  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("WWWWWWWWWWWWWWWW");
  wallTexture.push_back("WRRRRRWRRRRRWRRR");
  wallTexture.push_back("WRRRRRWRRRRRWRRR");
  wallTexture.push_back("WWWWWWWWWWWWWWWW");
  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("RRRWRRRRRWRRRRRW");
  wallTexture.push_back("WWWWWWWWWWWWWWWW");
  wallTexture.push_back("WRRRRRWRRRRRWRRR");
}

void Engine::MainLoop(){
  //ustawianie zegara
  auto tp1 = chrono::system_clock::now();
  auto tp2 = chrono::system_clock::now();

  while(true){
    //Ustawianie stałych odstępów mimo różnej ilości FPS
    tp2 = chrono::system_clock::now();
    chrono::duration<float> elapsedTime = tp2 - tp1;
    tp1 = tp2;
    float ElapsedTime = elapsedTime.count();

    PlayerMove(ElapsedTime);

    for (int x = 0; x < nScreenWidth; x++){
      float RayAngle = (Player.Angle - Player.FOV / 2.0f) + ((float)x / (float)nScreenWidth) * Player.FOV;

      float StepSize = 0.1f;
      float DistanceToWall = 0.0f;

      bool HitWall = false;
      bool Boundary = false;

      float EyeX = sinf(RayAngle);
      float EyeY = cosf(RayAngle);

      int TestX, TestY;
      RayCasting(StepSize,DistanceToWall,HitWall,Boundary,EyeX,EyeY, TestX, TestY);

      //Obliczanie dystansu między sufitem a podłogą
      int Ceiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)DistanceToWall);
      int Floor = nScreenHeight - Ceiling;

      //obliczenia do tekstury
      float BlockMidX = (float)TestX + 0.5f;
      float BlockMidY = (float)TestY + 0.5f;
      float fTestPointX = Player.X + EyeX * DistanceToWall;
      float fTestPointY = Player.Y + EyeY * DistanceToWall;
      float SampleX = 0.0f;

      float TestAngle = atan2((fTestPointY - BlockMidY), (fTestPointX - BlockMidX));
        if (TestAngle >= -3.1415f * 0.25f && TestAngle < 3.1415f * 0.25f)
          SampleX = fTestPointY - (float)TestY;
        if (TestAngle >= 3.1415f * 0.25f && TestAngle < 3.1415f * 0.75f)
          SampleX = fTestPointX - (float)TestX;
        if (TestAngle < -3.1415f * 0.25f && TestAngle >= -3.1415f * 0.75f)
          SampleX = fTestPointX - (float)TestX;
        if (TestAngle >= 3.1415f * 0.75f || TestAngle < -3.1415f * 0.75f)
          SampleX = fTestPointY - (float)TestY;

      Shading(DistanceToWall, Ceiling, Floor, Boundary, x, SampleX);
    }

    DrawMap();

    DrawBoard();
    DrawInfo(ElapsedTime);

    refresh();
  }
}

void Engine::Shading(float DistanceToWall, int Ceiling, int Floor, bool Boundary, int x, float SampleX){
  //Shader walls based on Distance
  short nShade = ' ';
  if (DistanceToWall <= MaxRenderDistance / 4.0f)     nShade = '#';
  else if (DistanceToWall < MaxRenderDistance / 3.0f) nShade = 'O';
  else if (DistanceToWall < MaxRenderDistance / 2.0f) nShade = 'i';
  else if (DistanceToWall < MaxRenderDistance / 1.5f) nShade = ';';
  else if (DistanceToWall < MaxRenderDistance)        nShade = '.';
  else                                                nShade = ' ';

  if (Boundary) nShade = ' ';

  for (int y = 0; y < nScreenHeight; y++){
    if (y <= Ceiling){
      buffer[y][x] = ' ';
      colorMap[y][x] = 'W';
    }
    else if (y > Ceiling && y <= Floor)
    {
      if (DistanceToWall < MaxRenderDistance)
        buffer[y][x] = nShade;

      //tesktura
      float tX, tY;
      tX = SampleX;
      tY = ((float)y - (float)Ceiling) / ((float)Floor - (float)Ceiling);

      int textureSize = wallTexture.size();

      int sx = tX * (float)textureSize;
      int sy = tY * (float)textureSize-1;
      if (sx < 0 || sx > textureSize || sy < 0 || sy > textureSize)
        colorMap[y][x] = ' ';
      else
        colorMap[y][x] = wallTexture[sy][sx];

    }
    else {
      float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
      if (b < 0.25)      nShade = '&';
      else if (b < 0.5)  nShade = 'x';
      else if (b < 0.75) nShade = ',';
      else if (b < 0.9)  nShade = '-';
      else               nShade = ' ';
      buffer[y][x] = nShade;
      colorMap[y][x] = 'B';
    }
  }
}

void Engine::DrawMap(){
  //////////////////////////Rysowanie mapy///////////////////
  for (int y = 0; y < MapHeight; y++)
    for (int x = 0; x < MapWidth; x++)
      buffer[y][x] = map[y * MapWidth + x];
  /////////////////////////Rysowanie gracza na mapie/////////
  buffer[(int)Player.X][(int)Player.Y] = 'P';
}

void Engine::DrawInfo(float ElapsedTime){
  //print stats
  attron(COLOR_PAIR(3));
  printw("FPS: %3.2f   X: %3.2f, Y: %3.2f                       ", 1.0f / ElapsedTime, Player.Y, Player.X);
  attroff(COLOR_PAIRS);
}

void Engine::DrawBoard(){
  //wypisywanie na ekran
  start_color();
  use_default_colors();
  init_pair(1, COLOR_WHITE, -1);
  init_pair(2, COLOR_BLUE, -1);
  init_pair(3, COLOR_RED, -1);
  move(0,0);
  for (int y = 0; y < nScreenHeight; y++){
    for (int x = 0; x < nScreenWidth; x++){
      if (colorMap[y][x] == 'B')
        attron(COLOR_PAIR(2));
      else if (buffer[y][x] == 'P' || colorMap[y][x] == 'R')
        attron(COLOR_PAIR(3));
      else
        attroff(COLOR_PAIRS);
      printw("%c", buffer[y][x]);
    }
    printw("\n");
  }
}

void Engine::RayCasting(float &StepSize,float &DistanceToWall,bool &HitWall,bool &Boundary,float &EyeX,float &EyeY, int &TestX, int &TestY){
  while (!HitWall && DistanceToWall < MaxRenderDistance){
    DistanceToWall += StepSize;
    TestX = (int)(Player.X + EyeX * DistanceToWall);
    TestY = (int)(Player.Y + EyeY * DistanceToWall);

    if (TestX < 0 || TestX >= MapWidth || TestY < 0 || TestY >= MapHeight){
      HitWall = true;
      DistanceToWall = MaxRenderDistance;
    } else {
      if (map.c_str()[TestX * MapWidth + TestY] == '#'){
        //Ray hits HitWall
        HitWall = true;

        vector<pair<float, float>> p;

        for (int tx = 0; tx < 2; tx++)
          for (int ty = 0; ty < 2; ty++){
            float vy = (float)TestY + ty - Player.Y;
            float vx = (float)TestX + tx - Player.X;
            float d = sqrt(vx*vx + vy*vy);
            float dot = (EyeX * vx / d) + (EyeY * vy / d);
            p.push_back(make_pair(d, dot));
          }

          //Sort Pairs
          sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first;});

          float Bound = 0.01;
          if (acos(p.at(0).second) < Bound) Boundary = true;
          if (acos(p.at(1).second) < Bound) Boundary = true;
          if (acos(p.at(2).second) < Bound) Boundary = true;
      }
    }
  }
}

char Engine::PlayerMove(float ElapsedTime){
  //Pobranie klawisza
  nodelay(stdscr, true); //non-block input from getch()
  char keyPressed = getch();
  nodelay(stdscr, false);
  //Rotacja kamery
  if (keyPressed == 'a')
    Player.Angle -= (Player.Speed * 0.75f) * ElapsedTime;
  else if (keyPressed == 'd')
    Player.Angle += (Player.Speed * 0.75f) * ElapsedTime;

  //Ruch gracza i sprawdzanie kolizji
  if (keyPressed == 'w'){
    Player.X += sinf(Player.Angle) * Player.Speed * ElapsedTime;;
    Player.Y += cosf(Player.Angle) * Player.Speed * ElapsedTime;;
    //sprawdzanie kolizji
    if (map.c_str()[(int)Player.X * MapWidth + (int)Player.Y] == '#'){
      Player.X -= sinf(Player.Angle) * Player.Speed * ElapsedTime;;
      Player.Y -= cosf(Player.Angle) * Player.Speed * ElapsedTime;;
    }
  } else if (keyPressed == 's'){
    Player.X -= sinf(Player.Angle) * Player.Speed * ElapsedTime;;
    Player.Y -= cosf(Player.Angle) * Player.Speed * ElapsedTime;;
    //sprawdzanie kolizji
    if (map.c_str()[(int)Player.X * MapWidth + (int)Player.Y] == '#'){
      Player.X += sinf(Player.Angle) * Player.Speed * ElapsedTime;;
      Player.Y += cosf(Player.Angle) * Player.Speed * ElapsedTime;;
    }
  }

  //koniec gry, gdy q
  if (keyPressed == 'q'){
    endwin();
    exit(1);
  }

  return keyPressed;
}

void Engine::FillEmpty(){
  for (int y = 0; y < nScreenHeight; y++)
    for (int x = 0; x < nScreenWidth; x++){
      buffer[y][x] = ' ';
      colorMap[y][x] = ' ';
    }
}
