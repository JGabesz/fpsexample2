#ifndef INC_CCHASINGCHARACTER_H
#define INC_CCHASINGCHARACTER_H

#include "CNPCharacter.h"
#include "CProjectile.h"

#include <irrlicht.h>
#include <irrAI.h>

using namespace irr;

class CChasingCharacter : public CNPCharacter {
      
	public:
		CChasingCharacter(const SCharacterDesc& desc, c8* meshPath, c8* texturePath, scene::IMetaTriangleSelector* selector);
		~CChasingCharacter();
		void chase(IAIEntity* chasee);
		void attack(IAIEntity* attackee);
		void makeNewPlan();
		void goTo(core::vector3df* pos);
		bool update(s32 elapsedTime);
    
		inline bool isChasing() { return Chasing; }
    
	private:
		bool Chasing, EnemyVisible;
		s32 TimeSinceLastShot;
		core::array<CProjectile*> Projectiles;
		scene::IMetaTriangleSelector* MetaSelector;
        
};

#endif /* INC_CCHASINGCHARACTER_H */
