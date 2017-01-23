/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

#ifndef IO_HELPERS_H_H
#define IO_HELPERS_H_H

#include <string>
#include <fstream>
#include "Eigen/Core"

namespace iohelpers {

using std::vector;
using std::string;

struct TransformInfo
{
	int x;
	int y;
	Eigen::Matrix2f transform;
};

vector<Point> read_points(string filename)
{
	if (filename.empty()) {
		return vector<Point>();
	}

	std::ifstream file;
	file.open(filename.c_str(), std::ios_base::in);

	if (!file.is_open()) {
		return vector<Point>();
	}

	vector<Point> points;

	string line;
	while (std::getline(file, line)) {
		std::stringstream str_stream(line);
		int x, y;
		if (str_stream >> x >> y) {
			points.push_back(Point(x, y));
		}
	}

	file.close();

	return points;
}


void save_tensors(string filename, const Image<Eigen::Matrix2f> &tensors)
{
	std::ofstream file;
	file.open(filename.c_str(), std::ios_base::out);

	if (!file.is_open()) {
		return;
	}

	for (uint y = 0; y < tensors.size_y(); y++) {
		for (uint x = 0; x < tensors.size_x(); x++) {
			file << tensors(x, y)(0, 0) << " ";
		}
		file << "  ";
		for (uint x = 0; x < tensors.size_x(); x++) {
			file << tensors(x, y)(0, 1) << " ";
		}
		file << "  ";
		for (uint x = 0; x < tensors.size_x(); x++) {
			file << tensors(x, y)(1, 0) << " ";
		}
		file << "  ";
		for (uint x = 0; x < tensors.size_x(); x++) {
			file << tensors(x, y)(1, 1) << " ";
		}
		file << std::endl;
	}

	file.close();
}


void save_floats(string filename, const Image<float> &floats)
{
	std::ofstream file;
	file.open(filename.c_str(), std::ios_base::out);

	if (!file.is_open()) {
		return;
	}

	for (uint y = 0; y < floats.size_y(); y++) {
		for (uint x = 0; x < floats.size_x(); x++) {
			file << floats(x, y) << " ";
		}
		file << std::endl;
	}

	file.close();
}


/**
 * Saves transforms. Each transform has 6 components. 1-2 are the coordinates T=[x;y] of
 * the center. 3-6 is the column-wise stacking of a 2x2 transform matrix A.
 */
void save_transforms(string filename, const vector<TransformInfo> &transforms)
{
	std::ofstream file;
	file.open(filename.c_str(), std::ios_base::out);

	if (!file.is_open()) {
		return;
	}

	for (auto it = transforms.begin(); it != transforms.end(); ++it) {
		file << it->x << " " << it->y << " " << (float)it->transform(0, 0) << " " << (float)it->transform(1, 0) << " " << (float)it->transform(0, 1) << " " << (float)it->transform(1, 1) << std::endl;
	}

	file.close();
}

}	// namespace iohelpers

#endif //IO_HELPERS_H_H
