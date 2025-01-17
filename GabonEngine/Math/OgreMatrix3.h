/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __Matrix3_H__
#define __Matrix3_H__

#include "OgreVector3.h"
#include "OgrePrerequisites.h"


// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */
    /** A 3x3 matrix which can represent rotations around axes.
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.geometrictools.com/).</b>
        @par
            The coordinate system is assumed to be <b>right-handed</b>.
    */
    class _OgreExport Matrix3
    {
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
        Matrix3 () {}
        explicit Matrix3 (const float arr[3][3])
        {
            memcpy(m,arr,9*sizeof(float));
        }

        Matrix3 (float fEntry00, float fEntry01, float fEntry02,
                    float fEntry10, float fEntry11, float fEntry12,
                    float fEntry20, float fEntry21, float fEntry22)
        {
            m[0][0] = fEntry00;
            m[0][1] = fEntry01;
            m[0][2] = fEntry02;
            m[1][0] = fEntry10;
            m[1][1] = fEntry11;
            m[1][2] = fEntry12;
            m[2][0] = fEntry20;
            m[2][1] = fEntry21;
            m[2][2] = fEntry22;
        }

        /// Member access, allows use of construct mat[r][c]
        const float* operator[] (size_t iRow) const
        {
            return m[iRow];
        }

        float* operator[] (size_t iRow)
        {
            return m[iRow];
        }

        Vector3 GetColumn (size_t iCol) const;
        void SetColumn(size_t iCol, const Vector3& vec);
        void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

        /** Tests 2 matrices for equality.
         */
        bool operator== (const Matrix3& rkMatrix) const;

        /** Tests 2 matrices for inequality.
         */
        bool operator!= (const Matrix3& rkMatrix) const
        {
            return !operator==(rkMatrix);
        }

        // arithmetic operations
        /** Matrix addition.
         */
        Matrix3 operator+ (const Matrix3& rkMatrix) const;

        /** Matrix subtraction.
         */
        Matrix3 operator- (const Matrix3& rkMatrix) const;

        /** Matrix concatenation using '*'.
         */
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;

        /// Vector * matrix [1x3 * 3x3 = 1x3]
        _OgreExport friend Vector3 operator* (const Vector3& rkVector,
            const Matrix3& rkMatrix);

        /// Matrix * scalar
        Matrix3 operator* (float fScalar) const;

        /// Scalar * matrix
        _OgreExport friend Matrix3 operator* (float fScalar, const Matrix3& rkMatrix);

        // utilities
        Matrix3 Transpose () const;
        bool Inverse (Matrix3& rkInverse, float fTolerance = 1e-06) const;
        Matrix3 Inverse (float fTolerance = 1e-06) const;
        float Determinant () const;

        Matrix3 transpose() const { return Transpose(); }
        Matrix3 inverse() const { return Inverse(); }
        float determinant() const { return Determinant(); }

        /** Determines if this matrix involves a negative scaling. */
        bool hasNegativeScale() const { return determinant() < 0; }

        /// Singular value decomposition
        void SingularValueDecomposition (Matrix3& rkL, Vector3& rkS,
            Matrix3& rkR) const;
        void SingularValueComposition (const Matrix3& rkL,
            const Vector3& rkS, const Matrix3& rkR);

        /// Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        void Orthonormalize ();

        /// Orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (Matrix3& rkQ, Vector3& rkD,
            Vector3& rkU) const;

        float SpectralNorm () const;

        /// Note: Matrix must be orthonormal
        void ToAngleAxis (Vector3& rkAxis, float& rfAngle) const;
//         inline void ToAngleAxis (Vector3& rkAxis, float& rfAngle) const {
//             float r;
//             ToAngleAxis ( rkAxis, r );
//             rfAngle = r;
//         }
        void FromAngleAxis (const Vector3& rkAxis, const float& ffloats);

        /** The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
            where yaw is rotation about the Up vector, pitch is rotation about the
            Right axis, and roll is rotation about the Direction axis. */
        bool ToEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool ToEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        void FromEulerAnglesXYZ (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        void FromEulerAnglesXZY (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        void FromEulerAnglesYXZ (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        void FromEulerAnglesYZX (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        void FromEulerAnglesZXY (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        void FromEulerAnglesZYX (const float& fYAngle, const float& fPAngle, const float& fRAngle);
        /// Eigensolver, matrix must be symmetric
        void EigenSolveSymmetric (float afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void TensorProduct (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);

        /** Determines if this matrix involves a scaling. */
        bool hasScale() const
        {
            // check magnitude of column vectors (==local axes)
            float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
            if (!Math::floatEqual(t, 1.0, (float)1e-04))
                return true;
            t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
            if (!Math::floatEqual(t, 1.0, (float)1e-04))
                return true;
            t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
            if (!Math::floatEqual(t, 1.0, (float)1e-04))
                return true;

            return false;
        }

        /** Function for writing to a stream.
        */
        inline friend std::ostream& operator <<
            ( std::ostream& o, const Matrix3& mat )
        {
            o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << "; "
                            << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << "; "
                            << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
            return o;
        }

        static const float EPSILON;
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    protected:
        // support for eigensolver
        void Tridiagonal (float afDiag[3], float afSubDiag[3]);
        bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

        // support for singular value decomposition
        static const float msSvdEpsilon;
        static const unsigned int msSvdMaxIterations;
        static void Bidiagonalize (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);
        static void GolubKahanStep (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);

        // support for spectral norm
        static float MaxCubicRoot (float afCoeff[3]);

        float m[3][3];

        // for faster access
        friend class Matrix4;
    };

    /// Matrix * vector [3x3 * 3x1 = 3x1]
    inline Vector3 operator*(const Matrix3& m, const Vector3& v)
    {
        return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    /** @} */
    /** @} */
}
#endif
