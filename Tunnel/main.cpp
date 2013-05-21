#include <irrlicht.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "HDMStereoRender/HMDStereoRender.h" 

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#define MAX_OBJECTS 50
//#define EYES_OFFSET 5

float EYES_OFFSET = 5.0;
float VIEWPORT_OFFSET = 5.0;

class EventHandler : public IEventReceiver
{
public:
   bool OnEvent(const SEvent& event)
   {   
      if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
        if (event.KeyInput.Key == KEY_ESCAPE)
        {
          device->closeDevice();
          return true;
        }
      }
      return false;
   }
   IrrlichtDevice* device;

}; 


const int ResX=1280;
const int ResY=800;
bool fullscreen = true;
bool vsync = true;

int main()
{
	// create device
    EventHandler receiver;

	ISceneNode* objects [MAX_OBJECTS];



	 IrrlichtDevice *device = createDevice(EDT_OPENGL, dimension2d<u32>(ResX, ResY), 16, fullscreen, false, vsync, &receiver); 
	 
	 receiver.device = device; 

    if (!device)
        return 1;

    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();

	HMDDescriptor HMD;
	// Parameters from the Oculus Rift DK1
	HMD.hResolution = ResX;
	HMD.vResolution = ResY;
	HMD.hScreenSize = 0.14976;
	HMD.vScreenSize = 0.0936;
	HMD.interpupillaryDistance = 0.064;
	HMD.lensSeparationDistance = 0.064;
	HMD.eyeToScreenDistance = 0.041;
	HMD.distortionK[0] = 1.0;
	HMD.distortionK[1] = 0.22;
	HMD.distortionK[2] = 0.24;
	HMD.distortionK[3] = 0.0;

	HMDStereoRender renderer(device, HMD, 10); 

	smgr->addCameraSceneNodeFPS();

	 device->getCursorControl()->setVisible(false); 


	// load the quake map
	device->getFileSystem()->addZipFileArchive("media/map-20kdm2.pk3");
	IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
	ISceneNode* levelNode = smgr->addOctreeSceneNode(mesh->getMesh(0), 0, -1, 128);
	levelNode->setPosition(core::vector3df(-1350,-90,-1400));
	  
	// load a faerie 
	IAnimatedMesh* faerie = smgr->getMesh("media/faerie.md2");
	IAnimatedMeshSceneNode* faerieNode = smgr->addAnimatedMeshSceneNode(faerie);
	faerieNode->setMaterialTexture(0, driver->getTexture("media/faerie2.bmp"));
	faerieNode->setMaterialFlag(EMF_LIGHTING, false);
	faerieNode->setPosition(vector3df(40,190,-1030));
	faerieNode->setRotation(vector3df(0,-90,0));
	faerieNode->setMD2Animation(EMAT_SALUTE);

	// load a dwarf
	IAnimatedMesh* dwarf = smgr->getMesh("media/dwarf.x");
	IAnimatedMeshSceneNode* dwarfNode = smgr->addAnimatedMeshSceneNode(dwarf);
	dwarfNode->setPosition(vector3df(40,-25,20));
	  

	/*  
    if (node)
    {
        node->setMaterialFlag(EMF_LIGHTING, false);
        node->setMD2Animation(scene::EMAT_STAND);
        node->setMaterialTexture( 0, driver->getTexture("media/sydney.bmp") );
    }
	*/


	vector3df pos = vector3df(0,0,0);

	//naplníme tunel pøekážkama

	 srand (time(NULL));
	/* generate secret number between 1 and 10: */
	
	for(int i = 0; i < MAX_OBJECTS; i++){
		objects[i] = smgr->addCubeSceneNode(2);
		objects[i]->setPosition( vector3df( (rand() % 30) - 5, (rand() % 30) - 5, rand() % 80) );
	}

	device->setInputReceivingSceneManager(smgr);
	
    while(device->run())
    {
        driver->beginScene(true, true, SColor(255,100,101,140));
		
		for(int i = 0; i < MAX_OBJECTS; i++){
			vector3df tmpPos = objects[i]->getPosition();
			if(tmpPos.Z > pos.Z) continue;
			
			objects[i]->setPosition( vector3df( (rand() % 30) - 15, (rand() % 30) - 15, rand() % 80 + pos.Z) );
		}
		
		renderer.drawAll(smgr); 
		//smgr->drawAll();
        
        guienv->drawAll();

        driver->endScene();
    }


    device->drop();

    return 0;
}