/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

/// Created on: Oct 21, 2013

#ifndef A_AFFINE_PATCH_DISTANCE_H_
#define A_AFFINE_PATCH_DISTANCE_H_

#include "distance_info.h"
#include "structure_tensor_bundle.h"
#include "point.h"

namespace msas
{

/**
 * Abstract class representing any calculator of affine invariant patch distance.
 */
class AAffinePatchDistance
{
public:
	AAffinePatchDistance()
		: _scale(1.0f), _bilateral_k_color(0.0f), _bilateral_k_spatial(1.0f), _use_bilateral(false) {}

	virtual ~AAffinePatchDistance() {}

	/// Compute patch distance between two given points.
	/// @param source_bundle Bundle embedding the first image and its corresponding structure tensor field.
	/// @param source_point Point of interest in the first image.
	/// @param target_bundle Bundle embedding the second image and its corresponding structure tensor field.
	/// @param target_point Point of interest in the second image.
	/// @note @param source_bundle and @param target_bundle may coincide.
	virtual DistanceInfo calculate(const StructureTensorBundle &source_bundle,
								   Point source_point,
								   const StructureTensorBundle &target_bundle,
								   Point target_point) = 0;

	/// Get scale parameter (relative scale w.r.t. the radius)
	float scale()
	{
		return _scale;
	}

	/// Set scale parameter (relative scale w.r.t. the radius)
	void set_scale(float value)
	{
		_scale = value;
		update_weights();
	}

	/// Get kappa-color for bilateral weights
	float bilateral_k_color()
	{
		return _bilateral_k_color;
	}

	/// Set kappa-color for bilateral weights
	void set_bilateral_k_color(float value)
	{
		_bilateral_k_color = value;
		_use_bilateral = std::abs(_bilateral_k_color) > EPS;
	}

	/// Get kappa-spatial for bilateral weights
	float bilateral_k_spatial()
	{
		return _bilateral_k_spatial;
	}

	/// Set kappa-spatial for bilateral weights
	void set_bilateral_k_spatial(float value)
	{
		_bilateral_k_spatial = value;
		update_weights();
	}

protected:
	static constexpr float EPS = 0.0001f;
	float _scale;
	float _bilateral_k_color;
	float _bilateral_k_spatial;
	bool _use_bilateral;

	virtual void update_weights() = 0;
};

}	// namespace msas

#endif /* A_AFFINE_PATCH_DISTANCE_H_ */
