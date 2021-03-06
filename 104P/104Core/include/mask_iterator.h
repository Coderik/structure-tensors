/**
 * Copyright (C) 2016, Vadim Fedorov <coderiks@gmail.com>
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the simplified BSD
 * License. You should have received a copy of this license along
 * this program. If not, see
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

#ifndef MASK_ITERATOR_H_
#define MASK_ITERATOR_H_

#include <memory>
#include "i_iterable_mask.h"

/**
 * STL-like bidirectional input (constant) iterator for masked points.
 */
class MaskIterator
{ 
public:
	typedef std::allocator<Point>::const_reference	const_reference;
	typedef std::allocator<Point>::const_pointer	const_pointer;

	MaskIterator();
	MaskIterator(const IIterableMask *mask, Point current, bool reverse = false);
	MaskIterator(const MaskIterator& source);
	~MaskIterator();

	MaskIterator& operator=(const MaskIterator& source);
	bool operator==(const MaskIterator& other) const;
	bool operator!=(const MaskIterator& other) const;

	MaskIterator& operator++();
	MaskIterator operator++(int);
	MaskIterator& operator--();
	MaskIterator operator--(int);

	const const_reference operator*() const;
	const const_pointer operator->() const;

private:
	const IIterableMask *_mask;
	Point _current;
	bool _is_reverse;
};


 #endif /* MASK_ITERATOR_H_ */
