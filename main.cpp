
// Library Headers
#include <irrlicht.h>
#include <irrNet.h> //do the NET
#include <irrAI.h>
#include <iostream>

//conver wchar
#include <string>
#include "atlbase.h"
#include "atlstr.h"
#include "comutil.h"

//#pragma comment(lib, "WINMM.lib") 

//#pragma comment(lib, "irrNetLited.lib")
#pragma comment(lib, "ws2_32.lib")

#include "main.h"

// Common Headers
#include "CSetupDevice.h"
#include "ProgressBar.h"
#include "FillMetaSelector.h"
#ifdef IRRPHYSX_OCCLUSION
#include "IrrPhysxOcclusionQuery.h"
#else
#include "IrrOcclusionQuery2.h"
#endif

// Project headers
#include "CPlayerCharacter.h"
#include "CNPCharacter.h"
#include "CChasingCharacter.h"
#include "CFleeingCharacter.h"

// Irrlicht namespaces
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;

IrrlichtDevice* device = NULL;
video::IVideoDriver* driver = NULL;
scene::ISceneManager* smgr = NULL;
gui::IGUIEnvironment* guienv = NULL;
core::array<CNPCharacter*> characters;
CPlayerCharacter* player = NULL;
gui::IGUIStaticText* pausedText = NULL;
gui::IGUIStaticText* occlusionText = NULL;
gui::IGUIStaticText* aiDebugText = NULL;
IAIManager* aimgr = NULL;
bool quitGame = false;
bool paused = false;
bool mouseDown = false;
bool messageClosed = false;
bool occlusion = true;
bool aiDebug = false;
SWaypointGroup* waypointGroup = NULL;
core::array<core::stringc> mapFiles;
scene::IMetaTriangleSelector* metaSelector = NULL;




//net stuff
enum E_PACKET_TYPE
{
	EPT_TYPE = 1,
	EPT_ID = 2,
	EPT_BUFF = 3,
	EPT_POS = 4,
	EPT_ROT = 5,
	EPT_ORBPOS = 6,
	EPT_STAT = 7,
	EPT_FIREPOS = 8,
	EPT_HIT = 9,
	EPT_DIE = 10,
	EPT_WIN = 12,
	EPT_MESSAGE = 13
};

	std::string netbuff;
	vector3df netPosition;
	vector3df netRotation;
	vector3df netOrbPos;
	std::string netStatus;
	vector3df netFirePos;
	std::string netwin;
	core::stringc message;
	s32 netId;
	s32 netType;
	bool netHit;
	bool netDie;

//net stuff end

//NET
//SERVER

class ServerNetCallback : public net::INetCallback
{
	// We will store a pointer to the net manager.
	net::INetManager* netManager;

	// Here we will store the cannon's power and angle.
	// Remember that f32 is just a typedef for float.
	//s32 id;
	//s32 Mana;
	//std::string netbuff;
	//vector3df netPosition;
	//vector3df netRotation;
	//vector3df netOrbPos;
	//std::string netStatus;
	//vector3df netFirePos;
	//std::string netwin;
public:
	ServerNetCallback(net::INetManager* netManagerIn) : netManager(netManagerIn) {}
	

		// Override the "onConnect" function, don't forget to get the method
	// signature correct. Whenever a fresh client connects, their brand
	// spanking new player id will be passed to this function. A "u16"
	// is a typedef for an unsigned short, incase you were wondering.
	virtual void onConnect(const u16 playerId)
	{
		// When a client connects we inform all other connected
		// clients that a client with that player id has connected.
		
		// But first, lets say that we have already banned a few
		// clients from this server, and that we are keeping a list
		// of all the banned ip addresses. We can simply use
		// "getClientAddress()", to obtain the 32-bit representation
		// of the ip address and check it against the list.
		//////bool playerIsBanned = false;
		//////for(int i = 0;i < banList.size();++i)
		//////{
		//////	if(netManager->getClientAddress(playerId) == banList[i])
		//////	{
		//////		playerIsBanned = true;
		//////		break;
		//////	}
		//////}
		//////
		//////// If the player is banned, send a message to all connected
		//////// clients that a player that is banned tried to connect
		//////// and then kick the naughty player.
		//////if(playerIsBanned)
		//////{
		//////	net::SOutPacket packet;
		//////	packet << 	"A player that is banned tried to connect, " \
		//////		 	"so I kicked them.";
		//////	netManager->sendOutPacket(packet);
		//////	
		//////	// Kick the client by passing the player id.
		//////	netManager->kickClient(playerId);
		//////}
		//////else // Else we tell everyone who connected.
		//////{
			// I am using a core::stringc here, it is very
			// similar to a std::string. You can use a std::string
			// if you want.
			net::SOutPacket packet;
			core::stringc message;
			message = "Client number ";
			message += playerId;
			message += " has just connected.";
			packet << message;
			netManager->sendOutPacket(packet);
		//////}			
	}
	
	// Similar to the onConnect function, except it happens when a
	// player disconnects. When this happens we will just report
	// which player has disconnected.
	virtual void onDisconnect(const u16 playerId)
	{
		net::SOutPacket packet;
		core::stringc message;
		message = "Client number ";
		message += playerId;
		message += " has just left the building.";
		packet << message;
		netManager->sendOutPacket(packet);
	}
	
