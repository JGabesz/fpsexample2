#include "main.h"
#include "IrrPhysxOcclusionQuery.h"

#ifdef IRRPHYSX_OCCLUSION

#include <IrrPhysx.h>

using namespace IrrPhysx;

IPhysxManager* physxManager = NULL;

bool occlusionQuery(const core::line3df& ray) {
 
	if (physxManager) { 
		const core::array<SRaycastHitData>& objects = physxManager->raycastAllRigidObjects(ray);
		if (objects.size() > 0)
			return true;  
	}
      
	return false;
         
}

void recurseScene(IPhysxManager* physxManager, IrrlichtDevice* device, scene::ISceneNode* node) {

	if (!node) return;

	scene::ISceneManager* smgr = device->getSceneManager();
    
	if (node != smgr->getRootSceneNode()) {
        
		switch (node->getType()) {
      
			// Create box physx objects from these nodes
			case scene::ESNT_CUBE: {
				io::IAttributes* attribs = device->getFileSystem()->createEmptyAttributes(); 
				if (attribs)  {
					// Get the cube size
					node->serializeAttributes(attribs); 
					f32 size = attribs->getAttributeAsFloat("Size"); 
					attribs->drop();
					physxManager->createBoxObject(node->getAbsolutePosition(), node->getRotation(), size * node->getScale()/2.0f, 0.0f);
				}
				break;
			}
			
		    // Ignore all other nodes
			default:
				break;    
		}
    
	}
  
	// recurse through the children
	core::list<scene::ISceneNode*> children = node->getChildren();
	core::list<scene::ISceneNode*>::Iterator iter = children.begin();
	while (iter != children.end()) {
		recurseScene(physxManager, device, *iter);
		++iter;      
	}
  
}

void readScene(IrrlichtDevice* device) {

	// Set some Physx scene description parameters to optimise raycasting, see the documentation for SSceneDesc to understand these parameters
	// Set the 'bounding box' of the Physx scene to be 1000x1000x1000 (enough to enclose our scene nicely)
	SSceneDesc sceneDesc;
	sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-500,-500,-500), core::vector3df(500,500,500));
	// We want to create planes for the bounding box so objects won't go outside the bounds
	sceneDesc.BoundPlanes = true;
	// Our up axis is the Y axis
	sceneDesc.UpAxis = 1;
	// Set the gravity to a bit more than the default
	sceneDesc.Gravity = core::vector3df(0,-60,0);
	// Create our Physx manager which will help us deal with all the physics stuff
	physxManager = createPhysxManager(device, sceneDesc);

	recurseScene(physxManager, device, device->getSceneManager()->getRootSceneNode());
       
}

#endif /* IRRPHYSX_OCCLUSION */
