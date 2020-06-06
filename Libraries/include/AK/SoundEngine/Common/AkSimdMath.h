/***********************************************************************
  The content of this file includes source code for the sound engine
  portion of the AUDIOKINETIC Wwise Technology and constitutes "Level
  Two Source Code" as defined in the Source Code Addendum attached
  with this file.  Any use of the Level Two Source Code shall be
  subject to the terms and conditions outlined in the Source Code
  Addendum and the End User License Agreement for Wwise(R).

  Version: v2019.2.1  Build: 7250
  Copyright (c) 2006-2020 Audiokinetic Inc.
 ***********************************************************************/

//////////////////////////////////////////////////////////////////////
//
// AkSimdMath.h
//
// Library of static functions for math computations with SIMD in mind.
//
//////////////////////////////////////////////////////////////////////
#ifndef _AKSIMDMATH_H_
#define _AKSIMDMATH_H_

#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkSimd.h>
#include <AkMath.h>

namespace AkMath
{
	//Take 4 vectors <x,y,z> and return <x,x,x,x>, <y,y,y,y> and <z,z,z,z>
	AkForceInline void PermuteVectors3(const AKSIMD_V4F32& v0, const AKSIMD_V4F32& v1, const AKSIMD_V4F32& v2, const AKSIMD_V4F32& v3,
		AKSIMD_V4F32& out_xxxx, AKSIMD_V4F32& out_yyyy, AKSIMD_V4F32& out_zzzz)
	{
		AKSIMD_V4F32 xyxy0 = AKSIMD_SHUFFLE_V4F32(v0, v1, AKSIMD_SHUFFLE(1, 0, 1, 0));
		AKSIMD_V4F32 xyxy1 = AKSIMD_SHUFFLE_V4F32(v2, v3, AKSIMD_SHUFFLE(1, 0, 1, 0));
		out_xxxx = AKSIMD_SHUFFLE_V4F32(xyxy0, xyxy1, AKSIMD_SHUFFLE(2, 0, 2, 0));
		out_yyyy = AKSIMD_SHUFFLE_V4F32(xyxy0, xyxy1, AKSIMD_SHUFFLE(3, 1, 3, 1));

		AKSIMD_V4F32 zwzw0 = AKSIMD_SHUFFLE_V4F32(v0, v1, AKSIMD_SHUFFLE(3, 2, 3, 2));
		AKSIMD_V4F32 zwzw1 = AKSIMD_SHUFFLE_V4F32(v2, v3, AKSIMD_SHUFFLE(3, 2, 3, 2));
		out_zzzz = AKSIMD_SHUFFLE_V4F32(zwzw0, zwzw1, AKSIMD_SHUFFLE(2, 0, 2, 0));
	}

