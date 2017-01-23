/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

/// Created on: Sep 17, 2014

#ifndef A_GRID_PATCH_DISTANCE_H_
#define A_GRID_PATCH_DISTANCE_H_

#include "a_affine_patch_distance.h"
#include "ellipse_normalization.h"

namespace msas
{

/**
 * Abstract class representing any calculator of affine invariant patch distance that
 * uses intermediate regular grid to compare two patches.
 */
class AGridPatchDistance : public AAffinePatchDistance
{
public:
	AGridPatchDistance(int grid_size);
	virtual ~AGridPatchDistance();

	/// @copydoc AAffinePatchDistance::calculate()
	virtual DistanceInfo calculate(const StructureTensorBundle &source_bundle,
								   Point source_point,
								   const StructureTensorBundle &target_bundle,
								   Point target_point) = 0;

	/// Get the regular grid that is used in distance computation.
	std::shared_ptr<GridInfo> grid();

	/// Get number of points in a normalized patch that is used in distance computation.
	size_t normalized_patch_length();

	/// Get size (resolution) of the regular grid.
	int grid_size();

	/// Set size (resolution) of the regular grid.
	/// @note Causes recreation of the grid.
	void set_grid_size(int value);

	/// Get id of the reference channel.
	int reference_channel() const;

	/// Set id of a channel to be used to calculate distance between two patches. By default set to -1.
	/// If the value is out of range (e.g. -1), then all channel are used.
	void set_reference_channel(int value);

	/// Specify whether normalized patches should be cached or not (true by default).
	void set_use_cache(bool value);

protected:
	EllipseNormalization _normalization;
	std::shared_ptr<GridInfo> _grid;	// Note: we normalize patches to unit circles, so no need for two grids
	int _grid_size;
	std::unique_ptr<float[]> _weights;
	int _reference_channel;
	bool _use_cache;

	float calculate_gaussian(const std::vector<NormalizedPatch> &normalized_source,
							 const std::vector<NormalizedPatch> &normalized_target,
							 float radius,
							 int number_of_channels,
							 int &source_id,
							 int &target_id);

	float calculate_geodesic(const std::vector<NormalizedPatch> &normalized_source,
							 const std::vector<NormalizedPatch> &normalized_target,
							 float radius,
							 int number_of_channels,
							 int &source_id,
							 int &target_id);

	virtual void update_weights();

	float* calculate_weights(const GridInfo *grid, float sigma_factor);
};

}	// namespace msas


#endif /* A_GRID_PATCH_DISTANCE_H_ */
