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
#ifndef CTTF_MAT_H
#define CTTF_MAT_H

template <class _t>
struct mat4 {
	_t	m11, m12, m13, m14;
	_t	m21, m22, m23, m24;
	_t	m31, m32, m33, m34;
	_t	m41, m42, m43, m44;

	mat4<_t>() {}
	mat4<_t>(const _t* _a) {
		m11 = _a[0];
		m12 = _a[1];
		m13 = _a[2];
		m14 = _a[3];
		m21 = _a[4];
		m22 = _a[5];
		m23 = _a[6];
		m24 = _a[7];
		m31 = _a[8];
		m32 = _a[9];
		m33 = _a[10];
		m34 = _a[11];
		m41 = _a[12];
		m42 = _a[13];
		m43 = _a[14];
		m44 = _a[15];
	}
	mat4<_t>(const mat4<_t>& _a) {*this = _a;}

	void identity() {
		diag(1, 1, 1, 1);
	}

	void set(_t a11, _t a12, _t a13, _t a14,
		_t a21, _t a22, _t a23, _t a24,
		_t a31, _t a32, _t a33, _t a34,
		_t a41, _t a42, _t a43, _t a44) {

		m11 = a11;
		m12 = a12;
		m13 = a13;
		m14 = a14;
		m21 = a21;
		m22 = a22;
		m23 = a23;
		m24 = a24;
		m31 = a31;
		m32 = a32;
		m33 = a33;
		m34 = a34;
		m41 = a41;
		m42 = a42;
		m43 = a43;
		m44 = a44;
	}

	_t det() {
		return m11*m22*m33*m44 + m11*m23*m34*m42 + m11*m24*m32*m43
			+ m12*m21*m34*m43 + m12*m23*m31*m44 + m12*m24*m33*m41
			+ m13*m21*m32*m44 + m13*m22*m34*m41 + m13*m24*m31*m42
			+ m14*m21*m33*m42 + m14*m22*m31*m43 + m14*m23*m32*m41
			- m11*m22*m34*m43 - m11*m23*m32*m44 - m11*m24*m33*m42
			- m12*m21*m33*m44 - m12*m23*m34*m41 - m12*m24*m31*m43
			- m13*m21*m34*m42 - m13*m22*m31*m44 - m13*m24*m32*m41
			- m14*m21*m32*m43 - m14*m22*m33*m41 - m14*m23*m31*m42;
	}

	bool invert() {
		_t	d = det();
		if (d == 0) {
			return false;
		}
		set(m22*m33*m44 + m23*m34*m42 + m24*m32*m43 - m22*m34*m43 - m23*m32*m44 - m24*m33*m42,
			m12*m34*m43 + m13*m32*m44 + m14*m33*m42 - m12*m23*m44 - m13*m34*m42 - m14*m32*m43,
			m12*m23*m44 + m13*m24*m42 + m14*m22*m43 - m12*m24*m43 - m13*m22*m44 - m14*m23*m42,
			m12*m24*m33 + m13*m22*m34 + m14*m23*m32 - m12*m23*m34 - m13*m24*m32 - m14*m22*m33,
			m21*m34*m43 + m23*m31*m44 + m24*m33*m41 - m21*m33*m44 - m23*m34*m41 - m24*m31*m43,
			m11*m33*m44 + m13*m34*m41 + m14*m31*m43 - m11*m34*m43 - m13*m31*m44 - m14*m33*m41,
			m11*m24*m43 + m13*m21*m44 + m14*m23*m41 - m11*m23*m44 - m13*m24*m41 - m14*m21*m43,
			m11*m23*m34 + m13*m24*m31 + m14*m21*m33 - m11*m24*m33 - m13*m21*m34 - m14*m23*m31,
			m21*m32*m44 + m22*m23*m41 + m24*m31*m42 - m21*m34*m42 - m22*m31*m44 - m24*m32*m41,
			m11*m34*m42 + m12*m31*m44 + m14*m32*m41 - m11*m32*m44 - m12*m34*m41 - m14*m31*m42,
			m11*m22*m44 + m12*m24*m41 + m14*m21*m42 - m11*m24*m42 - m12*m21*m44 - m14*m22*m41,
			m11*m24*m32 + m12*m21*m34 + m14*m22*m31 - m11*m22*m34 - m12*m24*m31 - m14*m21*m32,
			m21*m33*m42 + m22*m31*m43 + m23*m32*m41 - m21*m32*m43 - m22*m33*m41 - m23*m31*m42,
			m11*m32*m43 + m12*m33*m41 + m13*m31*m42 - m11*m33*m42 - m12*m31*m43 - m13*m32*m41,
			m11*m23*m42 + m12*m21*m43 + m13*m22*m41 - m11*m22*m43 - m12*m23*m41 - m13*m21*m42,
			m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 - m13*m22*m31);
		(*this) *= 1 / d;
		return true;
	}