	// Our handlePacket function.
	virtual void handlePacket(net::SInPacket& packet)
	{
		// The packets will use a single char to store
		// the packet identifier, remember to use the
		// smallest possible datatype for storing your
		// packet identifiers. c8 is a typedef for char.
		c8 packetid;
		packet >> packetid;
		
		// Here we will switch based on the packet id.
		switch((E_PACKET_TYPE)packetid)
		{
		//case EPT_BUFF:
		//	packet >> netbuff;
			//buffkapcsolás
		//	break;
		case EPT_ID:
			packet >> netId;
			//player ID
			break;
		case EPT_POS:
			packet >> netPosition;
			//player pozicionálás
			break;
		case EPT_ROT:
			packet >> netRotation;
			//player rotálás
			break;
		//case EPT_ORBPOS:
		//	packet >> netOrbPos;
			//player orb pozicinálás
		//	break;
		case EPT_STAT:
			packet >> netStatus;
			//player státuszolás
			break;
		case EPT_FIREPOS:
			packet >> netFirePos;
			//player lövés pozicionálás
			break;
		case EPT_WIN:
			packet >> netwin;
			//player nyerés
			break;
		case EPT_MESSAGE:
			core::stringc message;
			packet >> message;
			std::cout << "Client " << packet.getPlayerId() << " says: " << message.c_str();
			std::cout << std::endl;
			break;
		}
		
		// After handling a packet, we will send an updated status of the cannon to all clients.
		//net::SOutPacket buffPacket;
		
		// The packet id is the first thing that goes in a packet. Note that I am casting it to a char,
		// because that is what we want to store it as, to save space. Be careful to use the same types
		// when sending and receiving, don't send as a char and receive as an int, it will cause trouble.
		//buffPacket << (c8)EPT_BUFF;
		//buffPacket << netbuff;
		
		// Send the packet to all connected clients.
		//netManager->sendOutPacket(buffPacket);

		
		//// Send a power update too.
		net::SOutPacket idPacket;
		idPacket << (c8)EPT_ID;
		idPacket << netId;
		netManager->sendOutPacket(idPacket);

		net::SOutPacket posPacket;
		posPacket << (c8)EPT_POS;
		posPacket << netPosition;
		netManager->sendOutPacket(posPacket);

		net::SOutPacket rotPacket;
		rotPacket << (c8)EPT_ROT;
		rotPacket << netRotation;
		netManager->sendOutPacket(rotPacket);

		net::SOutPacket statPacket;
		statPacket << (c8)EPT_STAT;
		statPacket << netStatus;
		netManager->sendOutPacket(statPacket);

		net::SOutPacket firePacket;
		firePacket << (c8)EPT_FIREPOS;
		firePacket << netFirePos;
		netManager->sendOutPacket(firePacket);

		net::SOutPacket winPacket;
		winPacket << (c8)EPT_WIN;
		winPacket << netwin;
		netManager->sendOutPacket(winPacket);
	}
};


//CLIENT
// The client callback.
class ClientNetCallback : public net::INetCallback
{
public:
	// Our handlePacket function.
	virtual void handlePacket(net::SInPacket& packet)
	{
		// Just like the server, we obtain the packet id and print
		// the information based on the packet we received. I hope the
		// rest of this function is self-explanatory.
		c8 packetid;
		packet >> packetid;

		
		switch((E_PACKET_TYPE)packetid)
		{
		//case EPT_BUFF:
		//	//std::string netbuff;
		//	packet >> netbuff;
		//	//buffkapcsolás
		//	break;
		case EPT_POS:
			//vector3df netPosition;
			packet >> netPosition;
			//player pozicionálás
			break;
		case EPT_ROT:
			//vector3df netRotation;
			packet >> netRotation;
			//player rotálás
			break;
		//case EPT_ORBPOS:
		//	//vector3df netOrbPos;
		//	packet >> netOrbPos;
		//	//player orb pozicinálás
		//	break;
		case EPT_STAT:
			//std::string netStatus;
			packet >> netStatus;
			//player státuszolás
			break;
		case EPT_FIREPOS:
			//vector3df netFirePos;
			packet >> netFirePos;
			//player lövés pozicionálás
			break;
		case EPT_WIN:
			//std::string netwin;
			packet >> netwin;
			//player nyerés
			break;
		case EPT_MESSAGE:
			core::stringc message;
			packet >> message;
			std::cout << "Client " << packet.getPlayerId() << " says: " << message.c_str();
			std::cout << std::endl;
			break;
		}
	}
};

void cleanUp() {
    
	// Remove characters
	for (u32 i = 0 ; i < characters.size() ; ++i)
		delete characters[i];     
	characters.clear();
  
	if (player) {
		delete player;
		player = NULL;
	}
  
	// Clear out AI Manager
	if (aimgr) {
		aimgr->clear();
		removeAIManager(aimgr);
		aimgr = NULL;
	}

	// Shutdown device
	if (device) {
		device->closeDevice();
		device->drop();
		device = NULL;
	}
  
}

class MyEventReceiver : public IEventReceiver {
      
	public:
		virtual bool OnEvent(const SEvent& event) {

			if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
				switch(event.KeyInput.Key) {
					case KEY_ESCAPE: {
						// Quit the game
						quitGame = true;
						return true;     
					}
					case KEY_KEY_V: {
						aiDebug = !aiDebug;
						if (aimgr) aimgr->setDebugVisible(aiDebug);
						if (aiDebugText) aiDebugText->setText(aiDebug?L"ON":L"OFF");
						return true;
					} 
					case KEY_KEY_P: {
						// Pause the scene
						paused = !paused;
						if (pausedText) pausedText->setVisible(paused);
						if (paused) { // Freezes animators and other timer dependent things
							if (device) device->getTimer()->stop();
							if (smgr && player && player->getCharacterType() != ECT_SPECTATING) smgr->getActiveCamera()->setInputReceiverEnabled(false);
						} else {// Unfreezes animators and other timer dependent things
							if (device) device->getTimer()->start();
							if (smgr) smgr->getActiveCamera()->setInputReceiverEnabled(true);
						}
						return true;
					} 
					case KEY_KEY_O: {
						occlusion = !occlusion;
						if (aimgr) aimgr->setOcclusionQueryCallback(occlusion?&occlusionQuery:NULL);
						if (occlusionText) occlusionText->setText(occlusion?L"ON":L"OFF");
						return true;     
					}
				}
			} else if (event.EventType == EET_MOUSE_INPUT_EVENT) {
				switch (event.MouseInput.Event) {
					case EMIE_LMOUSE_PRESSED_DOWN:
						mouseDown = true;
						break;  
					case EMIE_LMOUSE_LEFT_UP:
						mouseDown = false;
						break;     
				}     
			} else if (event.EventType == EET_GUI_EVENT) {
				if (event.GUIEvent.EventType == gui::EGET_MESSAGEBOX_OK) {
					messageClosed = true;
				}
			}  
          
