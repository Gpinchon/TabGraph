/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-15 13:55:26
*/

#pragma once

#include "BoundingElement.hpp"

/** @brief BoundingElement class

	This class provides the Axis Aligned Bounding Box bounding element
	@author: G. Pinchon
*/
class AABB : public BoundingElement {
public:
    /** Default constructor.
	*/
    AABB();
    /** Returns true if a collision if found with the provided BoundingElement
    */
    bool collides(const BoundingElement&);
};
