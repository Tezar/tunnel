#include <irrlicht.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;


class Level 
{
public:
	Level(IrrlichtDevice* _device){
		device=_device;
	}

	void makeLevel(vector3df* pos);

	IMesh* Level::createMesh(f32 radius, f32 length, 
			u32 tesselation, const video::SColor& color, 
			bool closeTop, f32 oblique) const;
	
	IrrlichtDevice* device;

}; 
