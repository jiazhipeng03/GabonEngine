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
#ifndef __Math_H__
#define __Math_H__

#define _OgreExport
#include "OgrePrerequisites.h"
// #include "OgreHeaderPrefix.h"
namespace Ogre
{
    class _OgreExport Math 
    {
	public:
		/** Default constructor.
		@param
		trigTableSize Optional parameter to set the size of the
		tables used to implement Sin, Cos, Tan
		*/
		Math() {};

		/** Default destructor.
		*/
		~Math() {};

		static inline int IAbs(int iValue) { return (iValue >= 0 ? iValue : -iValue); }
		static inline int ICeil(float fValue) { return int(ceil(fValue)); }
		static inline int IFloor(float fValue) { return int(floor(fValue)); }
		static int ISign(int iValue) {
			return (iValue > 0 ? +1 : (iValue < 0 ? -1 : 0));
		}

		/** Absolute value function
		@param
		fValue The value whose absolute value will be returned.
		*/
		static inline float Abs(float fValue) { return float(fabs(fValue)); }

		/** Arc cosine function
		@param fValue
		The value whose arc cosine will be returned.
		*/
		static float ACos(float fValue)
		{
			if (-1.0 < fValue)
			{
				if (fValue < 1.0)
					return (std::acos(fValue));
				else
					return (0.0);
			}
			else
			{
				return (PI);
			}
		}
		/** Arc sine function
		@param fValue
		The value whose arc sine will be returned.
		*/
		static float ASin(float fValue)
		{
			if (-1.0 < fValue)
			{
				if (fValue < 1.0)
					return (std::asin(fValue));
				else
					return (HALF_PI);
			}
			else
			{
				return (-HALF_PI);
			}
		}
		
		static inline float Cos(float fValue) { return cos(fValue); }
		static inline float Sin(float fValue) { return sin(fValue); }
		static inline float Tan(float fValue) { return tan(fValue); }
		/** Arc tangent function
		@param fValue
		The value whose arc tangent will be returned.
		*/
		static inline float ATan(float fValue) { return float(atan(fValue)); }

		/** Arc tangent between two values function
		@param fY
		The first value to calculate the arc tangent with.
		@param fX
		The second value to calculate the arc tangent with.
		*/
		static inline float ATan2(float fY, float fX) { return float(atan2(fY, fX)); }

		/** Ceiling function
		Returns the smallest following integer. (example: Ceil(1.1) = 2)

		@param fValue
		The value to round up to the nearest integer.
		*/
		static inline float Ceil(float fValue) { return float(ceil(fValue)); }
		static inline bool isNaN(float f)
		{
			// std::isnan() is C99, not supported by all compilers
			// However NaN always fails this next test, no other number does.
			return f != f;
		}

		static inline float Exp(float fValue) { return float(exp(fValue)); }

		/** Floor function
		Returns the largest previous integer. (example: Floor(1.9) = 1)

		@param fValue
		The value to round down to the nearest integer.
		*/
		static inline float Floor(float fValue) { return float(floor(fValue)); }

		static inline float Log(float fValue) { return float(log(fValue)); }

		/// Stored value of log(2) for frequent use
		static const float LOG2;

		static inline float Log2(float fValue) { return float(log(fValue) / LOG2); }

		static inline float LogN(float base, float fValue) { return float(log(fValue) / log(base)); }

		static inline float Pow(float fBase, float fExponent) { return float(pow(fBase, fExponent)); }

		static float Sign(float fValue);
		
		/// Simulate the shader function saturate that clamps a parameter value between 0 and 1
		static inline float saturate(float t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }
		static inline double saturate(double t) { return (t < 0) ? 0 : ((t > 1) ? 1 : t); }

		/** Simulate the shader function lerp which performers linear interpolation

		given 3 parameters v0, v1 and t the function returns the value of (1 - t)* v0 + t * v1.
		where v0 and v1 are matching vector or scalar types and t can be either a scalar or a
		vector of the same type as a and b.
		*/
		template <typename V, typename T> static V lerp(const V& v0, const V& v1, const T& t)
		{
			return v0 * (1 - t) + v1 * t;
		}

		/** Squared function.
		@param fValue
		The value to be squared (fValue^2)
		*/
		static inline float Sqr(float fValue) { return fValue*fValue; }

		/** Square root function.
		@param fValue
		The value whose square root will be calculated.
		*/
		static inline float Sqrt(float fValue) { return float(sqrt(fValue)); }
		
		/** Inverse square root i.e. 1 / Sqrt(x), good for vector
		normalisation.
		@param fValue
		The value whose inverse square root will be calculated.
		*/
		static float InvSqrt(float fValue) {
			return float(1.) / std::sqrt(fValue);
		}

		/** Generate a random number of unit length.
		@return
		A random number in the range from [0,1].
		*/
		static float UnitRandom()
		{
			return float(rand()) / RAND_MAX;
		}

		/** Generate a random number within the range provided.
		@param fLow
		The lower bound of the range.
		@param fHigh
		The upper bound of the range.
		@return
		A random number in the range from [fLow,fHigh].
		*/
		static float RangeRandom(float fLow, float fHigh) {
			return (fHigh - fLow)*UnitRandom() + fLow;
		}

		/** Generate a random number in the range [-1,1].
		@return
		A random number in the range from [-1,1].
		*/
		static float SymmetricRandom() {
			return 2.0f * UnitRandom() - 1.0f;
		}
		
		static inline float DegreesToRadians(float degrees) { return degrees * fDeg2Rad; }
		static inline float RadiansToDegrees(float radians) { return radians * fRad2Deg; }
		
		/** Compare 2 floats, using tolerance for inaccuracies.
		*/
		static bool floatEqual(float a, float b,
			float tolerance = std::numeric_limits<float>::epsilon()) {
			return std::abs(b - a) <= tolerance;
		}

		/** Clamp a value within an inclusive range. */
		template <typename T>
		static T Clamp(T val, T minval, T maxval)
		{
			assert(minval <= maxval && "Invalid clamp range");
			T temp = val < maxval ? val : maxval;// std::min(val, maxval);
			return std::max(temp, minval);
		}
		
		static const float POS_INFINITY;
		static const float NEG_INFINITY;
		static const float PI;
		static const float TWO_PI;
		static const float HALF_PI;
		static const float fDeg2Rad;
		static const float fRad2Deg;

	};

}

//#include "OgreHeaderSuffix.h"

#endif
