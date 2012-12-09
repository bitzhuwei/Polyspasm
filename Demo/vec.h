/**
 * Copyright (c) 2009-2012 Jesper Öqvist <jesper@llbit.se>
 *
 * This file is part of Polyspasm.
 *
 * Polyspasm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Polyspasm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyspasm; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef CTTF_VEC
#define CTTF_VEC

template <class _t>
struct vec2 {

	_t x, y;

	vec2<_t>() {}
	vec2<_t>(_t x, _t y) {this->x = x;this->y = y;}
	vec2<_t>(const vec2<_t>& _a) {*this = _a;}

	_t dot_product(const vec2<_t>& _a) {
		return x * _a.x + y * _a.y;
	}

	_t length() const {
		return sqrt(x*x + y*y);
	}

	_t square_length() const {
		return (x*x + y*y);
	}

	void normalize() {
		*this = (*this) / length();
	}

	vec2<_t> operator*(const vec2<_t>& _a) const {
		vec2<_t> _b;
		_b.x = x * _a.x;
		_b.y = y * _a.y;
		return _b;
	}

	vec2<_t> operator*(const _t& _a) const {
		vec2<_t> _b;
		_b.x = x * _a;
		_b.y = y * _a;
		return _b;
	}
	vec2<_t> operator/(const _t& _a) const {
		vec2<_t> _b;
		_b.x = x / _a;
		_b.y = y / _a;
		return _b;
	}

	vec2<_t> operator+(const vec2<_t>& _a) const {
		vec2<_t> _b;
		_b.x = x + _a.x;
		_b.y = y + _a.y;
		return _b;
	}

	vec2<_t> operator-(const vec2<_t>& _a) const {
		vec2<_t> _b;
		_b.x = x - _a.x;
		_b.y = y - _a.y;
		return _b;
	}

	const vec2<_t>& operator=(const vec2<_t>& _a) {
		x = _a.x;
		y = _a.y;
		return *this;
	}

	const vec2<_t>& operator+=(const vec2<_t>& _a) {
		x += _a.x;
		y += _a.y;
		return *this;
	}

	const vec2<_t>& operator-=(const vec2<_t>& _a) {
		x -= _a.x;
		y -= _a.y;
		return *this;
	}

	const vec2<_t>& operator*=(const _t& _a) {
		x *= _a;
		y *= _a;
		return *this;
	}

	const vec2<_t>& operator/=(const _t& _a) {
		x /= _a;
		y /= _a;
		return *this;
	}

	bool operator==(const vec2<_t>& _a) const {
		return ((_a.x == x) && (_a.y == y));
	}
};

template <class _t>
struct vec3
{
	_t x, y, z;

	vec3<_t>() {}
	vec3<_t>(_t x, _t y, _t z) {this->x = x;this->y = y;this->z = z;}
	vec3<_t>(const vec3<_t>& _a) {*this = _a;}

	_t dot_product(const vec3<_t>& _a) {
		return x * _a.x + y * _a.y + z * _a.z;
	}

	vec3<_t> cross(const vec3<_t>& _a) {
		return vec3<_t>(y*_a.z - z*_a.y, z*_a.x - x*_a.z, x*_a.y - y*_a.x);
	}

	_t length() const {
		return sqrt(x*x + y*y + z*z);
	}

	_t square_length() const {
		return x*x + y*y + z*z;
	}

	void normalize() {
		*this *= 1 / length();
	}

	vec3<_t> operator*(const vec3<_t>& _a) const {
		vec3<_t> _b;
		_b.x = x * _a.x;
		_b.y = y * _a.y;
		_b.z = z * _a.z;
		return _b;
	}

	vec3<_t> operator*(const _t& _a) const {
		vec3<_t> _b;
		_b.x = x * _a;
		_b.y = y * _a;
		_b.z = z * _a;
		return _b;
	}
	vec3<_t> operator/(const _t& _a) const {
		vec3<_t> _b;
		_b.x = x / _a;
		_b.y = y / _a;
		_b.z = z / _a;
		return _b;
	}

	vec3<_t> operator+(const vec3<_t>& _a) const {
		vec3<_t> _b;
		_b.x = x + _a.x;
		_b.y = y + _a.y;
		_b.z = z + _a.z;
		return _b;
	}
	vec3<_t> operator-(const vec3<_t>& _a) const {
		vec3<_t> _b;
		_b.x = x - _a.x;
		_b.y = y - _a.y;
		_b.z = z - _a.z;
		return _b;
	}

	vec3<_t>& operator=(const vec3<_t>& _a) {
		x = _a.x;
		y = _a.y;
		z = _a.z;
		return *this;
	}

	const vec3<_t>& operator+=(const vec3<_t>& _a) {
		x += _a.x;
		y += _a.y;
		z += _a.z;
		return *this;
	}

	const vec3<_t>& operator-=(const vec3<_t>& _a) {
		x -= _a.x;
		y -= _a.y;
		z -= _a.z;
		return *this;
	}

	const vec3<_t>& operator*=(const vec3<_t>& _a) {
		x *= _a.x;
		y *= _a.y;
		z *= _a.z;
		return *this;
	}

	const vec3<_t>& operator*=(const _t& _a) {
		x *= _a;
		y *= _a;
		z *= _a;
		return *this;
	}
};

typedef vec2<float>	vec2f;
typedef vec2<int>	vec2i;
typedef vec3<float>	vec3f;

#endif
