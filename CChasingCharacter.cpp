#include "CChasingCharacter.h"
#include "main.h"

#define SHOT_DELAY_TIME 80

static void stateChangedCallback(E_NPC_STATE_TYPE state, void* userData) {
  
	CChasingCharacter* character = (CChasingCharacter*)userData;
	if (!character) return;
  
	switch (state) {
		case ENST_WAITING:   
		case ENST_MOVING:
			character->setAnimation(state);
			break;
	}
  
}

static void eventCallback(E_NPC_EVENT_TYPE event, void* userData, void* eventData) {
       
	CChasingCharacter* character = (CChasingCharacter*)userData;
	if (!character) return;
  
	switch (event) {
		case ENET_AT_GOAL:
			character->makeNewPlan();
			break;  
		case ENET_ENEMY_VISIBLE: {
			// CHASE..
			character->chase((IAIEntity*)eventData);
			break;
		}	
		case ENET_ENEMY_IN_RANGE: {
			// ATTACK..
			character->attack((IAIEntity*)eventData);
			break;
		}
		case ENET_ENEMY_POSITION_KNOWN: {
			// HUNT...
			character->goTo((core::vector3df*)eventData);
			break;
		}
	}
      
}

CChasingCharacter::CChasingCharacter(const SCharacterDesc& desc, c8* meshPath, c8* texturePath, scene::IMetaTriangleSelector* selector) : CNPCharacter(desc, ECT_CHASING, meshPath, texturePath) {

	Chasing = false;
	EnemyVisible = false;
	TimeSinceLastShot = SHOT_DELAY_TIME + 1;
	MetaSelector = selector;
  
	if (AIEntity) {
		((INPC*)AIEntity)->setStayPut(false);
		((INPC*)AIEntity)->setStateChangedCallback(&stateChangedCallback);
		((INPC*)AIEntity)->setEventCallback(&eventCallback);
	}
  
}

CChasingCharacter::~CChasingCharacter() {

	// Remove projectiles
	for (u32 i = 0 ; i < Projectiles.size() ; ++i) 
		delete Projectiles[i];
	Projectiles.clear();
                                        
}

void CChasingCharacter::chase(IAIEntity* chasee) {
     
	if (!chasee || Chasing) return;
    
	// Run to chasee's position
	((INPC*)AIEntity)->setDestination(AIManager->getNearestWaypoint(((INPC*)AIEntity)->getWaypointGroup(), chasee->getAbsolutePosition()));
	Chasing = true;
	EnemyVisible = true;
	EnemyInteractText->setVisible(true);
  
}

void CChasingCharacter::attack(IAIEntity* attackee) {
  
	if (!attackee) return;
  
	EnemyVisible = true;
	EnemyInteractText->setVisible(true);

	((INPC*)AIEntity)->setStayPut(true);
  
	if (TimeSinceLastShot >= SHOT_DELAY_TIME && Ammo > 0){
		// Fire at chasee
		CProjectile* proj = new CProjectile(getAbsolutePosition(), (attackee->getAbsolutePosition()-getAbsolutePosition()).normalize(), SceneManager);
		if (proj) Projectiles.push_back(proj);
		TimeSinceLastShot = 0;
		Ammo--;
		TimeSinceLastRefill = 0;
	}
         
}

void CChasingCharacter::makeNewPlan() {

	// Look around for enemy?
  
	Chasing = false;
	EnemyInteractText->setVisible(false);
  
	// Pick a random waypoint to go to
	SWaypointGroup* waypointGroup = ((INPC*)AIEntity)->getWaypointGroup();
	((INPC*)AIEntity)->setDestination(rand()%waypointGroup->Waypoints.size());   
    
}

void CChasingCharacter::goTo(core::vector3df* pos) {

	if (Chasing || !pos) return;
	
	// Grab the nearest waypoint to the specified position and go there
	IWaypoint* wypt = AIManager->getNearestWaypoint(((INPC*)AIEntity)->getWaypointGroup(), *pos);
	((INPC*)AIEntity)->setDestination(wypt);
	Chasing = true;

}

bool CChasingCharacter::update(s32 elapsedTime) {

	if (CNPCharacter::update(elapsedTime)) return true;
 
	// Update projectiles and check collisions
	for (u32 p = 0 ; p < Projectiles.size() ; ++p) {
		if (Projectiles[p]->update()) {
			delete Projectiles[p];
			Projectiles.erase(p);
			p--;
		} else {
			CCharacter* enemy = NULL;
			core::line3df ray(Projectiles[p]->getPreviousPosition(), Projectiles[p]->getPosition());
			core::vector3df outVec;
			core::triangle3df outTri;
			const scene::ISceneNode* outNode;
			core::array<IAIEntity*> enemies = ((INPC*)AIEntity)->getEnemyGroup()->Entities;
			for (u32 e = 0 ; e < enemies.size() ; ++e) {
				enemy = (CCharacter*)(enemies[e]->getUserData());
				if (enemy->getAIEntity()->getNode()->getTransformedBoundingBox().intersectsWithLine(ray)) {
					enemy->registerHit();
					delete Projectiles[p];
					Projectiles.erase(p);
					p--;
					break;
				}
			}
			if (SceneManager->getSceneCollisionManager()->getCollisionPoint(ray, MetaSelector, outVec, outTri, outNode)) {
				delete Projectiles[p];
				Projectiles.erase(p);
				p--;
			} 
		}
	}
  
	TimeSinceLastShot += elapsedTime;
  
	if (!EnemyVisible) ((INPC*)AIEntity)->setStayPut(false);
  
	EnemyVisible = false;

	return false;
           
}
