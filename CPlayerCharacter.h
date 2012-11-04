#ifndef INC_CPLAYERCHARACTER_H
#define INC_CPLAYERCHARACTER_H

#include <irrlicht.h>
#include <irrAI.h>

#include "CCharacter.h"
#include "CProjectile.h"
#include "main.h"

using namespace irr;

class CPlayerCharacter : public CCharacter {
  
	public:
		CPlayerCharacter(const SCharacterDesc& desc, scene::ICameraSceneNode* camera, scene::IMetaTriangleSelector* selector, s32 type = ECT_SPECTATING);
		~CPlayerCharacter();
		bool update(s32 elapsedTime);
		void fire();
		core::vector3df ActPosition;
    
		inline scene::IAnimatedMeshSceneNode* getNode() { return GunNode; }
    
	private:
		scene::ICameraSceneNode* Camera;
		scene::IAnimatedMeshSceneNode* GunNode;
		s32 TimeSinceLastShot;
		core::list<CProjectile*> Projectiles;
		scene::IMetaTriangleSelector* MetaSelector;

                    
};

#endif /* INC_CPLAYERCHARACTER_H */
