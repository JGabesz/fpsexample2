#include "CFleeingCharacter.h"
#include "main.h"

/* Callback function to be informed of state changes in the NPCs
 *
 */
static void stateChangedCallback(E_NPC_STATE_TYPE state, void* userData) {
  
	CFleeingCharacter* character = (CFleeingCharacter*)userData;
	if (!character) return;
  
	switch (state) {  
		case ENST_WAITING: 
		case ENST_MOVING:
			character->setAnimation(state);
			break;
	}
  
}

static void eventCallback(E_NPC_EVENT_TYPE event, void* userData, void* eventData) {

	CFleeingCharacter* character = (CFleeingCharacter*)userData;
	if (!character) return;
  
	switch (event) {
		case ENET_AT_GOAL: 
			character->makeNewPlan();
			break;
		case ENET_ENEMY_VISIBLE:
		case ENET_ENEMY_IN_RANGE:
		case ENET_UNDER_ATTACK:
		case ENET_ENEMY_POSITION_KNOWN:
			// FLEE..
			character->setFleeing();
			break;
	}
      
}

CFleeingCharacter::CFleeingCharacter(const SCharacterDesc& desc, c8* meshPath, c8* texturePath) : CNPCharacter(desc, ECT_FLEEING, meshPath, texturePath) {

	Fleeing = false;
	Scanning = false;
  
	((INPC*)AIEntity)->setStateChangedCallback(&stateChangedCallback);
	((INPC*)AIEntity)->setEventCallback(&eventCallback);
  
	makeNewPlan();
    
}

bool CFleeingCharacter::update(s32 elapsedTime) {

	if (CNPCharacter::update(elapsedTime)) return true;

	if (Scanning) AIEntity->getNode()->setRotation(AIEntity->getNode()->getRotation() + core::vector3df(0,1,0));

	return false;

}

void CFleeingCharacter::setFleeing() {
     
	if (Fleeing) return; // maybe should alter fleeing path if an enemy is visible?
   
	// Pick a random waypoint to go to
	SWaypointGroup* wyptGroup = ((INPC*)AIEntity)->getWaypointGroup();
	((INPC*)AIEntity)->setDestination(rand()%wyptGroup->Waypoints.size());
	Fleeing = true;
	EnemyInteractText->setVisible(true);
	Scanning = false;
   
}

void CFleeingCharacter::makeNewPlan() {

	// My only plan is to watch my back!
  
	Fleeing = false;
	EnemyInteractText->setVisible(false);
  
	((INPC*)AIEntity)->setStayPut(true);
	Scanning = true;
      
}

void CFleeingCharacter::setAnimation(E_NPC_STATE_TYPE state) {

	scene::EMD2_ANIMATION_TYPE animation;
  
	switch (state) {
		case ENST_MOVING:
			if (Fleeing)        animation = scene::EMAT_CROUCH_WALK;
			else                animation = scene::EMAT_RUN;
			break;
		case ENST_WAITING:
			animation = scene::EMAT_STAND;
			break;
	}
  
	if (CurrAnimation != animation) {
		CharacterNode->setMD2Animation(animation);
		CurrAnimation = animation;
    
		/*CharacterNode->OnAnimate(0); // force update of character's bounding box for new animation, otherwise it wouldn't be updated until the next frame which is too late for us!
		// Update NPC to the current size of the character (could be crouching or standing)
		core::aabbox3d<f32> box = CharacterNode->getTransformedBoundingBox();
		AIEntity->setScale(box.MaxEdge - box.MinEdge);
		AIEntity->setOffset(core::vector3df(0, (box.MaxEdge.Y - box.MinEdge.Y)/2.0f, 0));*/
  }
           
}
