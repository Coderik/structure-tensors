/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

#include "a_grid_patch_distance.h"

namespace msas
{

using std::vector;

AGridPatchDistance::AGridPatchDistance(int grid_size)
: AAffinePatchDistance(),
  _grid_size(grid_size),
  _reference_channel(-1),
  _use_cache(true)
{
	_grid = _normalization.create_regular_grid(_grid_size);
	_weights.reset(calculate_weights(_grid.get(), _scale));
}


AGridPatchDistance::~AGridPatchDistance()
{

}


std::shared_ptr<GridInfo> AGridPatchDistance::grid()
{
	return _grid;
}


size_t AGridPatchDistance::normalized_patch_length()
{
	return _grid->nodes_length;
}


int AGridPatchDistance::grid_size()
{
	return _grid_size;
}


void AGridPatchDistance::set_grid_size(int value)
{
	if (_grid_size != value) {
		_grid_size = value;

		// Recompute grid
		_grid = _normalization.create_regular_grid(_grid_size);

		// Recompute weights
		_weights.reset(calculate_weights(_grid.get(), _scale));
	}
}


int AGridPatchDistance::reference_channel() const
{
	return _reference_channel;
}


void AGridPatchDistance::set_reference_channel(int value)
{
	_reference_channel = value;
}


void AGridPatchDistance::set_use_cache(bool value)
{
	_use_cache = value;
}


/* Protected */

/**
 * Calculate patch distance using Gaussian weights.
 * @param normalized_source Set of candidate normalizations of the source patch.
 * @param normalized_target Set of candidate normalizations of the target patch.
 * @param source_id [out] Id of the candidate source normalization that gives the smallest distance.
 * @param target_id [out] Id of the candidate target normalization that gives the smallest distance.
 */
float AGridPatchDistance::calculate_gaussian(const vector<NormalizedPatch> &normalized_source,
											 const vector<NormalizedPatch> &normalized_target,
											 float radius,
											 int number_of_channels,
											 int &source_id,
											 int &target_id)
{
	int number_of_channels_used = (_reference_channel < 0 || _reference_channel >= number_of_channels)
								  ? number_of_channels : 1;
	double min_distance = std::numeric_limits<float>::max();
	target_id = -2;
	source_id = -2;

	// Calculate distance values for every combination of source and target normalizations
	for (uint i = 0; i < normalized_target.size(); i++) {
		for (uint j = 0; j < normalized_source.size(); j++) {
			float** target_patch = normalized_target[i].patch.get();
			float** source_patch = normalized_source[j].patch.get();

			double distance = 0.0;
			double total_weight = 0.0;

			// Calculate distances using either all channels or only the reference one, if it is specified
			if (_reference_channel < 0 || _reference_channel >= number_of_channels) {
				for (int k = 0; k < _grid->nodes_length; k++) {
					if (source_patch[0][k] < -256.0f ||
						target_patch[0][k] < -256.0f) {    // we cannot compare points, if at least one of them is unknown
						continue;
					}

					// Calculate color difference at k-th node
					double color_distance = 0.0;
					for (int ch = 0; ch < number_of_channels; ch++) {
						color_distance += (source_patch[ch][k] - target_patch[ch][k]) *
										  (source_patch[ch][k] - target_patch[ch][k]);
					}

					distance += _weights[k] * color_distance;
					total_weight += _weights[k];
				}
			} else {
				for (int k = 0; k < _grid->nodes_length; k++) {
					if (source_patch[0][k] < -256.0f ||
						target_patch[0][k] < -256.0f) {    // we cannot compare points, if at least one of them is unknown
						continue;
					}

					// Calculate color difference at k-th node
					double color_distance = (source_patch[_reference_channel][k] - target_patch[_reference_channel][k]) *
											(source_patch[_reference_channel][k] - target_patch[_reference_channel][k]);

					distance += _weights[k] * color_distance;
					total_weight += _weights[k];
				}
			}

			// Normalize
			if (total_weight > 0) {
				distance /= ((double)number_of_channels_used * total_weight);
			} else {
				distance = std::numeric_limits<float>::max();
			}

			// Select the smallest distance and keep its corresponding configuration of patches
			if (distance < min_distance) {
				min_distance = (float)distance;
				target_id = i;
				source_id = j;
			}
		}
	}

	return (float)min_distance;
}


/**
 * Calculate patch distance using approximated geodesic weights.
 * @param normalized_source Set of candidate normalizations of the source patch.
 * @param normalized_target Set of candidate normalizations of the target patch.
 * @param source_id [out] Id of the candidate source normalization that gives the smallest distance.
 * @param target_id [out] Id of the candidate target normalization that gives the smallest distance.
 */
float AGridPatchDistance::calculate_geodesic(const vector<NormalizedPatch> &normalized_source,
											 const vector<NormalizedPatch> &normalized_target,
											 float radius,
											 int number_of_channels,
											 int &source_id,
											 int &target_id)
{
	float **target_patch = normalized_target[0].patch.get();
	float **source_patch = normalized_source[0].patch.get();

	// Compute color component for bilateral weights (geodesic weights approximation)
	std::unique_ptr<float[]> central_color;
	if (_reference_channel < 0 || _reference_channel >= number_of_channels) {
		central_color.reset(new float[number_of_channels]);
		for (int ch = 0; ch < number_of_channels; ch++) {
			central_color[ch] = target_patch[ch][_grid->nodes_length / 2];
		}
	} else {
		central_color.reset(new float[1]);
		central_color[0] = target_patch[_reference_channel][_grid->nodes_length / 2];
	}
	float color_k = _bilateral_k_color / (2.0f * (radius / _scale) * (radius / _scale));

	int number_of_channels_used = (_reference_channel < 0 || _reference_channel >= number_of_channels)
								  ? number_of_channels : 1;
	double min_distance = std::numeric_limits<float>::max();
	target_id = -2;
	source_id = -2;

	for (uint i = 0; i < normalized_target.size(); i++) {
		for (uint j = 0; j < normalized_source.size(); j++) {
			target_patch = normalized_target[i].patch.get();
			source_patch = normalized_source[j].patch.get();

			double distance = 0.0;
			double total_weight = 0.0;

			// Calculate distances using either all channels or only the reference one, if it is specified
			if (_reference_channel < 0 || _reference_channel >= number_of_channels) {
				for (int k = 0; k < _grid->nodes_length; k++) {
					if (source_patch[0][k] < -256.0f ||
						target_patch[0][k] <
						-256.0f) {    // we cannot compare points, if at least one of them is unknown
						continue;
					}

					// Calculate color difference at k-th node
					double color_distance = 0.0;
					for (int ch = 0; ch < number_of_channels; ch++) {
						color_distance += (source_patch[ch][k] - target_patch[ch][k]) *
										  (source_patch[ch][k] - target_patch[ch][k]);
					}

					// Calculate color weight
					double central_distance = 0.0;
					for (int ch = 0; ch < number_of_channels; ch++) {
						central_distance += (central_color[ch] - target_patch[ch][k]) *
											(central_color[ch] - target_patch[ch][k]);
					}
					double color_weight = LUT::exp_rcn(-color_k * central_distance);

					distance += color_weight * _weights[k] * color_distance;
					total_weight += color_weight * _weights[k];
				}
			} else {
				for (int k = 0; k < _grid->nodes_length; k++) {
					if (source_patch[0][k] < -256.0f ||
						target_patch[0][k] <
						-256.0f) {    // we cannot compare points, if at least one of them is unknown
						continue;
					}

					// Calculate color difference at k-th node
					double color_distance = (source_patch[_reference_channel][k] - target_patch[_reference_channel][k]) *
											(source_patch[_reference_channel][k] - target_patch[_reference_channel][k]);

					// Calculate color weight
					double central_distance = (central_color[0] - target_patch[_reference_channel][k]) *
											  (central_color[0] - target_patch[_reference_channel][k]);
					double color_weight = LUT::exp_rcn(-color_k * central_distance);

					distance += color_weight * _weights[k] * color_distance;
					total_weight += color_weight * _weights[k];
				}
			}

			// Normalize
			if (total_weight > 0) {
				distance /= ((double) number_of_channels_used * total_weight);
			} else {
				distance = std::numeric_limits<float>::max();
			}

			// Select the smallest distance and keep its corresponding configuration of patches
			if (distance < min_distance) {
				min_distance = (float) distance;
				target_id = i;
				source_id = j;
			}
		}
	}

	return (float)min_distance;
}


void AGridPatchDistance::update_weights()
{
	// Recompute weights
	_weights.reset(calculate_weights(_grid.get(), _scale));
}


/**
 * @param sigma_factor - number of sigmas we want to be fitted within the radius
 */
float* AGridPatchDistance::calculate_weights(const GridInfo *grid, float sigma_factor)
{
    // NOTE: since transformations are normalized by the radius, we transform ellipses to unit circles.
    const float radius = 1.0f;

	float *weights = new float[grid->nodes_length];
	float sigma_squared = 2.0f * (radius / sigma_factor) * (radius / sigma_factor) / _bilateral_k_spatial;

	for (int i = 0; i < grid->nodes_length; i++) {
		float weight = exp(-(grid->nodes[i].x * grid->nodes[i].x + grid->nodes[i].y * grid->nodes[i].y) / sigma_squared);
		weights[i] = weight;
	}

	return weights;
}

}	// namespace msas
