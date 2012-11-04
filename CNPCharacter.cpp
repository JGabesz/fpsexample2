#include "CNPCharacter.h"
#include "main.h"

CNPCharacter::CNPCharacter(const SCharacterDesc& desc, s32 type, c8* meshPath, c8* texturePath) : CCharacter(desc, type) {
 
	// Load up an MD2 animated mesh to use as an avatar for the NPC
	scene::IAnimatedMesh* mesh = SceneManager->getMesh(meshPath);
	if (!mesh) {
		printf("Character Mesh load failed\n");
		return;
	}
	CharacterNode = SceneManager->addAnimatedMeshSceneNode(mesh);
	if (!CharacterNode) {
		printf("Character Node creation failed\n");
		return;
	}
	CharacterNode->setMaterialFlag(video::EMF_LIGHTING, false);
	CharacterNode->setMD2Animation(scene::EMAT_STAND);
	CurrAnimation = scene::EMAT_STAND;
	CharacterNode->setMaterialTexture(0, SceneManager->getVideoDriver()->getTexture(texturePath));
  
	 // get the character's bounding box
	CharacterNode->OnAnimate(0);
	core::aabbox3d<f32> box = CharacterNode->getTransformedBoundingBox();
  
	// Give it a text scene node label for when it spots an enemy
	EnemyInteractText = SceneManager->addTextSceneNode(SceneManager->getGUIEnvironment()->getSkin()->getFont(),  L"!!! !!! !!!", video::SColor(255,255,0,0), CharacterNode);
	EnemyInteractText->setPosition(core::vector3df(0, (box.MaxEdge.Y - box.MinEdge.Y)/2.0f, 0));
	EnemyInteractText->setVisible(false);
  
	// Create the NPC    
	SCombatNPCDesc npcDesc;
	npcDesc.StartWaypointID = desc.StartWaypointID;
	npcDesc.WaypointGroupName = desc.WaypointGroupName;
	npcDesc.Range = 200.0f;
	npcDesc.UserData = this;
	npcDesc.Scale = box.MaxEdge - box.MinEdge;
	npcDesc.Offset = core::vector3df(0, (box.MaxEdge.Y - box.MinEdge.Y)/2.0f, 0);
	AIEntity = AIManager->createCombatNPC(npcDesc);
	if (!AIEntity) printf("Failed NPC creation\n");
                
}

CNPCharacter::~CNPCharacter() {
  
	if (CharacterNode) {
		CharacterNode->remove();
		CharacterNode = NULL;
	}
                              
}

bool CNPCharacter::update(s32 elapsedTime) {

	if (CCharacter::update(elapsedTime) || !((INPC*)AIEntity)) return true;

	CharacterNode->setPosition(AIEntity->getAbsolutePosition());
	CharacterNode->setRotation(AIEntity->getNode()->getRotation());
  
	Pos = CharacterNode->getAbsolutePosition();
  
	if (AIManager->isDebugVisible()) {
		if (((INPC*)AIEntity)->isVisibleToOtherEntity()) CharacterNode->setMaterialFlag(video::EMF_WIREFRAME, true);
		else                                             CharacterNode->setMaterialFlag(video::EMF_WIREFRAME, false);
	} else CharacterNode->setMaterialFlag(video::EMF_WIREFRAME, false);

	return false;
       
}

void CNPCharacter::setAnimation(E_NPC_STATE_TYPE state) {

	scene::EMD2_ANIMATION_TYPE animation;
  
	switch (state) {
		case ENST_MOVING:
			animation = scene::EMAT_RUN;
			break;
		case ENST_WAITING:
			animation = scene::EMAT_STAND;
			break;
	}
  
	if (CurrAnimation != animation) {
		CharacterNode->setMD2Animation(animation);
		CurrAnimation = animation;
	}
           
}
