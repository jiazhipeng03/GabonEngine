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
#ifndef __Matrix4__
#define __Matrix4__

// Precompiler options
#include "OgrePrerequisites.h"

#include "OgreMatrix3.h"
#include "OgreVector4.h"
#include "OgrePlane.h"
namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */
    class Matrix4;
    class Affine3;
    Matrix4 operator*(const Matrix4 &m, const Matrix4 &m2);
    /** Class encapsulating a standard 4x4 homogeneous matrix.
        @remarks
            OGRE uses column vectors when applying matrix multiplications,
            This means a vector is represented as a single column, 4-row
            matrix. This has the effect that the transformations implemented
            by the matrices happens right-to-left e.g. if vector V is to be
            transformed by M1 then M2 then M3, the calculation would be
            M3 * M2 * M1 * V. The order that matrices are concatenated is
            vital since matrix multiplication is not commutative, i.e. you
            can get a different result if you concatenate in the wrong order.
        @par
            The use of column vectors and right-to-left ordering is the
            standard in most mathematical texts, and is the same as used in
            OpenGL. It is, however, the opposite of Direct3D, which has
            inexplicably chosen to differ from the accepted standard and uses
            row vectors and left-to-right matrix multiplication.
        @par
            OGRE deals with the differences between D3D and OpenGL etc.
            internally when operating through different render systems. OGRE
            users only need to conform to standard maths conventions, i.e.
            right-to-left matrix multiplication, (OGRE transposes matrices it
            passes to D3D to compensate).
        @par
            The generic form M * V which shows the layout of the matrix
            entries is shown below:
            <pre>
                [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
                | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
                | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
                [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
            </pre>
    */
	// 用到时改成directx的左乘矩阵, trans, rot, scale都相反，不考虑兼容
    class _OgreExport TransformBase
    {
    protected:
        /// The matrix entries, indexed by [row][col].
        float m[4][4];
        // do not reduce storage for affine for compatibility with SSE, shader mat4 types
    public:
        float* operator[](size_t iRow)
        {
            assert(iRow < 4);
            return m[iRow];
        }

        const float* operator[](size_t iRow) const
        {
            assert(iRow < 4);
            return m[iRow];
        }

        /** Assignment from 3x3 matrix.
        */
        void set3x3Matrix(const Matrix3& mat3)
        {
            m[0][0] = mat3[0][0]; m[0][1] = mat3[0][1]; m[0][2] = mat3[0][2];
            m[1][0] = mat3[1][0]; m[1][1] = mat3[1][1]; m[1][2] = mat3[1][2];
            m[2][0] = mat3[2][0]; m[2][1] = mat3[2][1]; m[2][2] = mat3[2][2];
        }

        /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
        */
        Matrix3 linear() const
        {
            return Matrix3(m[0][0], m[0][1], m[0][2],
                           m[1][0], m[1][1], m[1][2],
                           m[2][0], m[2][1], m[2][2]);
        }

//         OGRE_DEPRECATED void extract3x3Matrix(Matrix3& m3x3) const { m3x3 = linear(); }
//         OGRE_DEPRECATED Quaternion extractQuaternion() const { return Quaternion(linear()); }
        /// Sets the translation transformation part of the matrix.
        void setTrans( const Vector3& v )
        {
            m[3][0] = v.x;
            m[3][1] = v.y;
            m[3][2] = v.z;
        }
        /// Extracts the translation transformation part of the matrix.
        Vector3 getTrans() const
        {
          return Vector3(m[3][0], m[3][1], m[3][2]);
        }
        /// Sets the scale part of the matrix.
        void setScale( const Vector3& v )
        {
            m[0][0] = v.x;
            m[1][1] = v.y;
            m[2][2] = v.z;
        }

        /** Builds a translation matrix
        */
        void makeTrans( const Vector3& v )
        {
            makeTrans(v.x, v.y, v.z);
        }

        void makeTrans( float tx, float ty, float tz )
        {
            m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
            m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
            m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
            m[3][0] = tx; m[3][1] = ty; m[3][2] = tz; m[3][3] = 1.0;
        }

        float determinant() const;

        Matrix4 transpose() const;

        /** Building a Affine3 from orientation / scale / position.
        @remarks
            Transform is performed in the order scale, rotate, translation, i.e. translation is independent
            of orientation axes, scale does not affect size of translation, rotation and scaling are always
            centered on the origin.
        */
        void makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

        /** Building an inverse Affine3 from orientation / scale / position.
        @remarks
            As makeTransform except it build the inverse given the same data as makeTransform, so
            performing -translation, -rotate, 1/scale in that order.
        */
        void makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);

        /** Function for writing to a stream.
         */
        inline friend std::ostream& operator<<(std::ostream& o, const TransformBase& mat)
        {
            o << "Matrix4(";
            for (size_t i = 0; i < 4; ++i)
            {
                for (size_t j = 0; j < 4; ++j)
                {
                    o << mat[i][j];
                    if(j != 3)
                        o << ", ";
                }

                if(i != 3)
                    o << "; ";
            }
            o << ")";
            return o;
        }
    };
    /// Transform specialization for projective - encapsulating a 4x4 Matrix
    class _OgreExport Matrix4 : public TransformBase
    {
    public:
        /// Do <b>NOT</b> initialize the matrix for efficiency.
        Matrix4() {}

        Matrix4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33 )
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
        }

        /** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3 matrix.
         */

        inline Matrix4(const Matrix3& m3x3)
        {
          operator=(IDENTITY);
          operator=(m3x3);
        }

        /** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling Quaternion.
         */
        
        inline Matrix4(const Quaternion& rot)
        {
          Matrix3 m3x3;
          rot.ToRotationMatrix(m3x3);
          *this = IDENTITY;
          *this = m3x3;
        }
        
        Matrix4& operator=(const Matrix3& mat3) {
            set3x3Matrix(mat3);
            return *this;
        }

        //OGRE_DEPRECATED Matrix4 concatenate(const Matrix4& m2) const { return *this * m2; }

        /** Tests 2 matrices for equality.
        */
        inline bool operator == ( const Matrix4& m2 ) const
        {
            if( 
                m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
                m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
                m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
                m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
                return false;
            return true;
        }

        /** Tests 2 matrices for inequality.
        */
        inline bool operator != ( const Matrix4& m2 ) const
        {
            if( 
                m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
                m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
                m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
                m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
                return true;
            return false;
        }

        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;
        /** Useful little matrix which takes 2D clipspace {-1, 1} to {0,1}
            and inverts the Y. */
        static const Matrix4 CLIPSPACE2DTOIMAGESPACE;

        inline Matrix4 operator*(float scalar) const
        {
            return Matrix4(
                scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
                scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
                scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
                scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
        }
        
        Matrix4 adjoint() const;
        Matrix4 inverse() const;

		static Matrix4 buildReflectionMatrix(const Plane& p)
		{
			return Matrix4(
				-2 * p.normal.x * p.normal.x + 1, -2 * p.normal.x * p.normal.y, -2 * p.normal.x * p.normal.z, 0,
				-2 * p.normal.y * p.normal.x, -2 * p.normal.y * p.normal.y + 1, -2 * p.normal.y * p.normal.z, 0,
				-2 * p.normal.z * p.normal.x, -2 * p.normal.z * p.normal.y, -2 * p.normal.z * p.normal.z + 1, 0,
				-2 * p.normal.x * p.d, -2 * p.normal.y * p.d, -2 * p.normal.z * p.d, 1
			);
		}
    };

    /// Transform specialization for 3D Affine - encapsulating a 3x4 Matrix
    class _OgreExport Affine3 : public TransformBase
    {
    public:
        /// Do <b>NOT</b> initialize the matrix for efficiency.
        Affine3() {}

        /// @copydoc TransformBase::makeTransform
        Affine3(const Vector3& position, const Quaternion& orientation, const Vector3& scale = Vector3::UNIT_SCALE)
        {
            makeTransform(position, scale, orientation);
        }

        Affine3(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = 0;   m[3][1] = 0;   m[3][2] = 0;   m[3][3] = 1;
        }

        /// extract the Affine part of a Matrix4
        explicit Affine3(const Matrix4& mat)
        {
            m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2]; m[0][3] = mat[0][3];
            m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2]; m[1][3] = mat[1][3];
            m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2]; m[2][3] = mat[2][3];
            m[3][0] = 0;         m[3][1] = 0;         m[3][2] = 0;         m[3][3] = 1;
        }

        Affine3& operator=(const Matrix3& mat3) {
            set3x3Matrix(mat3);
            return *this;
        }

        /** Tests 2 matrices for equality.
        */
        bool operator==(const Affine3& m2) const
        {
            if(
                m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
                m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
                m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] )
                return false;
            return true;
        }

        bool operator!=(const Affine3& m2) const { return !(*this == m2); }

        Affine3 inverse() const;

        /** Decompose to orientation / scale / position.
        */
        void decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const;

        /// every Affine3 transform is also a _const_ Matrix4
        operator const Matrix4&() const { return reinterpret_cast<const Matrix4&>(*this); }

        using TransformBase::getTrans;

        /** Gets a translation matrix.
        */
        static Affine3 getTrans( const Vector3& v )
        {
            return getTrans(v.x, v.y, v.z);
        }

        /** Gets a translation matrix - variation for not using a vector.
        */
        static Affine3 getTrans( float t_x, float t_y, float t_z )
        {
            return Affine3(1, 0, 0, t_x,
                           0, 1, 0, t_y,
                           0, 0, 1, t_z);
        }

        /** Gets a scale matrix.
        */
        static Affine3 getScale( const Vector3& v )
        {
            return getScale(v.x, v.y, v.z);
        }

        /** Gets a scale matrix - variation for not using a vector.
        */
        static Affine3 getScale( float s_x, float s_y, float s_z )
        {
            return Affine3(s_x, 0, 0, 0,
                           0, s_y, 0, 0,
                           0, 0, s_z, 0);
        }


        static const Affine3 ZERO;
        static const Affine3 IDENTITY;
    };

    inline Matrix4 TransformBase::transpose() const
    {
        return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
                       m[0][1], m[1][1], m[2][1], m[3][1],
                       m[0][2], m[1][2], m[2][2], m[3][2],
                       m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    /** Matrix addition.
    */
    inline Matrix4 operator+(const Matrix4& m, const Matrix4& m2)
    {
        Matrix4 r;

        r[0][0] = m[0][0] + m2[0][0];
        r[0][1] = m[0][1] + m2[0][1];
        r[0][2] = m[0][2] + m2[0][2];
        r[0][3] = m[0][3] + m2[0][3];

        r[1][0] = m[1][0] + m2[1][0];
        r[1][1] = m[1][1] + m2[1][1];
        r[1][2] = m[1][2] + m2[1][2];
        r[1][3] = m[1][3] + m2[1][3];

        r[2][0] = m[2][0] + m2[2][0];
        r[2][1] = m[2][1] + m2[2][1];
        r[2][2] = m[2][2] + m2[2][2];
        r[2][3] = m[2][3] + m2[2][3];

        r[3][0] = m[3][0] + m2[3][0];
        r[3][1] = m[3][1] + m2[3][1];
        r[3][2] = m[3][2] + m2[3][2];
        r[3][3] = m[3][3] + m2[3][3];

        return r;
    }

    /** Matrix subtraction.
    */
    inline Matrix4 operator-(const Matrix4& m, const Matrix4& m2)
    {
        Matrix4 r;
        r[0][0] = m[0][0] - m2[0][0];
        r[0][1] = m[0][1] - m2[0][1];
        r[0][2] = m[0][2] - m2[0][2];
        r[0][3] = m[0][3] - m2[0][3];

        r[1][0] = m[1][0] - m2[1][0];
        r[1][1] = m[1][1] - m2[1][1];
        r[1][2] = m[1][2] - m2[1][2];
        r[1][3] = m[1][3] - m2[1][3];

        r[2][0] = m[2][0] - m2[2][0];
        r[2][1] = m[2][1] - m2[2][1];
        r[2][2] = m[2][2] - m2[2][2];
        r[2][3] = m[2][3] - m2[2][3];

        r[3][0] = m[3][0] - m2[3][0];
        r[3][1] = m[3][1] - m2[3][1];
        r[3][2] = m[3][2] - m2[3][2];
        r[3][3] = m[3][3] - m2[3][3];

        return r;
    }

    inline Matrix4 operator*(const Matrix4 &m, const Matrix4 &m2)
    {
        Matrix4 r;
        r[0][0] = m[0][0] * m2[0][0] + m[0][1] * m2[1][0] + m[0][2] * m2[2][0] + m[0][3] * m2[3][0];
        r[0][1] = m[0][0] * m2[0][1] + m[0][1] * m2[1][1] + m[0][2] * m2[2][1] + m[0][3] * m2[3][1];
        r[0][2] = m[0][0] * m2[0][2] + m[0][1] * m2[1][2] + m[0][2] * m2[2][2] + m[0][3] * m2[3][2];
        r[0][3] = m[0][0] * m2[0][3] + m[0][1] * m2[1][3] + m[0][2] * m2[2][3] + m[0][3] * m2[3][3];

        r[1][0] = m[1][0] * m2[0][0] + m[1][1] * m2[1][0] + m[1][2] * m2[2][0] + m[1][3] * m2[3][0];
        r[1][1] = m[1][0] * m2[0][1] + m[1][1] * m2[1][1] + m[1][2] * m2[2][1] + m[1][3] * m2[3][1];
        r[1][2] = m[1][0] * m2[0][2] + m[1][1] * m2[1][2] + m[1][2] * m2[2][2] + m[1][3] * m2[3][2];
        r[1][3] = m[1][0] * m2[0][3] + m[1][1] * m2[1][3] + m[1][2] * m2[2][3] + m[1][3] * m2[3][3];

        r[2][0] = m[2][0] * m2[0][0] + m[2][1] * m2[1][0] + m[2][2] * m2[2][0] + m[2][3] * m2[3][0];
        r[2][1] = m[2][0] * m2[0][1] + m[2][1] * m2[1][1] + m[2][2] * m2[2][1] + m[2][3] * m2[3][1];
        r[2][2] = m[2][0] * m2[0][2] + m[2][1] * m2[1][2] + m[2][2] * m2[2][2] + m[2][3] * m2[3][2];
        r[2][3] = m[2][0] * m2[0][3] + m[2][1] * m2[1][3] + m[2][2] * m2[2][3] + m[2][3] * m2[3][3];

        r[3][0] = m[3][0] * m2[0][0] + m[3][1] * m2[1][0] + m[3][2] * m2[2][0] + m[3][3] * m2[3][0];
        r[3][1] = m[3][0] * m2[0][1] + m[3][1] * m2[1][1] + m[3][2] * m2[2][1] + m[3][3] * m2[3][1];
        r[3][2] = m[3][0] * m2[0][2] + m[3][1] * m2[1][2] + m[3][2] * m2[2][2] + m[3][3] * m2[3][2];
        r[3][3] = m[3][0] * m2[0][3] + m[3][1] * m2[1][3] + m[3][2] * m2[2][3] + m[3][3] * m2[3][3];

        return r;
    }
    inline Affine3 operator*(const Affine3 &m, const Affine3 &m2)
    {
        return Affine3(
            m[0][0] * m2[0][0] + m[0][1] * m2[1][0] + m[0][2] * m2[2][0],
            m[0][0] * m2[0][1] + m[0][1] * m2[1][1] + m[0][2] * m2[2][1],
            m[0][0] * m2[0][2] + m[0][1] * m2[1][2] + m[0][2] * m2[2][2],
            m[0][0] * m2[0][3] + m[0][1] * m2[1][3] + m[0][2] * m2[2][3] + m[0][3],

            m[1][0] * m2[0][0] + m[1][1] * m2[1][0] + m[1][2] * m2[2][0],
            m[1][0] * m2[0][1] + m[1][1] * m2[1][1] + m[1][2] * m2[2][1],
            m[1][0] * m2[0][2] + m[1][1] * m2[1][2] + m[1][2] * m2[2][2],
            m[1][0] * m2[0][3] + m[1][1] * m2[1][3] + m[1][2] * m2[2][3] + m[1][3],

            m[2][0] * m2[0][0] + m[2][1] * m2[1][0] + m[2][2] * m2[2][0],
            m[2][0] * m2[0][1] + m[2][1] * m2[1][1] + m[2][2] * m2[2][1],
            m[2][0] * m2[0][2] + m[2][1] * m2[1][2] + m[2][2] * m2[2][2],
            m[2][0] * m2[0][3] + m[2][1] * m2[1][3] + m[2][2] * m2[2][3] + m[2][3]);
    }

    /** Vector transformation using '*'.
        @remarks
            Transforms the given 3-D vector by the matrix, projecting the
            result back into <i>w</i> = 1.
        @note
            This means that the initial <i>w</i> is considered to be 1.0,
            and then all the tree elements of the resulting 3-D vector are
            divided by the resulting <i>w</i>.
    */
    inline Vector3 operator*(const Matrix4& m, const Vector3& v)
    {
        Vector3 r;

        float fInvW = 1.0f / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

        r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
        r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
        r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

        return r;
    }
    /// @overload
    inline Vector3 operator*(const Affine3& m,const Vector3& v)
    {
        return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
    }

    inline Vector4 operator*(const Matrix4& m, const Vector4& v)
    {
        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
    }
    inline Vector4 operator*(const Affine3& m, const Vector4& v)
    {
        return Vector4(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            v.w);
    }

    inline Vector4 operator * (const Vector4& v, const Matrix4& mat)
    {
        return Vector4(
            v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
            v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
            v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
            v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
            );
    }

//     inline Plane operator * (const Matrix4& mat, const Plane& p)
//     {
//         Plane ret;
//         Matrix4 invTrans = mat.inverse().transpose();
//         Vector4 v4( p.normal.x, p.normal.y, p.normal.z, p.d );
//         v4 = invTrans * v4;
//         ret.normal.x = v4.x;
//         ret.normal.y = v4.y;
//         ret.normal.z = v4.z;
//         ret.d = v4.w / ret.normal.normalise();
// 
//         return ret;
//     }
    /** @} */
    /** @} */

}
#endif
