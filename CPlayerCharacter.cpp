#include "CPlayerCharacter.h"

#define SHOT_DELAY_TIME 80

/* Character class constructor. Creates an NPC AIEntity and an avatar to represent it nicely
 *
 * Params:
 * ISceneManager*: pointer to the ISceneManager to add the avatar to
 * irrAIManager*: pointer to the irrAIManager that the NPC AIEntity is linked to
 * s32: id for the NPC AIEntity & avatar to be given
 * s32: id of the waypoint to start at
 * IMetaTriangleSelector*: meta selector of the world for collision detection
 */
CPlayerCharacter::CPlayerCharacter(const SCharacterDesc& desc, scene::ICameraSceneNode* camera, scene::IMetaTriangleSelector* selector, s32 type) : CCharacter(desc, type) {
  
	Camera = camera;
	TimeSinceLastShot = SHOT_DELAY_TIME;
	MetaSelector = selector;
  
	if (type != ECT_SPECTATING) {
		// Create the PAIE  
		SAIEntityDesc pDesc;
		pDesc.UserData = this;
		pDesc.Scale = core::vector3df(15,60,15);
		pDesc.Offset = core::vector3df(0,-30,0);
		pDesc.Name = "PLYR";
		AIEntity = AIManager->createPlayerAIEntity(pDesc);
		if (!AIEntity) printf("Failed PAIE creation\n"); 
	}
  
	// Load up an MD2 animated mesh to use as an avatar for the NPC
	switch (type) {
		case ECT_CHASING: {
			scene::IAnimatedMesh* mesh = SceneManager->getMesh("../../Media/blaster.3ds");
			if (!mesh) {
				printf("Gun Mesh load failed\n");
				return;
			}
			GunNode = SceneManager->addAnimatedMeshSceneNode(mesh);
			if (!GunNode) {
				printf("Gun Node creation failed\n");
				return;
			}
			if (Camera) GunNode->setParent(Camera);
			GunNode->setScale(core::vector3df(0.25f,0.25f,0.25f));
			GunNode->setPosition(core::vector3df(4.5f,-5,9));
			GunNode->setRotation(core::vector3df(0,-90,0));
			GunNode->setMaterialFlag(video::EMF_LIGHTING, false);
			break;
		}
		case ECT_FLEEING:
		case ECT_SPECTATING:
			// No gun node needed
			GunNode = NULL;
			break;
	}
                     
}

CPlayerCharacter::~CPlayerCharacter() {
  
	if (GunNode) {
		GunNode->remove();
		GunNode = NULL;
	}
  
	// Remove projectiles
	core::list<CProjectile*>::Iterator iter = Projectiles.begin();
	while (iter != Projectiles.end()) {
		delete *iter;
		iter++;
	}
	Projectiles.clear();
                           
}

bool CPlayerCharacter::update(s32 elapsedTime) {
       
	if (CCharacter::update(elapsedTime)) return true;
  
	if (AIEntity) {
		// tell the paie where we are now
		core::vector3df pos = Camera->getAbsolutePosition();
		AIEntity->setPosition(pos);
		ActPosition = Camera->getAbsolutePosition();
	}
   
	// Update projectiles and check collisions
	core::list<CProjectile*>::Iterator projIter = Projectiles.begin();
	while (projIter != Projectiles.end()) {
		if ((*projIter)->update()) {
			delete *projIter;
			projIter = Projectiles.erase(projIter);
		} else {
			CCharacter* enemy = NULL;
			bool collision = false;
			core::line3df ray((*projIter)->getPreviousPosition(), (*projIter)->getPosition());
			core::vector3df outVec;
			core::triangle3df outTri;
			const scene::ISceneNode* outNode;
			core::array<IAIEntity*> enemies = ((IPlayerAIEntity*)AIEntity)->getEnemyGroup()->Entities;
			for (u32 i = 0 ; i < enemies.size() ; ++i) {
				enemy = (CCharacter*)(enemies[i]->getUserData());
				if (enemy->getAIEntity()->getNode()->getTransformedBoundingBox().intersectsWithLine(ray)) {
					enemy->registerHit();
					collision = true;
					delete *projIter;
					projIter = Projectiles.erase(projIter);
					break;
				}
			}
			if (SceneManager->getSceneCollisionManager()->getCollisionPoint(ray, MetaSelector, outVec, outTri, outNode)) {
				collision = true;
				delete *projIter;
				projIter = Projectiles.erase(projIter);
			}
			if (!collision) projIter++;   
		}
	}
  
	TimeSinceLastShot += elapsedTime;
     
	return false;
  
}

void CPlayerCharacter::fire() {
  
	if (CharacterType == ECT_CHASING && TimeSinceLastShot >= SHOT_DELAY_TIME && Ammo > 0) {
		// Calculate the position to fire from
		core::vector3df offset = core::vector3df(6.5f,-6.5f,30);
		core::matrix4 mat = Camera->getAbsoluteTransformation();
		mat.transformVect(offset); 
		// Fire projectile
		CProjectile* proj = new CProjectile(offset, (Camera->getTarget()-Camera->getAbsolutePosition()).normalize(), SceneManager);
		if (proj) Projectiles.push_back(proj);
		TimeSinceLastShot = 0;
		Ammo--;
		TimeSinceLastRefill = 0;
	}
  
}
