/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

#include "affine_patch_distance.h"

using std::vector;

namespace msas
{

AffinePatchDistance::AffinePatchDistance(int grid_size)
: AGridPatchDistance(grid_size)
{

}


msas::DistanceInfo AffinePatchDistance::calculate(const msas::StructureTensorBundle &source_bundle,
												  Point source_point,
												  const msas::StructureTensorBundle &target_bundle,
												  Point target_point)
{
	// Get normalized patches

	vector<NormalizedPatch> *normalized_source = (_use_cache) ?
												 source_bundle.normalized_patch(source_point.x, source_point.y) :
												 new vector<NormalizedPatch>();

	if (!normalized_source->size()) {
        normalize_patch_internal(source_bundle, source_point, *normalized_source);
	}

	vector<NormalizedPatch> *normalized_target = (_use_cache) ?
												 target_bundle.normalized_patch(target_point.x, target_point.y) :
												 new vector<NormalizedPatch>();

	if (!normalized_target->size()) {
		normalize_patch_internal(target_bundle, target_point, *normalized_target);
	}

	// Find minimum distance among all possible combinations of orientations
	int number_of_channels = target_bundle.image().number_of_channels();
	float target_radius = target_bundle.radius();
	int target_id = -2;
	int source_id = -2;
	msas::DistanceInfo min_distance;
	if (_use_bilateral) {
		min_distance.distance = calculate_geodesic(*normalized_source, *normalized_target, target_radius, number_of_channels, source_id, target_id);
	} else {
		min_distance.distance = calculate_gaussian(*normalized_source, *normalized_target, target_radius, number_of_channels, source_id, target_id);
	}

	// Fill min_distance
	min_distance.first_point = source_point;
	min_distance.second_point = target_point;
	min_distance.first_transform = (*normalized_source)[source_id].extra_transform *
								   (*normalized_source)[source_id].base_transform;
	min_distance.second_transform = (*normalized_target)[target_id].extra_transform *
									(*normalized_target)[target_id].base_transform;

	if (!_use_cache) {
		delete normalized_source;
		delete normalized_target;
	}

	return min_distance;
}


void AffinePatchDistance::precompute_normalized_patches(const StructureTensorBundle &bundle)
{
	if (!_use_cache) {
		return;
	}

	#pragma omp parallel for schedule(dynamic,1) collapse(2) shared(bundle)
	for (uint y = 0; y < bundle.size_y(); y++) {
		for (uint x = 0; x < bundle.size_x(); x++) {
			Point point(x, y);
			vector<NormalizedPatch> *normalized_patch = bundle.normalized_patch(point.x, point.y);
			normalize_patch_internal(bundle, point, *normalized_patch);
		}
	}
}

/* Private */

inline void AffinePatchDistance::normalize_patch_internal(const StructureTensorBundle &bundle,
														  Point point,
														  std::vector<NormalizedPatch> &normalized_patch)
{
	// Compute dominant orientations
	Eigen::Matrix2f transformation = bundle.transform(point);
	vector<Point> region = bundle.region(point);
	vector<float> dominant_orientations = _normalization.calculate_dominant_orientations(bundle.gradient_x(),
																						 bundle.gradient_y(),
																						 region,
																						 transformation,
																						 point);

	// For every dominant orientation compute its corresponding patch normalization
	for (auto it = dominant_orientations.begin(); it != dominant_orientations.end(); ++it) {
		Eigen::Matrix2f rotation = _normalization.rotation(*it);
		std::shared_ptr<float*> normalization = _normalization.interpolate_to_grid(*_grid,
																				   bundle.image(),
																				   bundle.mask(),
																				   rotation * transformation,
																				   point);
		normalized_patch.push_back(NormalizedPatch(normalization, transformation, rotation));
	}
}

}	// namespace msas
