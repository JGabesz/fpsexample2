#ifndef INC_IRROCCLUSIONQUERY_H
#define INC_IRROCCLUSIONQUERY_H

#include "main.h"

#ifndef IRRPHYSX_OCCLUSION

#include <irrlicht.h>

using namespace irr;

/**
\brief Sets the physical representation of the world
\param selector - The Triangle Selector for the world
*/
void setOcclusionTriangleSelector(scene::ITriangleSelector* selector);
/**
\brief Sets the scene manager of the world
\param smgr - The world's scene manager
*/
void setOcclusionSceneManager(scene::ISceneManager* smgr);
/**
\brief Performs an occusion query on the world with the specified ray and returns true if the ray intersects with the world, false otherwise
\param ray - The ray to use in the occlusion query
*/
bool occlusionQuery(const core::line3df& ray);

#endif /* !IRRPHYSX_OCCLUSION */

#endif /* INC_IRROCCLUSIONQUERY_H */