			return false;
      
		}
	
};

void populateSetupWindow(CSetupDevice* setupDevice) {
  
	if (!setupDevice) return;
     
	gui::IGUIEnvironment* setupGUI = setupDevice->getGUIEnvironment();
	gui::IGUIComboBox* box = NULL;

	// create a combobox for selecting server or client
	setupGUI->addStaticText(L"Tipus:", core::rect<s32>(10,50, 100,80), false, true);
	box = setupGUI->addComboBox(core::rect<s32>(60,50, 180,70), NULL, 2);
	box->addItem(L"Server");
	box->addItem(L"Client");
	box->setSelected(0);
    
	// create an editbox for ip
	setupGUI->addStaticText(L"IP:", core::rect<s32>(220,50, 300,80), false, true);
	setupGUI->addEditBox(L"127.0.0.1", core::rect<s32>(270,50, 380,70), true, NULL, 4);
	//box->addItem(L"Chasing");
	//box->addItem(L"Fleeing");
	//box->addItem(L"Spectating");
	//box->setSelected(0);
  
	// num enemies on each team
	setupGUI->addStaticText(L"No. Bots:", core::rect<s32>(10,80, 100,110), false, true);
	setupGUI->addEditBox(L"1", core::rect<s32>(110,80, 180,100), true, NULL, 3);
	//setupGUI->addStaticText(L"No. Fleeing:", core::rect<s32>(10,110, 100,140), false, true);
	//setupGUI->addEditBox(L"1", core::rect<s32>(110,110, 180,130), true, NULL, 4);
  
	// create a combobox for selecting the map
	setupGUI->addStaticText(L"Map:", core::rect<s32>(10,20, 100,50), false, true);
	box = setupGUI->addComboBox(core::rect<s32>(60,20, 180,40), NULL, 5);
	io::IFileSystem* fs = setupDevice->getDevice()->getFileSystem();
	const io::path workingDir = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo("../../Media");
	io::IFileList* fl = fs->createFileList();
	for (u32 i = 0 ; i < fl->getFileCount() ; i++) {
		if (!fl->isDirectory(i)) {
			core::stringc strc = fl->getFileName(i);
			s32 idx = strc.findLast('.');
			if (idx != -1 && strcmp(&strc.c_str()[idx], ".irr") == 0) {
				core::stringw strw = strc.c_str();
				box->addItem(strw.c_str());
				mapFiles.push_back(strc);
			}
		}
	} 
	box->setSelected(0);
	fs->changeWorkingDirectoryTo(workingDir);
  
}

void progressBarRender() {

	guienv->drawAll();

}

