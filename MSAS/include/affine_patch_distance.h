/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

/// Created on: Jan 19, 2015

#ifndef AFFINE_PATCH_DISTANCE_H_
#define AFFINE_PATCH_DISTANCE_H_

#include <memory>
#include "a_grid_patch_distance.h"

namespace msas
{

/**
 * Specific implementation of affine invariant patch distance calculator that
 * uses intermediate regular grid to compare two patches. Dominant orientations
 * (as in SIFT) are computed to determine the additional rotation between patches.
 */
class AffinePatchDistance : public AGridPatchDistance
{
public:
	AffinePatchDistance(int grid_size);
	virtual ~AffinePatchDistance() {}

	/// @copydoc AAffinePatchDistance::calculate()
	virtual DistanceInfo calculate(const StructureTensorBundle &source_bundle,
								   Point source_point,
								   const StructureTensorBundle &target_bundle,
								   Point target_point);

    void precompute_normalized_patches(const StructureTensorBundle &bundle);

private:
	inline void normalize_patch_internal(const StructureTensorBundle &bundle,
										 Point point,
										 std::vector<NormalizedPatch> &normalized_patch);
};

}	// namespace msas

#endif /* AFFINE_PATCH_DISTANCE_H_ */
