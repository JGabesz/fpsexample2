#ifndef INC_IRRPHYSXOCCLUSIONQUERY_H
#define INC_IRRPHYSXOCCLUSIONQUERY_H

#include "main.h"

#ifdef IRRPHYSX_OCCLUSION

#include <irrlicht.h>

using namespace irr;

/**
\brief Walks the scene graph to build up a physical version of the world
\param device - The device controlling the scene
*/
void readScene(IrrlichtDevice* device);
/**
\brief Performs an occusion query on the world with the specified ray and returns true if the ray intersects with the world, false otherwise
\param ray - The ray to use in the occlusion query
*/
bool occlusionQuery(const core::line3df& ray);

#endif /* IRRPHYSX_OCCLUSION */

#endif /* INC_IRRPHYSXOCCLUSIONQUERY_H */