	void diag(_t d1, _t d2, _t d3, _t d4) {
		m11 = d1;
		m12 = m13 = m14 = 0;
		m22 = d2;
		m21 = m23 = m24 = 0;
		m33 = d3;
		m31 = m32 = m34 = 0;
		m44 = d4;
		m41 = m42 = m43 = 0;
	}

	mat4<_t> operator*(const mat4<_t>& _a) const {
		mat4<_t> _b;
		_b.m11 = m11*_a.m11 + m12*_a.m21 + m13*_a.m31 + m14*_a.m41;
		_b.m12 = m11*_a.m11 + m12*_a.m21 + m13*_a.m31 + m14*_a.m41;
		_b.m13 = m11*_a.m13 + m12*_a.m23 + m13*_a.m33 + m14*_a.m43;
		_b.m14 = m11*_a.m14 + m12*_a.m24 + m13*_a.m34 + m14*_a.m44;
		_b.m21 = m21*_a.m11 + m22*_a.m21 + m23*_a.m31 + m24*_a.m41;
		_b.m22 = m21*_a.m11 + m22*_a.m21 + m23*_a.m31 + m24*_a.m41;
		_b.m23 = m21*_a.m13 + m22*_a.m23 + m23*_a.m33 + m24*_a.m43;
		_b.m24 = m21*_a.m14 + m22*_a.m24 + m23*_a.m34 + m24*_a.m44;
		_b.m31 = m31*_a.m11 + m32*_a.m21 + m33*_a.m31 + m34*_a.m41;
		_b.m32 = m31*_a.m11 + m32*_a.m21 + m33*_a.m31 + m34*_a.m41;
		_b.m33 = m31*_a.m13 + m32*_a.m23 + m33*_a.m33 + m34*_a.m43;
		_b.m34 = m31*_a.m14 + m32*_a.m24 + m33*_a.m34 + m34*_a.m44;
		_b.m41 = m41*_a.m11 + m42*_a.m21 + m43*_a.m31 + m44*_a.m41;
		_b.m42 = m41*_a.m11 + m42*_a.m21 + m43*_a.m31 + m44*_a.m41;
		_b.m43 = m41*_a.m13 + m42*_a.m23 + m43*_a.m33 + m44*_a.m43;
		_b.m44 = m41*_a.m14 + m42*_a.m24 + m43*_a.m34 + m44*_a.m44;
		return _b;
	}

	const mat4<_t>& operator*=(const _t& _a) {
		m11 *= _a;
		m12 *= _a;
		m13 *= _a;
		m14 *= _a;
		m21 *= _a;
		m22 *= _a;
		m23 *= _a;
		m24 *= _a;
		m31 *= _a;
		m32 *= _a;
		m33 *= _a;
		m34 *= _a;
		m41 *= _a;
		m42 *= _a;
		m43 *= _a;
		m44 *= _a;
		return *this;
	}

	const mat4<_t>& operator=(const mat4<_t>& _a) {
		m11 = _a.m11;
		m12 = _a.m12;
		m13 = _a.m13;
		m14 = _a.m14;
		m21 = _a.m21;
		m22 = _a.m22;
		m23 = _a.m23;
		m24 = _a.m24;
		m31 = _a.m31;
		m32 = _a.m32;
		m33 = _a.m33;
		m34 = _a.m34;
		m41 = _a.m41;
		m42 = _a.m42;
		m43 = _a.m43;
		m44 = _a.m44;
		return *this;
	}


};

typedef mat4<float> mat4f;

#endif
