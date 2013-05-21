#include <irrlicht.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


#define MAX_OBJECTS 50

class EventHandler : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        // Remember whether each key is down or up
        if (event.EventType == irr::EET_KEY_INPUT_EVENT){
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
			printf("%i - > %i %i\n",event.KeyInput.Key,event.KeyInput.PressedDown, IsKeyDown(irr::KEY_KEY_W) );				
		}

        return false;
    }

    // This is used to check whether a key is being held down
    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }
    
    EventHandler()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }

private:
    // We use this array to store the current state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


int main()
{
	// create device
    EventHandler receiver;

	ISceneNode* objects [MAX_OBJECTS];

	 IrrlichtDevice *device =
		 createDevice( video::EDT_OPENGL, dimension2d<u32>(800, 600), 16,
            false, false, false, &receiver);

    if (!device)
        return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");


    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();

    IAnimatedMesh* mesh = smgr->getMesh("media/sydney.md2");
    if (!mesh)
    {
        device->drop();
        return 1;
    }
    IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

    if (node)
    {
        node->setMaterialFlag(EMF_LIGHTING, false);
        node->setMD2Animation(scene::EMAT_STAND);
        node->setMaterialTexture( 0, driver->getTexture("media/sydney.bmp") );
    }

	ICameraSceneNode* camera = smgr->addCameraSceneNode(0, vector3df(0,0,-40), vector3df(0,0,0));


	//naplníme tunel pøekážkama

	 srand (time(NULL));
	/* generate secret number between 1 and 10: */
	
	for(int i = 0; i < MAX_OBJECTS; i++){
		objects[i] = smgr->addCubeSceneNode(2);
		objects[i]->setPosition( vector3df( (rand() % 30) - 5, (rand() % 30) - 5, rand() % 80) );
	}



    while(device->run())
    {
        driver->beginScene(true, true, SColor(255,100,101,140));
		
		vector3df pos = camera->getPosition();

		if(receiver.IsKeyDown(irr::KEY_KEY_W)){
			camera->setPosition( pos+vector3df(0,0,0.1)  );
			camera->setTarget( pos+vector3df(0,0,5) );
		}

		for(int i = 0; i < MAX_OBJECTS; i++){
			vector3df tmpPos = objects[i]->getPosition();
			if(tmpPos.Z > pos.Z) continue;
			
			objects[i]->setPosition( vector3df( (rand() % 30) - 15, (rand() % 30) - 15, rand() % 80 + pos.Z) );
		}

		
            




        smgr->drawAll();
        guienv->drawAll();

        driver->endScene();
    }


    device->drop();

    return 0;
}