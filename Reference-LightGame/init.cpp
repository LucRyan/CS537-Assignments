#include <fstream>

using namespace std;


// This temp for testing
void processfile(){

  ifstream level_file ("level.txt");
  if (level_file.is_open())
  {

    // read in the floor dimensions (30x30 max)
    level_file >> world.x;
    level_file >> world.y;

    // fill in the wall data
    for(int i=0;i<world.x;i++)
      for(int j=0;j<world.y;j++)
	  level_file >> world.walls[i][j];

    int box_num;
    double x,y;
    level_file >> box_num;
    for(int i=0; i<box_num; i++)
      {
	level_file >> x >> y;
	world.addBox( box(x, y) );
      }


    // read in bin coordinates
    // should be as many bins as boxes
    for(int i=0; i<box_num; i++)
      {
	level_file >> x >> y;
	world.addBin( bin(x,y) );
      }
    
    world.syncBinsBoxes();

    level_file.close();
  }
  else cout << "Unable to open level.txt which stores the level information";
}