	//Take 3 vectors <x3,x2,x1,x0>, <y,y,y,y> and <z,z,z,z> and return 4 vectors <x,y,z,z> 
	AkForceInline void UnpermuteVectors3(const AKSIMD_V4F32& xxxx, const AKSIMD_V4F32& yyyy, const AKSIMD_V4F32& zzzz, 
		AKSIMD_V4F32& out_v0, AKSIMD_V4F32& out_v1, AKSIMD_V4F32& out_v2, AKSIMD_V4F32& out_v3)
	{
		/*__m128 _mm_shuffle_ps(__m128 lo, __m128 hi, _MM_SHUFFLE(hi3, hi2, lo1, lo0))
		Interleave inputs into low 2 floats and high 2 floats of output.Basically
		out[0] = lo[lo0];
		out[1] = lo[lo1];
		out[2] = hi[hi2];
		out[3] = hi[hi3];
		*/

		AKSIMD_V4F32 x0x1y0y1 = AKSIMD_SHUFFLE_V4F32(xxxx, yyyy, AKSIMD_SHUFFLE(1, 0, 1, 0));
		AKSIMD_V4F32 z0z1z0z1 = AKSIMD_SHUFFLE_V4F32(zzzz, zzzz, AKSIMD_SHUFFLE(1, 0, 1, 0));
		
		out_v0 = AKSIMD_SHUFFLE_V4F32(x0x1y0y1, z0z1z0z1, AKSIMD_SHUFFLE(2, 0, 2, 0));
		out_v1 = AKSIMD_SHUFFLE_V4F32(x0x1y0y1, z0z1z0z1, AKSIMD_SHUFFLE(3, 1, 3, 1));

		AKSIMD_V4F32 x2x3y2y3 = AKSIMD_SHUFFLE_V4F32(xxxx, yyyy, AKSIMD_SHUFFLE(3, 2, 3, 2));
		AKSIMD_V4F32 z2z3z2z3 = AKSIMD_SHUFFLE_V4F32(zzzz, zzzz, AKSIMD_SHUFFLE(3, 2, 3, 2));
		
		out_v2 = AKSIMD_SHUFFLE_V4F32(x2x3y2y3, z2z3z2z3, AKSIMD_SHUFFLE(2, 0, 2, 0));
		out_v3 = AKSIMD_SHUFFLE_V4F32(x2x3y2y3, z2z3z2z3, AKSIMD_SHUFFLE(3, 1, 3, 1));
	}

	//Take 4 vectors <x,y,z,w> and return <x,x,x,x>, <y,y,y,y>, <z,z,z,z> and <w,w,w,w>
	AkForceInline void PermuteVectors4(const AKSIMD_V4F32& v0, const AKSIMD_V4F32& v1, const AKSIMD_V4F32& v2, const AKSIMD_V4F32& v3,
		AKSIMD_V4F32& out_xxxx, AKSIMD_V4F32& out_yyyy, AKSIMD_V4F32& out_zzzz, AKSIMD_V4F32& out_wwww)
	{
		AKSIMD_V4F32 xyxy0 = AKSIMD_SHUFFLE_V4F32(v0, v1, AKSIMD_SHUFFLE(1, 0, 1, 0));
		AKSIMD_V4F32 xyxy1 = AKSIMD_SHUFFLE_V4F32(v2, v3, AKSIMD_SHUFFLE(1, 0, 1, 0));
		out_xxxx = AKSIMD_SHUFFLE_V4F32(xyxy0, xyxy1, AKSIMD_SHUFFLE(2, 0, 2, 0));
		out_yyyy = AKSIMD_SHUFFLE_V4F32(xyxy0, xyxy1, AKSIMD_SHUFFLE(3, 1, 3, 1));

		AKSIMD_V4F32 zwzw0 = AKSIMD_SHUFFLE_V4F32(v0, v1, AKSIMD_SHUFFLE(3, 2, 3, 2));
		AKSIMD_V4F32 zwzw1 = AKSIMD_SHUFFLE_V4F32(v2, v3, AKSIMD_SHUFFLE(3, 2, 3, 2));
		out_zzzz = AKSIMD_SHUFFLE_V4F32(zwzw0, zwzw1, AKSIMD_SHUFFLE(2, 0, 2, 0));
		out_wwww = AKSIMD_SHUFFLE_V4F32(zwzw0, zwzw1, AKSIMD_SHUFFLE(3, 1, 3, 1));
	}

	// 3-element dot product of 4 vectors.
	AkForceInline AKSIMD_V4F32 DotPoduct3_4x4(const AKSIMD_V4F32& v0_x, const AKSIMD_V4F32& v0_y, const AKSIMD_V4F32& v0_z,
		const AKSIMD_V4F32& v1_x, const AKSIMD_V4F32& v1_y, const AKSIMD_V4F32& v1_z)
	{
		return AKSIMD_ADD_V4F32(AKSIMD_ADD_V4F32(AKSIMD_MUL_V4F32(v0_x, v1_x), AKSIMD_MUL_V4F32(v0_y, v1_y)), AKSIMD_MUL_V4F32(v0_z, v1_z));
	}

