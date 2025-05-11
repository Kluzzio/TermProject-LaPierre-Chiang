#include <iostream>

#include "engine.h"


int main(int argc, char **argv)
{
  std::cout << "Creating engine...\n";
  Engine *engine = new Engine("Term Project - LaPierre/Chiang", 800, 600);
  
  std::cout << "Initializing engine...\n";
  if (!engine->Initialize()) {
      std::cout << "The engine failed to start.\n";
      delete engine;
      return 1;
  }
  
  std::cout << "Engine initialized successfully.\n";
  engine->Run();
  std::cout << "Engine finished running.\n";
  
}
