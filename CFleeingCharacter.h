#ifndef INC_CFLEEINGCHARACTER_H
#define INC_CFLEEINGCHARACTER_H

#include "CNPCharacter.h"
#include <irrlicht.h>
#include <irrAI.h>

using namespace irr;

class CFleeingCharacter : public CNPCharacter {
      
	public:
		CFleeingCharacter(const SCharacterDesc& desc, c8* meshPath, c8* texturePath);
		bool update(s32 elapsedTime);
		void setFleeing();
		void makeNewPlan();   
		void setAnimation(E_NPC_STATE_TYPE state);
    
		inline bool isFleeing() { return Fleeing; };
    
  private:
		bool Fleeing;
		bool Scanning;
     
};

#endif /* INC_CFLEEINGCHARACTER_H */
