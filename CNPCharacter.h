#ifndef INC_CNPCHARACTER_H
#define INC_CNPCHARACTER_H

#include <irrlicht.h>
#include <irrAI.h>

#include "CCharacter.h"

using namespace irr;

class CNPCharacter : public CCharacter {
  
	public:
		enum E_NPCHARACTER_TYPE {
			CHASING_CHARACTER,
			FLEEING_CHARACTER     
		};
        
		CNPCharacter(const SCharacterDesc& desc, s32 type, c8* meshPath, c8* texturePath);
		virtual ~CNPCharacter();
		virtual bool update(s32 elapsedTime);
		virtual void setAnimation(E_NPC_STATE_TYPE state);
        
		inline scene::IAnimatedMeshSceneNode* getNode() { return CharacterNode; }
		inline void registerHit() {
			CCharacter::registerHit();
			((INPC*)AIEntity)->sendEventToNPC(ENET_UNDER_ATTACK, NULL);
		}
    
	protected:
		scene::IAnimatedMeshSceneNode* CharacterNode;
		scene::ITextSceneNode* EnemyInteractText;
		scene::EMD2_ANIMATION_TYPE CurrAnimation;    
                    
};

#endif /* INC_CNPCHARACTER_H */
