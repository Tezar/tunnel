#include <irrlicht.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "HDMStereoRender/HMDStereoRender.h" 
#include "OVR.h"
#include <iostream>
#include <conio.h>

#include "localSettings.h"

#include "level.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace OVR;
using namespace std;

Ptr<DeviceManager>	pManager;
Ptr<HMDDevice>		pHMD;
Ptr<SensorDevice>	pSensor;
SensorFusion		FusionResult;
HMDInfo			Info;
bool			InfoLoaded;

#define MAX_OBJECTS 50

void Clear();
void Init();
void Open();
void Output();

class EventHandler : public IEventReceiver
{
public:
   bool OnEvent(const SEvent& event)
   {   
      if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
        if (event.KeyInput.Key == KEY_ESCAPE)
        {
          device->closeDevice();
		  Clear();
          return true;
        }
      }
      return false;
   }
   IrrlichtDevice* device;

}; 

#ifdef OCCULUS
	const int ResX=1920;
	const int ResY=1080;
	bool fullscreen = true;
#else
	const int ResX=800;
	const int ResY=600;
	bool fullscreen = false;
#endif	

bool vsync = true;

int main()
{
	// create device
    EventHandler receiver;
	Init();
	Output();
	ISceneNode* objects [MAX_OBJECTS];



	 IrrlichtDevice *device = createDevice(EDT_OPENGL, dimension2d<u32>(ResX, ResY), 32, fullscreen, false, vsync, &receiver); 
	 
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


	#ifdef OCCULUS
		ICameraSceneNode* camera = smgr->addCameraSceneNode();
		camera->bindTargetAndRotation(false);
		camera->setTarget(vector3df(1,0,0));
	#else	
		smgr->addCameraSceneNodeFPS();
	#endif	

	
    device->getCursorControl()->setVisible(false); 


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
	  
	
	Level currentLevel(device);
	currentLevel.makeLevel(0);


	vector3df pos = vector3df(0,0,0);

	//naplníme tunel pøekážkama

	 srand (time(NULL));
	/* generate secret number between 1 and 10: */
	
	for(int i = 0; i < MAX_OBJECTS; i++){
		objects[i] = smgr->addCubeSceneNode(2);
		objects[i]->setMaterialFlag(EMF_LIGHTING, false);
		objects[i]->setPosition( vector3df( (rand() % 30) - 5, (rand() % 30) - 5, rand() % 80) );
	}

	//device->setInputReceivingSceneManager(smgr);
	
    while(device->run())
    {
        driver->beginScene(true, true, SColor(255,100,101,140));
		
		for(int i = 0; i < MAX_OBJECTS; i++){
			vector3df tmpPos = objects[i]->getPosition();
			if(tmpPos.Z > pos.Z) continue;
			
			objects[i]->setPosition( vector3df( (rand() % 30) - 15, (rand() % 30) - 15, rand() % 80 + pos.Z) );
		}
		
		
		if(pSensor){
			Quatf quaternion = FusionResult.GetOrientation();

		   ICameraSceneNode* camera = smgr->getActiveCamera();
   
		   vector3df rot; 
		   

		  // irr::core::quaternion q1(quaternion.x,quaternion.y, quaternion.z, quaternion.w);
		   irr::core::quaternion q1(-quaternion.z,quaternion.y,-quaternion.x, quaternion.w);
		   q1.normalize();
		   q1.toEuler(rot);
   
		   
		   //camera->getParent()->setRotation(rot*RADTODEG);


			float yaw, pitch, roll;
			quaternion.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
			camera->getParent()->setRotation( vector3df(RadToDegree(pitch),RadToDegree(yaw),RadToDegree(roll)));
			//camera->setRotation( vector3df(RadToDegree(-pitch),RadToDegree(-yaw),RadToDegree(roll)));
			//camera->setProjectionMatrix(ToMatrix(quaternion));
			cout << " Yaw: " << RadToDegree(yaw) << 
				", Pitch: " << RadToDegree(pitch) << 
				", Roll: " << RadToDegree(roll) << endl;
		
			if (_kbhit()) exit(0);
		}

		#ifdef OCCULUS
			renderer.drawAll(smgr); 
		#else
			smgr->drawAll();
		#endif

        guienv->drawAll();

        driver->endScene();
    }


    device->drop();
	Clear();
    return 0;
}

/** Oculus init **/
void Init()
{
	System::Init();

	pManager = *DeviceManager::Create();

	pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();

	if (pHMD)
        {
           InfoLoaded = pHMD->GetDeviceInfo(&Info);

	   pSensor = *pHMD->GetSensor();
	}
	else
	{
	   pSensor = *pManager->EnumerateDevices<SensorDevice>().CreateDevice();
	}

	if (pSensor)
	{
	   FusionResult.AttachToSensor(pSensor);
	}
}
void Clear()
{
	pSensor.Clear();
        pHMD.Clear();
	pManager.Clear();

	System::Destroy();
}
void Output()
{
	cout << "----- Oculus Console -----" << endl;

	if (pHMD)
	{
		cout << " [x] HMD Found" << endl;
	}
	else
	{
		cout << " [ ] HMD Not Found" << endl;
	}

	if (pSensor)
	{
		cout << " [x] Sensor Found" << endl;
	}
	else
	{
		cout << " [ ] Sensor Not Found" << endl;
	}

	cout << "--------------------------" << endl;

	if (InfoLoaded)
        {
		cout << " DisplayDeviceName: " << Info.DisplayDeviceName << endl;
		cout << " ProductName: " << Info.ProductName << endl;
		cout << " Manufacturer: " << Info.Manufacturer << endl;
		cout << " Version: " << Info.Version << endl;
		cout << " HResolution: " << Info.HResolution<< endl;
		cout << " VResolution: " << Info.VResolution<< endl;
		cout << " HScreenSize: " << Info.HScreenSize<< endl;
		cout << " VScreenSize: " << Info.VScreenSize<< endl;
		cout << " VScreenCenter: " << Info.VScreenCenter<< endl;
		cout << " EyeToScreenDistance: " << Info.EyeToScreenDistance << endl;
		cout << " LensSeparationDistance: " << Info.LensSeparationDistance << endl;
		cout << " InterpupillaryDistance: " << Info.InterpupillaryDistance << endl;
		cout << " DistortionK[0]: " << Info.DistortionK[0] << endl;
		cout << " DistortionK[1]: " << Info.DistortionK[1] << endl;
		cout << " DistortionK[2]: " << Info.DistortionK[2] << endl;
		cout << "--------------------------" << endl;
        }
}