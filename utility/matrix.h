/*
    Inertial Measurement Unit Maths Library
    Copyright (C) 2013-2014  Samuel Cowen
    www.camelsoftware.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IMUMATH_MATRIX_HPP
#define IMUMATH_MATRIX_HPP

#include <string.h>
#include <stdint.h>

#include "vector.h"

namespace imu
{


template <uint8_t N> class Matrix
{
public:
    Matrix()
    {
        memset(_cell_data, 0, N*N*sizeof(double));
    }

    Matrix(const Matrix &m)
    {
        for (int ij = 0; ij < N*N; ++ij)
            _cell_data[ij] = m._cell_data[ij];
    }

    Matrix& operator=(const Matrix& m)
    {
        for (int ij = 0; ij < N*N; ++ij)
            _cell_data[ij] = m._cell_data[ij];
        return *this;
    }

    Vector<N> getRow(int i) const
    {
        Vector<N> ret;
        for (int j = 0; j < N; ++j)
            ret[j] = cell(i, j);
        return ret;
    }

    Vector<N> getColumn(int j) const
    {
        Vector<N> ret;
        for (int i = 0; i < N; ++i)
            ret[i] = cell(i, j);
        return ret;
    }

    void setRow(const Vector<N>& v, int i)
    {
        for (int j = 0; j < N; ++j)
            cell(i, j) = v[j];
    }

    void setColumn(const Vector<N>& v, int j)
    {
        for (int i = 0; i < N; i++)
            cell(i, j) = v[i];
    }

    double operator()(int i, int j) const
    {
        return cell(i, j);
    }
    double& operator()(int i, int j)
    {
        return cell(i, j);
    }

    double cell(int i, int j) const
    {
        return _cell_data[i*N+j];
    }
    double& cell(int i, int j)
    {
        return _cell_data[i*N+j];
    }

    Matrix operator+(const Matrix& m) const
    {
        Matrix ret;
        for (int ij = 0; ij < N*N; ++ij)
            ret._cell_data[ij] = _cell_data[ij] + m._cell_data[ij];
        return ret;
    }

    Matrix operator-(const Matrix& m) const
    {
        Matrix ret;
        for (int ij = 0; ij < N*N; ++ij)
            ret._cell_data[ij] = _cell_data[ij] - m._cell_data[ij];
        return ret;
    }

    Matrix operator*(double scalar) const
    {
        Matrix ret;
        for (int ij = 0; ij < N*N; ++ij)
            ret._cell_data[ij] = _cell_data[ij] * scalar;
        return ret;
    }

    Matrix operator*(const Matrix& m) const
    {
        Matrix ret;
        for (int i = 0; i < N; ++i)
        {
            Vector<N> row = getRow(i);
            for (int j = 0; j < N; ++j)
                ret(i, j) = row.dot(m.getColumn(j));
        }
        return ret;
    }

    Matrix transposed() const
    {
        Matrix ret;
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
                ret(j, i) = cell(i, j);
        }
        return ret;
    }

    Matrix<N-1> minor(int row, int col) const
    {
        int im, jm;
        Matrix<N-1> ret;
        for (int i = 0, im = 0; i < N; ++i)
        {
            if (i == row)
                continue;

            for (int j = 0, jm = 0; j < N; ++j)
            {
                if (j != col)
                    ret(im, jm++) = cell(i, j);
            }
            ++im;
        }
        return ret;
    }

    double determinant() const
    {
        // specialization for N == 1 given below this class
        double det = 0.0, sign = 1.0;
        for (int i = 0; i < N; ++i, sign = -sign)
            det += sign * cell(0, i) * minor(0, i).determinant();
        return det;
    }

    Matrix inverted() const
    {
        Matrix ret;
        double det = determinant();

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                ret(i, j) = minor(j, i).determinant() / det;
                if ( (i+j)%2 == 1)
                    ret(i, j) = -ret(i, j);
            }
        }
        return ret;
    }

private:
    double _cell_data[N*N];
};

template<>
inline double Matrix<1>::determinant() const
{
    return cell(0, 0);
}

};

#endif