int main() {
   

	////Server vs Client
	//std::cout << "Client (c) or Server (s)?\n";
	//char lan;
	//std::cin >> lan;



	// Create a software based device to allow the user to choose some options
	CSetupDevice* setupDevice = new CSetupDevice(core::dimension2d<u32>(480,200));
	if (!setupDevice) {
		printf("Failed setupDevice creation\n");
		cleanUp();
		return 1;
	}
  
	gui::IGUIEnvironment* setupGUI = setupDevice->getGUIEnvironment();
	populateSetupWindow(setupDevice);
  
	if (setupDevice->execute()) { // user closed the window... they don't want to play my game :'(
		printf("quit setup\n");
		delete setupDevice;
		setupDevice = NULL;
		setupGUI = NULL;
		cleanUp();               
		return 0;
	}
    
	s32 playerType = ECT_CHASING;
	s32 programType = ((gui::IGUIComboBox*)setupGUI->getRootGUIElement()->getElementFromId(2))->getSelected();
	s32 numBots = _wtoi(((gui::IGUIEditBox*)setupGUI->getRootGUIElement()->getElementFromId(3))->getText());
	if (numBots >= 10) numBots = 10;
	wchar_t *w_ipAddress = new wchar_t[15];
	wcsncpy_s(w_ipAddress,15,(((gui::IGUIEditBox*)setupGUI->getRootGUIElement()->getElementFromId(4))->getText()),15);

	//convert widechar to char
	size_t origsize = wcslen(w_ipAddress) + 1;
    size_t convertedChars = 0;
    // Use a multibyte string to append the type of string
    // to the new string before displaying the result.
    char strConcat[] = "";
    size_t strConcatsize = (strlen( strConcat ) + 1)*2;
    // Allocate two bytes in the multibyte output string for every wide
    // character in the input string (including a wide character
    // null). Because a multibyte character can be one or two bytes,
    // you should allot two bytes for each character. Having extra
    // space for the new string is not an error, but having
    // insufficient space is a potential security problem.
    const size_t newsize = origsize*2;
    // The new string will contain a converted copy of the original
    // string plus the type of string appended to it.
    char *ipAddress = new char[newsize+strConcatsize];
    // Put a copy of the converted string into nstring
    wcstombs_s(&convertedChars, ipAddress, newsize, w_ipAddress, _TRUNCATE);
    // append the type of string to the new string.
    _mbscat_s((unsigned char*)ipAddress, newsize+strConcatsize, (unsigned char*)strConcat);



	s32 driverType = ((gui::IGUIComboBox*)setupGUI->getRootGUIElement()->getElementFromId(1))->getSelected() + 1; // EDT_NULL not provided on the list so must factor that in
	s32 mapIndex = ((gui::IGUIComboBox*)setupGUI->getRootGUIElement()->getElementFromId(5))->getSelected();
    
	//delete setupDevice;
	//setupDevice = NULL;
	//setupGUI = NULL;
	mouseDown = false;
  
	// Create the proper device with the selected driver and the desktop resolution
	MyEventReceiver receiver;
	device = createDevice(video::E_DRIVER_TYPE(driverType), core::dimension2d<u32>(800,600), 24, false, false, true, &receiver);
	if (!device) {
		printf("Failed device creation\n");
		cleanUp();
		return 1;
	}
	device->getCursorControl()->setVisible(false);
	device->run(); // recreating the device makes run() return false first time so call it here so the render loop below won't quit early
  	//device->run(); // recreating the device makes run() return false first time so call it here so the render loop below won't quit early

	// Re-grab the new device pointers and set the font again
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();




	//////////////////
	//Server version//
	//////////////////
	if (programType == 0)	{
		// Create an irrNetLite server.
		net::INetManager* netManager = net::createIrrNetServer(0);
		
		// Pass in a server specific net callback.
		ServerNetCallback* serverCallback = new ServerNetCallback(netManager);
		netManager->setNetCallback(serverCallback);
		//netManager->setVerbose(true);

	//setup

	updateProgress(device, 0, &progressBarRender);
  
	guienv->getSkin()->setFont(guienv->getFont("../../Media/GillSans12.png")); 
  
	// Initialise the AI manager
	// Must be done before smgr->loadScene() as it tells smgr how to create WaypointSceneNodes
	aimgr = createAIManager(device);
	if (!aimgr) {
		printf("Failed irrAIManager creation\n");
		cleanUp();
		return 1;
	} 
    
	updateProgress(device, 20, &progressBarRender);
  
	// Load the scene created with irrEdit
	core::stringc mapPath = "../../Media/";
	mapPath += mapFiles[mapIndex];
	smgr->loadScene(mapPath.c_str()); 
  
	updateProgress(device, 30, &progressBarRender);
  
	// Create a metaSelector from the objects in the scene
	metaSelector = smgr->createMetaTriangleSelector();
	fillMetaSelector(device, metaSelector, smgr->getRootSceneNode());
	// Set up occlusion callback info
	#ifndef IRRPHYSX_OCCLUSION
	setOcclusionSceneManager(smgr);
	setOcclusionTriangleSelector(metaSelector);
	#else
	readScene(device);
	#endif
	if (occlusion) aimgr->setOcclusionQueryCallback(&occlusionQuery);

	updateProgress(device, 40, &progressBarRender);
  
	// Load the AI data as created in the IrrAI Editor
	core::stringc aiPath = "../../Media/";
	s32 pos = mapFiles[mapIndex].findLast('_');
	if (pos != -1) {
		aiPath += mapFiles[mapIndex].subString(0, pos);
		aiPath += ".irrai";      
	}
	aimgr->loadAI(aiPath.c_str());
	aimgr->createDebugWaypointMeshes();
	waypointGroup = aimgr->getWaypointGroupFromIndex(0);
  
	updateProgress(device, 50, &progressBarRender);
  
	// Create our NPCs
	CNPCharacter* character;
	CCharacter::SCharacterDesc desc;
      
	for (s32 i = 0 ; i < numBots ; ++i) {
		desc.SceneManager = smgr;
		desc.AIManager = aimgr;
		desc.StartWaypointID = -1;
		desc.WaypointGroupName = waypointGroup->getName();
		character = new CChasingCharacter(desc, "../../Media/init2win.md2", "../../Media/init2winred.jpg", metaSelector);
		if (!character) printf("Failed bot character creation\n");
		characters.push_back(character);
	}
  
	//for (s32 i = 0 ; i < numFleeing ; ++i) {
	//	desc.SceneManager = smgr;
	//	desc.AIManager = aimgr;
	//	desc.StartWaypointID = -1;
	//	desc.WaypointGroupName = waypointGroup->getName();
	//	character = new CFleeingCharacter(desc, "../../Media/init2win.md2", "../../Media/init2winblue.jpg");
	//	if (!character) printf("Failed character2 creation\n");
	//	characters.push_back(character);
	//}
    
	// Create two opposing 'teams'
	SEntityGroup* botTeam = aimgr->createEntityGroup();
	SEntityGroup* playerTeam = aimgr->createEntityGroup();
  
	for (u32 i = 0 ; i < characters.size() ; ++i) {
		if (characters[i]->getCharacterType() == ECT_CHASING) {
			botTeam->addEntity(characters[i]->getAIEntity());
			characters[i]->getAIEntity()->setEnemyGroup(playerTeam);
			characters[i]->getAIEntity()->setAllyGroup(botTeam);
//			characters[i]->getAIEntity()->getID();
		//} else {
		//	fleeTeam->addEntity(characters[i]->getAIEntity());
		//	characters[i]->getAIEntity()->setEnemyGroup(chaseTeam);
		//	characters[i]->getAIEntity()->setAllyGroup(fleeTeam);
		}     
	}
  
	updateProgress(device, 60, &progressBarRender);
    
	// Setup the camera, positioned over waypoint 2
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(NULL, 75.0f, 0.25f, -1, NULL, 0, playerType != ECT_SPECTATING);
	if (!camera) {
		printf("Failed camera creation\n");
		cleanUp();
		return 1;
	}
	IWaypoint* waypoint = aimgr->getWaypointFromId(waypointGroup, 1);
	if (waypoint) 
		camera->setPosition(waypoint->getPosition()+core::vector3df(0,150,0));
	else
		camera->setPosition(core::vector3df(0,0,0));  
	camera->setTarget(core::vector3df(0,0,0));
    
	// give it a collision animator so you can't go through walls and so it will be affected by gravity if the player is not spectating.
	core::vector3df gravity(0,-10.0f,0);
	if (playerType == ECT_SPECTATING) gravity = core::vector3df(0,0,0);
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(metaSelector, camera, core::vector3df(15,60,15), gravity, core::vector3df(0,0,0));
	camera->addAnimator(anim);
	anim->drop();
  
	// Create the player
	CCharacter::SCharacterDesc pDesc;
	pDesc.SceneManager = smgr;
	pDesc.AIManager = aimgr;
	player = new CPlayerCharacter(pDesc, camera, metaSelector, playerType);
	if (!player) {
		printf("failed player creation\n");             
	}
  
	////if (playerType == ECT_FLEEING) {
	////	fleeTeam->addEntity(player->getAIEntity());
	////	player->getAIEntity()->setEnemyGroup(chaseTeam);
	////	player->getAIEntity()->setAllyGroup(fleeTeam);
	////} else 
	if (playerType == ECT_CHASING) {
		playerTeam->addEntity(player->getAIEntity());
		player->getAIEntity()->setEnemyGroup(botTeam);
		//player->getAIEntity()->setAllyGroup(chaseTeam);
	}
  
	updateProgress(device, 70, &progressBarRender);
  
	// Pre-load explosion and projectile textures
	c8 path[100];
	for (s32 i = 1 ; i <= 11 ; ++i) {
		snprintf(path, 100, "../../Media/Explosion/%02d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(70+i), &progressBarRender);
	}
	for (s32 i = 1 ; i <= 7 ; ++i) {
		snprintf(path, 100, "../../Media/Projectile/portal%d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(82+i), &progressBarRender);
	}
	for (s32 i = 1 ; i <= 6 ; ++i) {
		snprintf(path, 100, "../../Media/Plasmaball/%02d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(89+i), &progressBarRender);
	}
    
	updateProgress(device, 95, &progressBarRender);
         
	// App info
	guienv->addImage(core::rect<s32>(20,23,525,225));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"", core::rect<s32>(25,25,650,50));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	pausedText = guienv->addStaticText(L"PAUSED", core::rect<s32>(450,25,650,50));
	pausedText->setVisible(paused);
	pausedText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* infoText = guienv->addStaticText(L"Cursors - move\nMouse - look\nLeft mouse button - fire (if chasing)\nV - Toggle AI debug info\nO - Toggle FOV Occlusion\nP - Pause\nEsc - Exit", core::rect<s32>(25,50,650,225));
	infoText->setOverrideColor(video::SColor(255,255,255,255));
	aiDebugText = guienv->addStaticText(aiDebug?L"ON":L"OFF", core::rect<s32>(450,125,650,150));
	aiDebugText->setOverrideColor(video::SColor(255,255,255,255));
	occlusionText = guienv->addStaticText(occlusion?L"ON":L"OFF", core::rect<s32>(450,150,650,175));
	occlusionText->setOverrideColor(video::SColor(255,255,255,255));
	guienv->addImage(driver->getTexture("../../Media/irrlichtirrailogo.png"), core::position2di(800-150,600-125));
	#ifdef IRRPHYSX_OCCLUSION
	guienv->addImage(driver->getTexture("../../Media/irrlichtirrphysxlogo.png"), core::position2di(25,600-125));
	guienv->addImage(driver->getTexture("../../Media/nvidiaphysxlogo.png"), core::position2di(150,600-125));
	#endif
      
	updateProgress(device, 100);
	device->sleep(500);

  
	// Ready to start running and rendering the app now!
	s32 lastFPS = -1;
	s32 startTime = device->getTimer()->getTime();
	s32 elapsedTime = 0;
	s32 timeSinceBlueEradication = 0;

	while(device->run() && !quitGame)
	{
        
		if (!device->isWindowActive()) continue;
    
		// Calculate elapsed time
		elapsedTime = device->getTimer()->getTime() - startTime;
		startTime = device->getTimer()->getTime();
    
		if (botTeam->Entities.size() == 0) timeSinceBlueEradication += elapsedTime;
		if (!player || timeSinceBlueEradication > 1000) break; // All bots destroyed OR player destroyed, game over
    
		if (!paused)
		{   
			// Update AIManager
			aimgr->update(elapsedTime);
			// Update characters
			for (u32 i = 0 ; i < characters.size() ; ++i) 
			{
				if (characters[i]->update(elapsedTime)) 
				{
					delete characters[i];
					characters.erase(i);
					i--;
				}  
			}     
			if (player) 
			{
				if (mouseDown) player->fire();
				if (player->update(elapsedTime)) 
				{
					delete player;
					player = NULL;
				}
			}
		}
          
		// Update FPS
		s32 fps = driver->getFPS();
		if (lastFPS != fps) 
		{
			wchar_t wstr[1024];
			swprintf(wstr, 1024, L"IrrAI v%.1f- FPS Example [%ls] fps: %d", IRRAI_VERSION, driver->getName(), fps);
			fpsText->setText(wstr);
			lastFPS = fps;
		}
        
		// Render scene
		driver->beginScene(true, true, video::SColor(255,0,0,0));
    
		smgr->drawAll();
		guienv->drawAll(); 
    
		if (player && player->getCharacterType() != ECT_SPECTATING) 
		{
			// draw player's resource level
			s32 barWidth = 256;
			s32 barHeight = 16;
			s32 value = 0;
			f32 valueMax = 0.1f;
			video::SColor colours[4];
			core::stringc str;
			if (player->getCharacterType() == ECT_CHASING) 
			{
				colours[0] = video::SColor(255,0,0,255);
				colours[1] = video::SColor(255,0,0,255);
				colours[2] = video::SColor(255,0,0,255);
				colours[3] = video::SColor(255,0,0,255);
				value = player->getAmmo();
				valueMax = (f32)CCharacter::MAX_AMMO;
				str = "Ammo: ";
				s32 imgDim = 64;
				driver->draw2DImage(driver->getTexture("../../Media/crosshair.png"), core::rect<s32>(400-(imgDim/2),300-(imgDim/2), 400+(imgDim/2),300+(imgDim/2)), core::rect<s32>(0,0, imgDim,imgDim), NULL, NULL, true);
			} else 
			{
				colours[0] = video::SColor(255,0,255,0);
				colours[1] = video::SColor(255,0,255,0);
				colours[2] = video::SColor(255,0,255,0);
				colours[3] = video::SColor(255,0,255,0);
				value = player->getHealth();  
				valueMax = (f32)CCharacter::MAX_HEALTH;  
				str = "Health: ";
			}
			core::rect<s32> clip(400-(barWidth/2),(600-50)-barHeight, (400-(barWidth/2))+((s32)(barWidth*(value/valueMax))),600-50);
			driver->draw2DImage(driver->getTexture("../../Media/progress_bar.png"),
                        core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50),
                        core::rect<s32>(0,0, barWidth,barHeight),
                        &clip, colours, true);
			driver->draw2DImage(driver->getTexture("../../Media/progress_box.png"), core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50), core::rect<s32>(0,0, barWidth,barHeight), NULL, NULL, true);
			core::stringw strw = str.c_str();
			strw += value;
			guienv->getSkin()->getFont()->draw(strw.c_str(), core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50), video::SColor(255,255,255,255), true, true);
		}
    
		driver->endScene();
        
	}
  
	if (device->run() && !quitGame) 
	{ // If the user didn't quit (with esc) and the device is still running then the only way to get here is if all J5s were destroyed OR the player died
		// display the necessary message
		core::stringw msg;
		if (player) 
			if (player->getCharacterType() == ECT_SPECTATING)    msg = "Those damned blues are no more... game over!"; // player spectating so no credit
			else                                                 msg = "Those damned blues are no more... congratulations!"; // player not dead then they destroyed all J5s
		else        msg = "You died... commiserations"; // player died
  

		//sending out the bot data to the clients
		net::SOutPacket packet;
		for (u32 i = 0 ; i < characters.size() ; ++i) {
			packet << netId << characters[i]->getAIEntity()->getID();
			packet << netPosition << characters[i]->getAIEntity()->getAbsolutePosition();
			//packet << netStatus << characters[i]->getAIEntity()->
		}
		packet.compressPacket();

		guienv->addMessageBox(L"Game Over", msg.c_str());
		device->getCursorControl()->setVisible(true);
		device->getTimer()->stop();
		smgr->getActiveCamera()->setInputReceiverEnabled(false);
     
		// wait for message box to be closed or esc to be pressed
		while (device->run() && !messageClosed && !quitGame) 
		{
			driver->beginScene(true, true, video::SColor(255,0,0,0));
			smgr->drawAll();
			guienv->drawAll();
			driver->endScene(); 
			netManager->update(1000); 
		}
    
		device->getTimer()->start();
	}

	delete netManager;
	//	delete clientCallback;
	cleanUp();

  
	// Reset fields
	quitGame = false;
	paused = false;
	messageClosed = false;

  
	return main();
	}



	//////////////////
	//Client version//
	//////////////////
	if(programType == 1)
	{
		//// Create an irrNetLite server.
		////net::INetManager* netManager = net::createIrrNetServer(0);
		////
		////// Pass in a server specific net callback.
		////ServerNetCallback* serverCallback = new ServerNetCallback(netManager);
		////netManager->setNetCallback(serverCallback);
		////netManager->setVerbose(true);

		ClientNetCallback* clientCallback = new ClientNetCallback();
		net::INetManager* netManager = net::createIrrNetClient(clientCallback, ipAddress);
		
		// The clients in this example will simply send a custom greeting message
		// when they connect and then wait and poll for events.
		
		// If there wasn't a problem connecting we will send a greeting message.
		if(netManager->getConnectionStatus() != net::EICS_FAILED)
		{
			// Print a simple menu.
			std::cout << "Connection Established" << std::endl;
					
			//// Take the input.
			//std::string message;
			//std::cin >> message;
			//
			//// Send a packet with the message entered.
			//net::SOutPacket packet;
			//packet << message;
			//netManager->sendOutPacket(packet);
		}




	//setup
  
	updateProgress(device, 0, &progressBarRender);
  
	guienv->getSkin()->setFont(guienv->getFont("../../Media/GillSans12.png")); 
  
	// Initialise the AI manager
	// Must be done before smgr->loadScene() as it tells smgr how to create WaypointSceneNodes
	aimgr = createAIManager(device);
	if (!aimgr) {
		printf("Failed irrAIManager creation\n");
		cleanUp();
		return 1;
	} 
    
	updateProgress(device, 20, &progressBarRender);
  
	// Load the scene created with irrEdit
	core::stringc mapPath = "../../Media/";
	mapPath += mapFiles[mapIndex];
	smgr->loadScene(mapPath.c_str()); 
  
	updateProgress(device, 30, &progressBarRender);
  
	// Create a metaSelector from the objects in the scene
	metaSelector = smgr->createMetaTriangleSelector();
	fillMetaSelector(device, metaSelector, smgr->getRootSceneNode());
	// Set up occlusion callback info
	#ifndef IRRPHYSX_OCCLUSION
	setOcclusionSceneManager(smgr);
	setOcclusionTriangleSelector(metaSelector);
	#else
	readScene(device);
	#endif
	if (occlusion) aimgr->setOcclusionQueryCallback(&occlusionQuery);

	updateProgress(device, 40, &progressBarRender);
  
	// Load the AI data as created in the IrrAI Editor
	core::stringc aiPath = "../../Media/";
	s32 pos = mapFiles[mapIndex].findLast('_');
	if (pos != -1) {
		aiPath += mapFiles[mapIndex].subString(0, pos);
		aiPath += ".irrai";      
	}
	aimgr->loadAI(aiPath.c_str());
	aimgr->createDebugWaypointMeshes();
	waypointGroup = aimgr->getWaypointGroupFromIndex(0);
  
	updateProgress(device, 50, &progressBarRender);
  
	// Create our NPCs
	CNPCharacter* character;
	CCharacter::SCharacterDesc desc;
  
	//server handles the bots
	//for (s32 i = 0 ; i < numBots ; ++i) {
	//	desc.SceneManager = smgr;
	//	desc.AIManager = aimgr;
	//	desc.StartWaypointID = -1;
	//	desc.WaypointGroupName = waypointGroup->getName();
	//	character = new CChasingCharacter(desc, "../../Media/init2win.md2", "../../Media/init2winred.jpg", metaSelector);
	//	if (!character) printf("Failed bot character creation\n");
	//	characters.push_back(character);
	//}
  
	//for (s32 i = 0 ; i < numFleeing ; ++i) {
	//	desc.SceneManager = smgr;
	//	desc.AIManager = aimgr;
	//	desc.StartWaypointID = -1;
	//	desc.WaypointGroupName = waypointGroup->getName();
	//	character = new CFleeingCharacter(desc, "../../Media/init2win.md2", "../../Media/init2winblue.jpg");
	//	if (!character) printf("Failed character2 creation\n");
	//	characters.push_back(character);
	//}
    
	// Create two opposing 'teams'
	SEntityGroup* botTeam = aimgr->createEntityGroup();
	SEntityGroup* playerTeam = aimgr->createEntityGroup();
  
	//for (u32 i = 0 ; i < characters.size() ; ++i) {
	//	if (characters[i]->getCharacterType() == ECT_CHASING) {
	//		botTeam->addEntity(characters[i]->getAIEntity());
	//		characters[i]->getAIEntity()->setEnemyGroup(playerTeam);
	//		characters[i]->getAIEntity()->setAllyGroup(botTeam);
	//	//} else {
	//	//	fleeTeam->addEntity(characters[i]->getAIEntity());
	//	//	characters[i]->getAIEntity()->setEnemyGroup(chaseTeam);
	//	//	characters[i]->getAIEntity()->setAllyGroup(fleeTeam);
	//	}   
	//}
  
	updateProgress(device, 60, &progressBarRender);
   
	// Setup the camera, positioned over waypoint 2
	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS(NULL, 75.0f, 0.25f, -1, NULL, 0, playerType != ECT_SPECTATING);
	if (!camera) {
		printf("Failed camera creation\n");
		cleanUp();
		return 1;
	}
	IWaypoint* waypoint = aimgr->getWaypointFromId(waypointGroup, 1);
	if (waypoint) 
		camera->setPosition(waypoint->getPosition()+core::vector3df(0,150,0));
	else
		camera->setPosition(core::vector3df(0,0,0));  
	camera->setTarget(core::vector3df(0,0,0));
    
	// give it a collision animator so you can't go through walls and so it will be affected by gravity if the player is not spectating.
	core::vector3df gravity(0,-10.0f,0);
	if (playerType == ECT_SPECTATING) gravity = core::vector3df(0,0,0);
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(metaSelector, camera, core::vector3df(15,60,15), gravity, core::vector3df(0,0,0));
	camera->addAnimator(anim);
	anim->drop();
  
	// Create the player
	CCharacter::SCharacterDesc pDesc;
	pDesc.SceneManager = smgr;
	pDesc.AIManager = aimgr;
	player = new CPlayerCharacter(pDesc, camera, metaSelector, playerType);
	if (!player) {
		printf("failed player creation\n");             
	}
  
	////if (playerType == ECT_FLEEING) {
	////	fleeTeam->addEntity(player->getAIEntity());
	////	player->getAIEntity()->setEnemyGroup(chaseTeam);
	////	player->getAIEntity()->setAllyGroup(fleeTeam);
	////} else 
	if (playerType == ECT_CHASING) {
		playerTeam->addEntity(player->getAIEntity());
		player->getAIEntity()->setEnemyGroup(botTeam);
		//player->getAIEntity()->setAllyGroup(chaseTeam);
	}
  
	updateProgress(device, 70, &progressBarRender);
  
	// Pre-load explosion and projectile textures
	c8 path[100];
	for (s32 i = 1 ; i <= 11 ; ++i) {
		snprintf(path, 100, "../../Media/Explosion/%02d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(70+i), &progressBarRender);
	}
	for (s32 i = 1 ; i <= 7 ; ++i) {
		snprintf(path, 100, "../../Media/Projectile/portal%d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(82+i), &progressBarRender);
	}
	for (s32 i = 1 ; i <= 6 ; ++i) {
		snprintf(path, 100, "../../Media/Plasmaball/%02d.jpg", i);
		driver->getTexture(path);
		updateProgress(device, (f32)(89+i), &progressBarRender);
	}
    
	updateProgress(device, 95, &progressBarRender);
         
	// App info
	guienv->addImage(core::rect<s32>(20,23,525,225));
	gui::IGUIStaticText* fpsText = guienv->addStaticText(L"", core::rect<s32>(25,25,650,50));
	fpsText->setOverrideColor(video::SColor(255,255,255,255));
	pausedText = guienv->addStaticText(L"PAUSED", core::rect<s32>(450,25,650,50));
	pausedText->setVisible(paused);
	pausedText->setOverrideColor(video::SColor(255,255,255,255));
	gui::IGUIStaticText* infoText = guienv->addStaticText(L"Cursors - move\nMouse - look\nLeft mouse button - fire (if chasing)\nV - Toggle AI debug info\nO - Toggle FOV Occlusion\nP - Pause\nEsc - Exit", core::rect<s32>(25,50,650,225));
	infoText->setOverrideColor(video::SColor(255,255,255,255));
	aiDebugText = guienv->addStaticText(aiDebug?L"ON":L"OFF", core::rect<s32>(450,125,650,150));
	aiDebugText->setOverrideColor(video::SColor(255,255,255,255));
	occlusionText = guienv->addStaticText(occlusion?L"ON":L"OFF", core::rect<s32>(450,150,650,175));
	occlusionText->setOverrideColor(video::SColor(255,255,255,255));
	guienv->addImage(driver->getTexture("../../Media/irrlichtirrailogo.png"), core::position2di(800-150,600-125));
	#ifdef IRRPHYSX_OCCLUSION
	guienv->addImage(driver->getTexture("../../Media/irrlichtirrphysxlogo.png"), core::position2di(25,600-125));
	guienv->addImage(driver->getTexture("../../Media/nvidiaphysxlogo.png"), core::position2di(150,600-125));
	#endif
      
	updateProgress(device, 100);
	device->sleep(500);

  
	// Ready to start running and rendering the app now!
	s32 lastFPS = -1;
	s32 startTime = device->getTimer()->getTime();
	s32 elapsedTime = 0;
	s32 timeSinceBlueEradication = 0;

	//report to the server a new client to handle
	//net::SOutPacket IDpacket;
	//IDpacket << "+1";
	//IDpacket.compressPacket();
	//netManager->sendOutPacket(IDpacket);

	while(device->run() && !quitGame) {
        
		if (!device->isWindowActive()) continue;
    
		// Calculate elapsed time
		elapsedTime = device->getTimer()->getTime() - startTime;
		startTime = device->getTimer()->getTime();
    
		if (botTeam->Entities.size() == 0) timeSinceBlueEradication += elapsedTime;
		if (!player || timeSinceBlueEradication > 1000) break; // All bots destroyed OR player destroyed, game over
    
		if (!paused) {   
			// Update AIManager
			aimgr->update(elapsedTime);
			// Update characters
			for (u32 i = 0 ; i < characters.size() ; ++i) {
				if (characters[i]->update(elapsedTime)) {
					delete characters[i];
					characters.erase(i);
					i--;
				}  
			}     
			if (player) {
				if (mouseDown) player->fire();
				if (player->update(elapsedTime)) {
					delete player;
					player = NULL;
				}
			}
		}
          
		// Update FPS
		s32 fps = driver->getFPS();
		if (lastFPS != fps) {
			wchar_t wstr[1024];
			swprintf(wstr, 1024, L"IrrAI v%.1f- FPS Example [%ls] fps: %d", IRRAI_VERSION, driver->getName(), fps);
			fpsText->setText(wstr);
			lastFPS = fps;
		}
        
		// Render scene
		driver->beginScene(true, true, video::SColor(255,0,0,0));
    
		smgr->drawAll();
		guienv->drawAll(); 
    
		if (player && player->getCharacterType() != ECT_SPECTATING) {
			// draw player's resource level
			s32 barWidth = 256;
			s32 barHeight = 16;
			s32 value = 0;
			f32 valueMax = 0.1f;
			video::SColor colours[4];
			core::stringc str;
			if (player->getCharacterType() == ECT_CHASING) {
				colours[0] = video::SColor(255,0,0,255);
				colours[1] = video::SColor(255,0,0,255);
				colours[2] = video::SColor(255,0,0,255);
				colours[3] = video::SColor(255,0,0,255);
				value = player->getAmmo();
				valueMax = (f32)CCharacter::MAX_AMMO;
				str = "Ammo: ";
				s32 imgDim = 64;
				driver->draw2DImage(driver->getTexture("../../Media/crosshair.png"), core::rect<s32>(400-(imgDim/2),300-(imgDim/2), 400+(imgDim/2),300+(imgDim/2)), core::rect<s32>(0,0, imgDim,imgDim), NULL, NULL, true);
			} else {
				colours[0] = video::SColor(255,0,255,0);
				colours[1] = video::SColor(255,0,255,0);
				colours[2] = video::SColor(255,0,255,0);
				colours[3] = video::SColor(255,0,255,0);
				value = player->getHealth();  
				valueMax = (f32)CCharacter::MAX_HEALTH;  
				str = "Health: ";
			}
			core::rect<s32> clip(400-(barWidth/2),(600-50)-barHeight, (400-(barWidth/2))+((s32)(barWidth*(value/valueMax))),600-50);
			driver->draw2DImage(driver->getTexture("../../Media/progress_bar.png"),
                        core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50),
                        core::rect<s32>(0,0, barWidth,barHeight),
                        &clip, colours, true);
			driver->draw2DImage(driver->getTexture("../../Media/progress_box.png"), core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50), core::rect<s32>(0,0, barWidth,barHeight), NULL, NULL, true);
			core::stringw strw = str.c_str();
			strw += value;
			guienv->getSkin()->getFont()->draw(strw.c_str(), core::rect<s32>(400-(barWidth/2),(600-50)-barHeight, 400+(barWidth/2),600-50), video::SColor(255,255,255,255), true, true);
		}
    
		driver->endScene();
        
	}
  
	if (!quitGame && device->run()) { // If the user didn't quit (with esc) and the device is still running then the only way to get here is if all J5s were destroyed OR the player died
		// display the necessary message
		core::stringw msg;
		net::SOutPacket packet;
		



		packet.compressPacket();
		netManager->sendOutPacket(packet);

		if (player) 
			if (player->getCharacterType() == ECT_SPECTATING)    msg = "Those damned blues are no more... game over!"; // player spectating so no credit
			else                                                 msg = "Those damned blues are no more... congratulations!"; // player not dead then they destroyed all J5s
		else        msg = "You died... commiserations"; // player died
  
		guienv->addMessageBox(L"Game Over", msg.c_str());
		device->getCursorControl()->setVisible(true);
		device->getTimer()->stop();
		smgr->getActiveCamera()->setInputReceiverEnabled(false);
     
		// wait for message box to be closed or esc to be pressed
		while (device->run() && !messageClosed && !quitGame) {
			driver->beginScene(true, true, video::SColor(255,0,0,0));
			smgr->drawAll();
			guienv->drawAll();
			driver->endScene(); 

			netManager->update(1000);
		}
    
		device->getTimer()->start();
	}

	
	delete netManager;
	cleanUp();

  
	// Reset fields
	quitGame = false;
	paused = false;
	messageClosed = false;

  
	return 0;
	}
}
