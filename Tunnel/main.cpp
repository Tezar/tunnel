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
//#define EYES_OFFSET 5

float EYES_OFFSET = 5.0;
float VIEWPORT_OFFSET = 5.0;

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


const int ResX=1920;
const int ResY=1080;

int main(){
	int ViewPortOffset = 25;						// "frame" size
	int ViewPortSize = ResX/2-ViewPortOffset*2;		// ViewPortSize
	int ViewportTopOffset=(ResY-ViewPortSize)/2;	// top to ViewPort
	
	const int ResY=1080;
	// create device
    EventHandler receiver;

	ISceneNode* objects [MAX_OBJECTS];

	ICameraSceneNode* leftEye;
	ICameraSceneNode* rightEye;


	 IrrlichtDevice *device =
		 createDevice( video::EDT_DIRECT3D9, dimension2d<u32>(ResX, ResY), 32, true, false, false, &receiver);

    if (!device)
        return 1;

	device->setWindowCaption(L"A Hole");


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


	ISceneNode* cameraRig = smgr->addEmptySceneNode();
	leftEye = smgr->addCameraSceneNode(cameraRig);
	leftEye->bindTargetAndRotation(false);
	rightEye = smgr->addCameraSceneNode(cameraRig);
	


	vector3df pos = vector3df(0,0,0);

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
		
		if(receiver.IsKeyDown(irr::KEY_KEY_W)){
			pos.Z += 0.1;
			cameraRig->setPosition(pos);
		}

		if(receiver.IsKeyDown(irr::KEY_KEY_A)){
			EYES_OFFSET += 0.01;
			printf("eye %3.2f\n",EYES_OFFSET);
		}

		if(receiver.IsKeyDown(irr::KEY_KEY_D)){
			EYES_OFFSET -= 0.01;
			printf("eye %3.2f\n",EYES_OFFSET);
		}

		if(receiver.IsKeyDown(irr::KEY_KEY_Q)){
			VIEWPORT_OFFSET += 1;
			printf("view %3.2f\n",VIEWPORT_OFFSET);
		}

		if(receiver.IsKeyDown(irr::KEY_KEY_E)){
			VIEWPORT_OFFSET -= 1;
			printf("view %3.2f\n",VIEWPORT_OFFSET);
		}

		if(receiver.IsKeyDown(irr::KEY_ESCAPE)){
			break;
		}

		for(int i = 0; i < MAX_OBJECTS; i++){
			vector3df tmpPos = objects[i]->getPosition();
			if(tmpPos.Z > pos.Z) continue;
			
			objects[i]->setPosition( vector3df( (rand() % 30) - 15, (rand() % 30) - 15, rand() % 80 + pos.Z) );
		}

		leftEye->setPosition( vector3df(-EYES_OFFSET,5,0));
		rightEye->setPosition( vector3df(EYES_OFFSET,5,0));
		

		smgr->setActiveCamera(leftEye);
		leftEye->setTarget( pos + vector3df(-EYES_OFFSET,5,5));
		driver->setViewPort(rect<s32>(
			ViewPortOffset-VIEWPORT_OFFSET,
			ViewportTopOffset,
			ViewPortOffset+ViewPortSize-VIEWPORT_OFFSET,
			ViewPortSize));
        smgr->drawAll();

		smgr->setActiveCamera(rightEye);
		rightEye->setTarget( pos + vector3df(+EYES_OFFSET,5,5));
		driver->setViewPort(rect<s32>(
			ResX/2+VIEWPORT_OFFSET+ViewPortOffset,
			ViewportTopOffset,
			ResX/2+VIEWPORT_OFFSET+ViewPortOffset+ViewPortSize,
			ViewPortSize));
        smgr->drawAll();

        guienv->drawAll();

        driver->endScene();
    }


    device->drop();

    return 0;
}