#ifndef SPINDULYS_OBBOX_H
#define SPINDULYS_OBBOX_H

#include <cstddef>
#include <assert.h>
#include <iostream>

#include "../../spindulys.h"
#include "../platform.h"

#include "constants.h"
#include "bbox.h"
#include "linearspace3.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

/*! Oriented bounding box */
template<typename T>
struct OBBox
{
	public:

		__forceinline OBBox () {}

		__forceinline OBBox (EmptyTy)
			: space(one), bounds(empty) {}

		__forceinline OBBox (const BBox<T>& bounds)
			: space(one), bounds(bounds) {}

		__forceinline OBBox (const LinearSpace3<T>& space, const BBox<T>& bounds)
			: space(space), bounds(bounds) {}

		friend std::ostream& operator<<(std::ostream& cout, const OBBox& p) {
			return std::cout << "{ space = " << p.space << ", bounds = " << p.bounds << "}";
		}

	public:
		LinearSpace3<T> space; //!< orthonormal transformation
		BBox<T> bounds;        //!< bounds in transformed space
};

typedef OBBox<Vec3f> OBBox3f;

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif //SPINDULYS_OBBOX_H