	// 3-element dot product of 1 common vector with 4 vectors
	AkForceInline AKSIMD_V4F32 DotPoduct3_1x4(const AKSIMD_V4F32& v0_xyz, const AKSIMD_V4F32& v1_x, const  AKSIMD_V4F32& v1_y, const AKSIMD_V4F32& v1_z)
	{
		const AKSIMD_V4F32 v0_x = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(0, 0, 0, 0));
		const AKSIMD_V4F32 v0_y = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(1, 1, 1, 1));
		const AKSIMD_V4F32 v0_z = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(2, 2, 2, 2));
		return DotPoduct3_4x4(v0_x, v0_y, v0_z, v1_x, v1_y, v1_z);
	}

	// 4-element dot product of 4 vectors.
	AkForceInline AKSIMD_V4F32 DotPoduct4_4x4(const AKSIMD_V4F32& v0_x, const AKSIMD_V4F32& v0_y, const AKSIMD_V4F32& v0_z, const AKSIMD_V4F32& v0_w,
		const AKSIMD_V4F32& v1_x, const AKSIMD_V4F32& v1_y, const AKSIMD_V4F32& v1_z, const AKSIMD_V4F32& v1_w)
	{
		return AKSIMD_ADD_V4F32(
			AKSIMD_ADD_V4F32(
			AKSIMD_MUL_V4F32(v0_x, v1_x),
			AKSIMD_MUL_V4F32(v0_y, v1_y)),
			AKSIMD_ADD_V4F32(
			AKSIMD_MUL_V4F32(v0_z, v1_z),
			AKSIMD_MUL_V4F32(v0_w, v1_w)));
	}

	// 4-element dot product of 1 common vector with 4 vectors
	AkForceInline AKSIMD_V4F32 DotPoduct4_1x4(const AKSIMD_V4F32& v0_xyz, const AKSIMD_V4F32& v1_x, const AKSIMD_V4F32& v1_y, const AKSIMD_V4F32& v1_z, const AKSIMD_V4F32& v1_w)
	{
		const AKSIMD_V4F32 v0_x = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(0, 0, 0, 0));
		const AKSIMD_V4F32 v0_y = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(1, 1, 1, 1));
		const AKSIMD_V4F32 v0_z = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(2, 2, 2, 2));
		const AKSIMD_V4F32 v0_w = AKSIMD_SHUFFLE_V4F32(v0_xyz, v0_xyz, AKSIMD_SHUFFLE(2, 2, 2, 2));
		return DotPoduct4_4x4(v0_x, v0_y, v0_z, v0_w, v1_x, v1_y, v1_z, v1_w);
	}

	// Trig functions approximation (based on the Fast versions found in AkMath.h)
	AkForceInline AKSIMD_V4F32 AKSIMD_SIN_V4F32(const AKSIMD_V4F32 x)
	{
		const AKSIMD_V4F32 B = AKSIMD_SET_V4F32(4 / PI);
		const AKSIMD_V4F32 C = AKSIMD_SET_V4F32(-4 / (PI * PI));
		const AKSIMD_V4F32 P = AKSIMD_SET_V4F32(0.225f);

		//float y = B * x + C * x * fabs(x); //float y = X*(B+C*fabs(x));

		AKSIMD_V4F32 y = AKSIMD_ABS_V4F32(x);
		y = AKSIMD_MADD_V4F32(y, C, B);
		y = AKSIMD_MUL_V4F32(y, x);

		//	return P * (y * fabs(y) - y) + y; 
		AKSIMD_V4F32 sine = AKSIMD_ABS_V4F32(y);
		sine = AKSIMD_MSUB_V4F32(y, sine, y);
		sine = AKSIMD_MADD_V4F32(sine, P, y);
		return sine;
	}

	AkForceInline AKSIMD_V4F32 AKSIMD_COS_V4F32(const AKSIMD_V4F32 x)
	{
		//Compute the offset needed for the cosinus.  If you compare with FastCos, the constants have been combined.
		const AKSIMD_V4F32 offsetNoWrap = AKSIMD_SET_V4F32(PI / 2);				// cos = sin(x+pi/2)
		const AKSIMD_V4F32 offsetWrap = AKSIMD_SET_V4F32(PI / 2 - 2 * PI);		// Wrap: cos(x) = cos(x - 2 pi)
		const AKSIMD_V4F32 vHalfPI = AKSIMD_SET_V4F32(PI / 2);

		// (cond1 >= cond2) ? a : b
		AKSIMD_V4F32 offset = AKSIMD_SEL_GTEZ_V4F32(AKSIMD_SUB_V4F32(x, vHalfPI), offsetWrap, offsetNoWrap);
		return AKSIMD_SIN_V4F32(AKSIMD_ADD_V4F32(x, offset));
	}

	AkForceInline AKSIMD_V4F32 AKSIMD_ATAN2_V4F32(AKSIMD_V4F32 y, AKSIMD_V4F32 x)
	{
		const AKSIMD_V4F32 vNeg = AKSIMD_SET_V4F32(-1.0f);
		const AKSIMD_V4F32 vOne = AKSIMD_SET_V4F32(1.0f);
		const AKSIMD_V4F32 vZero = AKSIMD_SET_V4F32(0.0f);
		const AKSIMD_V4F32 vK = AKSIMD_SET_V4F32(0.28f);
		const AKSIMD_V4F32 vKRepro = AKSIMD_SET_V4F32(1.f / 0.28f);
		const AKSIMD_V4F32 vHalfPI = AKSIMD_SET_V4F32(PI / 2);
		const AKSIMD_V4F32 vPI = AKSIMD_SET_V4F32(PI);
		const AKSIMD_V4F32 vEpsilon = AKSIMD_SET_V4F32(1e-20f);

		//Ensure x is not zero a == 0 ? b : c.
		x = AKSIMD_VSEL_V4F32(x, vEpsilon, AKSIMD_EQ_V4F32(x, vZero));

		AKSIMD_V4F32 z = AKSIMD_DIV_V4F32(y, x);
		AKSIMD_V4F32 absz = AKSIMD_ABS_V4F32(z);
		AKSIMD_V4COND zcond = AKSIMD_GTEQ_V4F32(vOne, absz);

		//The approximation is done in 2 segments of the form: offset + z/a*(z*z + b);

		//if ( fabsf( z ) < 1.0f ) then use .28 for the a coef
		AKSIMD_V4F32 a = AKSIMD_VSEL_V4F32(vNeg, vK, zcond);

		//if ( fabsf( z ) < 1.0f ) then use 1 for the b factor, else use 0.28
		AKSIMD_V4F32 b = AKSIMD_VSEL_V4F32(vK, vKRepro, zcond);

		AKSIMD_V4F32 atan = AKSIMD_MADD_V4F32(z, z, b);
		atan = AKSIMD_MUL_V4F32(atan, a);
		atan = AKSIMD_DIV_V4F32(z, atan);

		//Adjust for quadrant
		//	zcond	x<0		y<0	 offset
		//	1		0		0	 0			
		//	1		0		1	 0			
		//	1		1		0	 +PI		
		//	1		1		1	 -PI		
		//	0		0		0	 +PI/2		
		//	0		0		1	 -PI/2		
		//	0		1		0	 +PI/2		
		//	0		1		1	 -PI/2		

		AKSIMD_V4F32 offsetByX = AKSIMD_SEL_GTEZ_V4F32(x, vZero, vPI);
		AKSIMD_V4F32 offset = AKSIMD_VSEL_V4F32(vHalfPI, offsetByX, zcond);
		AKSIMD_V4F32 sign = AKSIMD_SEL_GTEZ_V4F32(y, vOne, vNeg);

		//Apply computed offset.  
		atan = AKSIMD_MADD_V4F32(offset, sign, atan);
		return atan;
	}
}

#endif	//_AKSIMDMATH_H_
