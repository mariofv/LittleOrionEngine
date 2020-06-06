/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Version: v2019.2.1  Build: 7250
  Copyright (c) 2006-2020 Audiokinetic Inc.
*******************************************************************************/

// AkSimd.h

/// \file 
/// AKSIMD - arm_neon implementation

#ifndef _AKSIMD_ARM_NEON_H_
#define _AKSIMD_ARM_NEON_H_

#if defined _MSC_VER && defined _M_ARM64
	#include <arm64_neon.h>
#else
	#include <arm_neon.h>
#endif
#include <AK/SoundEngine/Common/AkTypes.h>

// Platform specific defines for prefetching
#define AKSIMD_ARCHMAXPREFETCHSIZE	(512) 				///< Use this to control how much prefetching maximum is desirable (assuming 8-way cache)		
#define AKSIMD_ARCHCACHELINESIZE	(64)				///< Assumed cache line width for architectures on this platform
#if defined __clang__ || defined __GNUC__
#define AKSIMD_PREFETCHMEMORY( __offset__, __add__ ) __builtin_prefetch(((char *)(__add__))+(__offset__)) 
#else
#define AKSIMD_PREFETCHMEMORY( __offset__, __add__ )
#endif

////////////////////////////////////////////////////////////////////////
/// @name Platform specific memory size alignment for allocation purposes
//@{

#define AKSIMD_ALIGNSIZE( __Size__ ) (((__Size__) + 15) & ~15)

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD types
//@{

typedef int32x4_t		AKSIMD_V4I32;		///< Vector of 4 32-bit signed integers
typedef int16x8_t		AKSIMD_V8I16;		///< Vector of 8 16-bit signed integers
typedef int16x4_t		AKSIMD_V4I16;		///< Vector of 4 16-bit signed integers
typedef uint32x4_t		AKSIMD_V4UI32;		///< Vector of 4 32-bit unsigned signed integers
typedef uint32x2_t		AKSIMD_V2UI32;		///< Vector of 2 32-bit unsigned signed integers
typedef int32x2_t		AKSIMD_V2I32;		///< Vector of 2 32-bit signed integers
typedef float32_t		AKSIMD_F32;			///< 32-bit float
typedef float32x2_t		AKSIMD_V2F32;		///< Vector of 2 32-bit floats
typedef float32x4_t		AKSIMD_V4F32;		///< Vector of 4 32-bit floats

typedef uint32x4_t		AKSIMD_V4COND;		///< Vector of 4 comparison results
typedef uint32x4_t		AKSIMD_V4ICOND;		///< Vector of 4 comparison results
typedef uint32x4_t		AKSIMD_V4FCOND;		///< Vector of 4 comparison results

#if defined(AK_CPU_ARM_NEON)
typedef float32x2x2_t	AKSIMD_V2F32X2;
typedef float32x4x2_t	AKSIMD_V4F32X2;
typedef float32x4x4_t	AKSIMD_V4F32X4;

typedef int32x4x2_t		AKSIMD_V4I32X2;
typedef int32x4x4_t		AKSIMD_V4I32X4;
#endif

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD loading / setting
//@{

/// Loads four single-precision, floating-point values (see _mm_load_ps)
#define AKSIMD_LOAD_V4F32( __addr__ ) vld1q_f32( (float32_t*)(__addr__) )

/// Loads four single-precision floating-point values from unaligned
/// memory (see _mm_loadu_ps)
#define AKSIMD_LOADU_V4F32( __addr__ ) vld1q_f32( (float32_t*)(__addr__) )

/// Loads a single single-precision, floating-point value, copying it into
/// all four words (see _mm_load1_ps, _mm_load_ps1)
#define AKSIMD_LOAD1_V4F32( __scalar__ ) vld1q_dup_f32( (float32_t*)(&(__scalar__)) )

/// Sets the four single-precision, floating-point values to __scalar__ (see
/// _mm_set1_ps, _mm_set_ps1)
#define AKSIMD_SET_V4F32( __scalar__ ) vdupq_n_f32( __scalar__ )

/// Sets the four integer values to __scalar__
#define AKSIMD_SET_V4I32( __scalar__ ) vdupq_n_s32( __scalar__ )

static AkForceInline int32x4_t AKSIMD_SETV_V4I32(int32_t d, int32_t c, int32_t b, int32_t a) {
	int32x4_t ret = vdupq_n_s32(0);
	ret = vsetq_lane_s32(d, ret, 3);
	ret = vsetq_lane_s32(c, ret, 2);
	ret = vsetq_lane_s32(b, ret, 1);
	ret = vsetq_lane_s32(a, ret, 0);
	return ret;
}

static AkForceInline int32x4_t AKSIMD_SETV_V2I64(int64_t b, int64_t a) {
	// On 32b ARM, dealing with an int64_t could invoke loading in memory directly,
	// e.g. dereferencing a 64b ptr as one of the inputs
	// ultimately resulting in a potentially unaligned 64b load.
	// By reinterpreting and using the 64b inputs as 32b inputs, even a load from
	// a pointer will not have any alignment requirements
	// ARM64 can handle dereferencing ptrs to 64b values directly safely, though
#if defined AK_CPU_ARM_64
	int64x2_t ret = vdupq_n_s64(0);
	ret = vsetq_lane_s64(b, ret, 1);
	ret = vsetq_lane_s64(a, ret, 0);
	return vreinterpretq_s32_s64(ret);
#else
	int32x4_t ret = vdupq_n_s32(0);
	ret = vsetq_lane_s32(int32_t((b >> 32) & 0xFFFFFFFF), ret, 3);
	ret = vsetq_lane_s32(int32_t((b >>  0) & 0xFFFFFFFF), ret, 2);
	ret = vsetq_lane_s32(int32_t((a >> 32) & 0xFFFFFFFF), ret, 1);
	ret = vsetq_lane_s32(int32_t((a >>  0) & 0xFFFFFFFF), ret, 0);
	return ret;
#endif
}

/// Sets the four single-precision, floating-point values to zero (see
/// _mm_setzero_ps)
#define AKSIMD_SETZERO_V4F32() AKSIMD_SET_V4F32( 0 )

/// Loads a single-precision, floating-point value into the low word
/// and clears the upper three words.
/// r0 := *p; r1 := 0.0 ; r2 := 0.0 ; r3 := 0.0 (see _mm_load_ss)
#define AKSIMD_LOAD_SS_V4F32( __addr__ ) vld1q_lane_f32( (float32_t*)(__addr__), AKSIMD_SETZERO_V4F32(), 0 );

/// Loads four 32-bit signed integer values (aligned)
#define AKSIMD_LOAD_V4I32( __addr__ ) vld1q_s32( (const int32_t*)(__addr__) )

/// Loads 8 16-bit signed integer values (aligned)
#define AKSIMD_LOAD_V8I16( __addr__ ) vld1q_s16( (const int16_t*)(__addr__) )

/// Loads 4 16-bit signed integer values (aligned)
#define AKSIMD_LOAD_V4I16( __addr__ ) vld1_s16( (const int16_t*)(__addr__) )

/// Loads unaligned 128-bit value (see _mm_loadu_si128)
#define AKSIMD_LOADU_V4I32( __addr__ ) vld1q_s32( (const int32_t*)(__addr__))
/// Sets the four 32-bit integer values to zero (see _mm_setzero_si128)
#define AKSIMD_SETZERO_V4I32() vdupq_n_s32( 0 )

/// Loads two single-precision, floating-point values
#define AKSIMD_LOAD_V2F32( __addr__ ) vld1_f32( (float32_t*)(__addr__) )
#define AKSIMD_LOAD_V2F32_LANE( __addr__, __vec__, __lane__ ) vld1_lane_f32( (float32_t*)(__addr__), (__vec__), (__lane__) );

/// Sets the two single-precision, floating-point values to __scalar__
#define AKSIMD_SET_V2F32( __scalar__ ) vdup_n_f32( __scalar__ )

/// Sets the two single-precision, floating-point values to zero
#define AKSIMD_SETZERO_V2F32() AKSIMD_SET_V2F32( 0 )

/// Loads data from memory and de-interleaves
#define AKSIMD_LOAD_V4F32X2( __addr__ ) vld2q_f32( (float32_t*)(__addr__) )
#define AKSIMD_LOAD_V2F32X2( __addr__ ) vld2_f32( (float32_t*)(__addr__) )

/// Loads data from memory and de-interleaves; only selected lane
#define AKSIMD_LOAD_V2F32X2_LANE( __addr__, __vec__, __lane__ ) vld2_lane_f32( (float32_t*)(__addr__), (__vec__), (__lane__) );
#define AKSIMD_LOAD_V4F32X4_LANE( __addr__, __vec__, __lane__ ) vld4q_lane_f32( (float32_t*)(__addr__), (__vec__), (__lane__) );

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD storing
//@{

/// Stores four single-precision, floating-point values. The address must be 16-byte aligned
#define AKSIMD_STORE_V4F32( __addr__, __vName__ ) vst1q_f32( (float32_t*)(__addr__), (__vName__) )

/// Stores four single-precision, floating-point values. The address does not need to be 16-byte aligned.
#define AKSIMD_STOREU_V4F32( __addr__, __vec__ ) vst1q_f32( (float32_t*)(__addr__), (__vec__) )

/// Stores the lower single-precision, floating-point value.
/// *p := a0 (see _mm_store_ss)
#define AKSIMD_STORE1_V4F32( __addr__, __vec__ ) vst1q_lane_f32( (float32_t*)(__addr__), (__vec__), 0 )

/// Stores four 32-bit integer values. The address must be 16-byte aligned.
#define AKSIMD_STORE_V4I32( __addr__, __vec__ ) vst1q_s32( (int32_t*)(__addr__), (__vec__) )

/// Stores four 32-bit integer values. The address does not need to be 16-byte aligned.
#define AKSIMD_STOREU_V4I32( __addr__, __vec__ ) vst1q_s32( (int32_t*)(__addr__), (__vec__) )

/// Stores four 32-bit unsigned integer values. The address does not need to be 16-byte aligned.
#define AKSIMD_STOREU_V4UI32( __addr__, __vec__ ) vst1q_u32( (uint32_t*)(__addr__), (__vec__) )

/// Stores two single-precision, floating-point values. The address must be 16-byte aligned.
#define AKSIMD_STORE_V2F32( __addr__, __vName__ ) vst1_f32( (AkReal32*)(__addr__), (__vName__) )

/// Stores data by interleaving into memory
#define AKSIMD_STORE_V4F32X2( __addr__, __vName__ ) vst2q_f32( (float32_t*)(__addr__), (__vName__) )
#define AKSIMD_STORE_V2F32X2( __addr__, __vName__ ) vst2_f32( (float32_t*)(__addr__), (__vName__) )

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD conversion
//@{

/// Converts the four signed 32-bit integer values of a to single-precision,
/// floating-point values (see _mm_cvtepi32_ps)
#define AKSIMD_CONVERT_V4I32_TO_V4F32( __vec__ ) vcvtq_f32_s32( __vec__ )

/// Converts the four single-precision, floating-point values of a to signed
/// 32-bit integer values (see _mm_cvtps_epi32)
static AkForceInline AKSIMD_V4I32 AKSIMD_ROUND_V4F32_TO_V4I32(AKSIMD_V4F32 a) {
#if defined AK_CPU_ARM_64
	return vcvtaq_s32_f32(a);
#else
	// on ARMv7, need to add 0.5 (away from zero) and truncate that
	float32x4_t halfPos = vdupq_n_f32(0.5f);
	float32x4_t halfNeg = vdupq_n_f32(-0.5f);
	float32x4_t zero = vdupq_n_f32(0.0f);
	const uint32x4_t signMask = vcgtq_f32(a, zero);
	const float32x4_t signedHalf = vbslq_f32(signMask, halfPos, halfNeg);
	const float32x4_t aOffset = vaddq_f32(a, signedHalf);
	return vcvtq_s32_f32(aOffset);
#endif
}

/// Converts the four single-precision, floating-point values of a to signed
/// 32-bit integer values by truncating (see _mm_cvttps_epi32)
#define AKSIMD_TRUNCATE_V4F32_TO_V4I32( __vec__ ) vcvtq_s32_f32( (__vec__) )

/// Converts the 4 half-precision floats in the lower 64-bits of the provided
/// vector to 4 full-precision floats 
#define AKSIMD_CONVERT_V4F16_TO_V4F32_LO(__vec__) AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER( vreinterpret_u16_s32(vget_low_s32( __vec__)))

/// Converts the 4 half-precision floats in the upper 64-bits of the provided
/// vector to 4 full-precision floats 
#define AKSIMD_CONVERT_V4F16_TO_V4F32_HI(__vec__) AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER( vreinterpret_u16_s32(vget_high_s32( __vec__)))


static AkForceInline AKSIMD_V4F32 AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER(uint16x4_t vecs16)
{
#if defined(AK_CPU_ARM_64)

	// float16 intrinsics were added in gcc 6.1, and we still use gcc4.9 on Android
	// all compilers that we support for arm64 - i.e. clang/msvc - support the intrinsics just fine
#if (defined(__GNUC__) && !defined(__llvm__)) && (__GNUC__ < 6 || __GNUC__ == 6 && __GNUC_MINOR__ < 1)
	float32x4_t ret;
	__asm__("fcvtl   %0.4s, %1.4h"    \
		: "=w"(ret) \
		: "w"(vecs16)
	);
	return ret;
#else
	float16x4_t vecf16 = vreinterpret_f16_s16(vecs16);
	return vcvt_f32_f16(vecf16);
#endif
#else
	uint32x4_t vecExtended = vshlq_n_u32(vmovl_u16(vecs16), 16);
	uint32x4_t expMantData = vandq_u32(vecExtended, vdupq_n_u32(0x7fff0000));
	uint32x4_t expMantShifted = vshrq_n_u32(expMantData, 3); // shift so that the float16 exp/mant is now split along float32's bounds

	// Determine if value is denorm or not, and use that to determine how much exponent should be scaled by, and if we need post-scale fp adjustment
	uint32x4_t isDenormMask = vcltq_u32(expMantData, vdupq_n_u32(0x03ff0000));
	uint32x4_t exponentIncrement = vbslq_u32(isDenormMask, vdupq_n_u32(0x38800000), vdupq_n_u32(0x38000000));
	uint32x4_t postIncrementAdjust = vandq_u32(isDenormMask, vdupq_n_u32(0x38800000));

	// Apply the exponent increment and adjust
	uint32x4_t expMantScaled = vaddq_u32(expMantShifted, exponentIncrement);
	uint32x4_t expMantAdj = vreinterpretq_u32_f32(vsubq_f32(vreinterpretq_f32_u32(expMantScaled), vreinterpretq_f32_u32(postIncrementAdjust)));

	// if fp16 val was inf or nan, preserve the inf/nan exponent field (we can just 'or' inf-nan 
	uint32x4_t isInfnanMask = vcgtq_u32(expMantData, vdupq_n_u32(0x7bffffff));
	uint32x4_t infnanExp = vandq_u32(isInfnanMask, vdupq_n_u32(0x7f800000));
	uint32x4_t expMantWithInfNan = vorrq_u32(expMantAdj, infnanExp);

	// reincorporate the sign
	uint32x4_t signData = vandq_u32(vecExtended, vdupq_n_u32(0x80000000));
	float32x4_t assembledFloat = vreinterpretq_f32_u32(vorrq_u32(signData, expMantWithInfNan));
	return assembledFloat;
#endif
}


/// Converts the 4 full-precision floats vector to 4 half-precision floats 
/// occupying the lower bits and leaving the upper bits as zero
static AkForceInline AKSIMD_V4I32 AKSIMD_CONVERT_V4F32_TO_V4F16(AKSIMD_V4F32 vec)
{
#if defined(AK_CPU_ARM_64)

	// float16 intrinsics were added in gcc 6.1, and we still use gcc4.9 on Android
	// all compilers that we support for arm64 - i.e. clang/msvc - support the intrinsics just fine
#if (defined(__GNUC__) && !defined(__llvm__)) && (__GNUC__ < 6 || __GNUC__ == 6 && __GNUC_MINOR__ < 1)
	int32x4_t ret;
	__asm__("fcvtn   %1.4h, %1.4s\n"    \
		"\tmov     %0.8b, %1.8b"	\
		: "=w"(ret) \
		: "w"(vec)
	);
	return ret;
#else
	float16x4_t ret = vcvt_f16_f32(vec);
	return vreinterpretq_s32_s16(vcombine_s16(vreinterpret_s16_f16(ret), vdup_n_s16(0)));
#endif // __GNUC__

#else
	uint32x4_t signData = vandq_u32(vreinterpretq_u32_f32(vec), vdupq_n_u32(0x80000000));
	uint32x4_t unsignedVec = vandq_u32(vreinterpretq_u32_f32(vec), vdupq_n_u32(0x7fffffff));

	// do the processing for values that will be denormed in float16
	// Add 0.5 to get value within range, and rounde; then move mantissa data up
	float32x4_t denormedVec = vaddq_f32(vreinterpretq_f32_u32(unsignedVec), vdupq_n_f32(0.5f));
	uint32x4_t denormResult = vshlq_n_u32(vreinterpretq_u32_f32(denormedVec), 16);

	// processing for values that will be normal in float16
	uint32x4_t subnormMagic = vdupq_n_u32(0xC8000FFF); // -131072 + rounding bias
	uint32x4_t normRoundPart1 = vaddq_u32(unsignedVec, subnormMagic);
	uint32x4_t mantLsb = vshlq_n_u32(unsignedVec, 31 - 13);
	uint32x4_t mantSignExtendLsb = vshrq_n_u32(mantLsb, 31); // Extend Lsb so that it's -1 when set
	uint32x4_t normRoundPart2 = vsubq_u32(normRoundPart1, mantSignExtendLsb); // and subtract the sign-extended bit to finish rounding up
	uint32x4_t normResult = vshlq_n_u32(normRoundPart2, 3);

	// Combine the norm and subnorm paths together
	uint32x4_t normalMinimum = vdupq_n_u32((127 - 14) << 23); // smallest float32 that yields a normalized float16
	uint32x4_t denormMask = vcgtq_u32(normalMinimum, unsignedVec);

	uint32x4_t nonNanFloat = vbslq_u32(denormMask, denormResult, normResult);

	// apply inf/nan check
	uint32x4_t isNotInfNanMask = vcltq_u32(unsignedVec, vdupq_n_u32(0x47800000)); // test if exponent bits are zero or not
	uint32x4_t mantissaData = vandq_u32(unsignedVec, vdupq_n_u32(0x007fffff));
	uint32x4_t isNanMask = vmvnq_u32(vceqq_f32(vec, vec)); // mark the parts of the vector where we have a mantissa (i.e. NAN) as 0xffffffff
	uint32x4_t nantissaBit = vandq_u32(isNanMask, vdupq_n_u32(0x02000000)); // set the NaN mantissa bit if mantissa suggests this is NaN
	uint32x4_t infData = vandq_u32(vmvnq_u32(mantissaData), vdupq_n_u32(0x7c000000)); // grab the exponent data from unsigned vec with no mantissa
	uint32x4_t infNanData = vorrq_u32(infData, nantissaBit); // if we have a non-zero mantissa, add the NaN mantissa bit

	uint32x4_t resultWithInfNan = vbslq_u32(isNotInfNanMask, nonNanFloat, infNanData); // and combine the results

	// reincorporate the original sign
	uint32x4_t signedResult = vorrq_u32(signData, resultWithInfNan);

	// store results packed in lower 64 bits, and set upper 64 to zero
	uint16x8x2_t resultZip = vuzpq_u16(vreinterpretq_u16_u32(signedResult), vdupq_n_u16(0));
	return vreinterpretq_s32_u16(resultZip.val[1]);
#endif
}

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD logical operations
//@{

/// Computes the bitwise AND of the 128-bit value in a and the
/// 128-bit value in b (see _mm_and_si128)
#define AKSIMD_AND_V4I32( __a__, __b__ ) vandq_s32( (__a__), (__b__) )

/// Compares the 8 signed 16-bit integers in a and the 8 signed
/// 16-bit integers in b for greater than (see _mm_cmpgt_epi16)
#define AKSIMD_CMPGT_V8I16( __a__, __b__ ) \
	vreinterpretq_s32_u16( vcgtq_s16( vreinterpretq_s16_s32(__a__), vreinterpretq_s16_s32(__b__) ) )

/// Compares for less than or equal (see _mm_cmple_ps)
#define AKSIMD_CMPLE_V4F32( __a__, __b__ ) vcleq_f32( (__a__), (__b__) )

#define AKSIMD_CMPLT_V4I32( __a__, __b__) vreinterpretq_s32_u32(vcltq_s32(__a__, __b__))
#define AKSIMD_CMPGT_V4I32( __a__, __b__)  vreinterpretq_s32_u32(vcgtq_s32(__a__,__b__))

#define AKSIMD_OR_V4I32( __a__, __b__ ) vorrq_s32(__a__,__b__)

#define AKSIMD_XOR_V4I32(__a__, __b__)  veorq_s32(__a__, __b__)

static AkForceInline AKSIMD_V4F32 AKSIMD_XOR_V4F32( const AKSIMD_V4F32& in_vec0, const AKSIMD_V4F32& in_vec1 )
{
	uint32x4_t t0 = vreinterpretq_u32_f32(in_vec0);
	uint32x4_t t1 = vreinterpretq_u32_f32(in_vec1);
	uint32x4_t res = veorq_u32(t0, t1);
	return vreinterpretq_f32_u32(res);
}

#define AKSIMD_SUB_V4I32(__a__, __b__) vsubq_s32(__a__, __b__)
//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shifting
//@{

/// Shifts the 4 signed or unsigned 32-bit integers in a left by
/// in_shiftBy bits while shifting in zeros (see _mm_slli_epi32)
#define AKSIMD_SHIFTLEFT_V4I32( __vec__, __shiftBy__ ) \
	vshlq_n_s32( (__vec__), (__shiftBy__) )

/// Shifts the 4 signed 32-bit integers in a right by in_shiftBy
/// bits while shifting in the sign bit (see _mm_srai_epi32)
#define AKSIMD_SHIFTRIGHTARITH_V4I32( __vec__, __shiftBy__ ) \
	vshrq_n_s32( (__vec__), (__shiftBy__) )

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shuffling
//@{

// Macro for combining two vector of 2 elements into one vector of
// 4 elements.
#define AKSIMD_COMBINE_V2F32( a, b ) vcombine_f32( a, b )

// Macro for shuffle parameter for AKSIMD_SHUFFLE_V4F32() (see _MM_SHUFFLE)
#define AKSIMD_SHUFFLE( fp3, fp2, fp1, fp0 ) \
	(((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | ((fp0)))

/// Selects four specific single-precision, floating-point values from
/// a and b, based on the mask i (see _mm_shuffle_ps)
// Usage: AKSIMD_SHUFFLE_V4F32( vec1, vec2, AKSIMD_SHUFFLE( z, y, x, w ) )
// If you get a link error, it's probably because the required
// _AKSIMD_LOCAL::SHUFFLE_V4F32< zyxw > is not implemented in
// <AK/SoundEngine/Platforms/arm_neon/AkSimdShuffle.h>.
#define AKSIMD_SHUFFLE_V4F32( a, b, zyxw ) \
	_AKSIMD_LOCAL::SHUFFLE_V4F32< zyxw >( a, b )

#define AKSIMD_SHUFFLE_V4I32( a, b, zyxw ) vreinterpretq_s32_f32(AKSIMD_SHUFFLE_V4F32( vreinterpretq_f32_s32(a), vreinterpretq_f32_s32(b), zyxw ))

/// Barrel-shift all floats by one.
#define AKSIMD_SHUFFLE_BCDA( __a__ ) AKSIMD_SHUFFLE_V4F32( (__a__), (__a__), AKSIMD_SHUFFLE(0,3,2,1))
	
// Various combinations of zyxw for _AKSIMD_LOCAL::SHUFFLE_V4F32< zyxw > are
// implemented in a separate header file to keep this one cleaner:
#include <AK/SoundEngine/Platforms/arm_neon/AkSimdShuffle.h>

/// Moves the upper two single-precision, floating-point values of b to
/// the lower two single-precision, floating-point values of the result.
/// The upper two single-precision, floating-point values of a are passed
/// through to the result.
/// r3 := a3; r2 := a2; r1 := b3; r0 := b2 (see _mm_movehl_ps)
inline AKSIMD_V4F32 AKSIMD_MOVEHL_V4F32( const AKSIMD_V4F32 abcd, const AKSIMD_V4F32 xyzw ) 
{
		//return akshuffle_zwcd( xyzw, abcd );
		AKSIMD_V2F32 zw = vget_high_f32( xyzw );
		AKSIMD_V2F32 cd = vget_high_f32( abcd );
		AKSIMD_V4F32 zwcd = vcombine_f32( zw , cd );
		return zwcd;
}

/// Moves the lower two single-precision, floating-point values of b to
/// the upper two single-precision, floating-point values of the result.
/// The lower two single-precision, floating-point values of a are passed
/// through to the result.
/// r3 := b1 ; r2 := b0 ; r1 := a1 ; r0 := a0 (see _mm_movelh_ps)
inline AKSIMD_V4F32 AKSIMD_MOVELH_V4F32( const AKSIMD_V4F32& xyzw, const AKSIMD_V4F32& abcd )
{
	return vcombine_f32( vget_low_f32( xyzw ) , vget_low_f32( abcd ) );
}

/// Swap the 2 lower floats together and the 2 higher floats together.	
//#define AKSIMD_SHUFFLE_BADC( __a__ ) AKSIMD_SHUFFLE_V4F32( (__a__), (__a__), AKSIMD_SHUFFLE(2,3,0,1))
#define AKSIMD_SHUFFLE_BADC( __a__ ) vrev64q_f32( __a__ )

/// Swap the 2 lower floats with the 2 higher floats.	
//#define AKSIMD_SHUFFLE_CDAB( __a__ ) AKSIMD_SHUFFLE_V4F32( (__a__), (__a__), AKSIMD_SHUFFLE(1,0,3,2))
#define AKSIMD_SHUFFLE_CDAB( __a__ ) vcombine_f32( vget_high_f32(__a__), vget_low_f32(__a__) )

/// Duplicates the odd items into the even items (d c b a -> d d b b )
#define AKSIMD_DUP_ODD(__vv) AKSIMD_SHUFFLE_V4F32(__vv, __vv, AKSIMD_SHUFFLE(3,3,1,1))

/// Duplicates the even items into the odd items (d c b a -> c c a a )
#define AKSIMD_DUP_EVEN(__vv) AKSIMD_SHUFFLE_V4F32(__vv, __vv, AKSIMD_SHUFFLE(2,2,0,0))

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD arithmetic
//@{

/// Subtracts the four single-precision, floating-point values of
/// a and b (a - b) (see _mm_sub_ps)
#define AKSIMD_SUB_V4F32( __a__, __b__ ) vsubq_f32( (__a__), (__b__) )

/// Subtracts the two single-precision, floating-point values of
/// a and b
#define AKSIMD_SUB_V2F32( __a__, __b__ ) vsub_f32( (__a__), (__b__) )

/// Subtracts the lower single-precision, floating-point values of a and b.
/// The upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 - b0 ; r1 := a1 ; r2 := a2 ; r3 := a3 (see _mm_sub_ss)
#define AKSIMD_SUB_SS_V4F32( __a__, __b__ ) \
	vsubq_f32( (__a__), vsetq_lane_f32( AKSIMD_GETELEMENT_V4F32( (__b__), 0 ), AKSIMD_SETZERO_V4F32(), 0 ) );

/// Adds the four single-precision, floating-point values of
/// a and b (see _mm_add_ps)
#define AKSIMD_ADD_V4F32( __a__, __b__ ) vaddq_f32( (__a__), (__b__) )

/// Adds the two single-precision, floating-point values of
/// a and b
#define AKSIMD_ADD_V2F32( __a__, __b__ ) vadd_f32( (__a__), (__b__) )

/// Adds the four integers of a and b
#define AKSIMD_ADD_V4I32( __a__, __b__ ) vaddq_s32( (__a__), (__b__) )

/// Multiplies the 4 low-parts of both operand into the 4 32-bit integers (no overflow)
#define AKSIMD_MULLO16_V4I32( __a__, __b__ ) vmulq_s32(__a__, __b__)

/// Compare the content of four single-precision, floating-point values of
/// a and b
#define AKSIMD_COMP_V4F32( __a__, __b__ ) vceqq_f32( (__a__), (__b__) )

/// Compare the content of two single-precision, floating-point values of
/// a and b
#define AKSIMD_COMP_V2F32( __a__, __b__ ) vceq_f32( (__a__), (__b__) )

/// Adds the lower single-precision, floating-point values of a and b; the
/// upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 + b0; r1 := a1; r2 := a2; r3 := a3 (see _mm_add_ss)
#define AKSIMD_ADD_SS_V4F32( __a__, __b__ ) \
	vaddq_f32( (__a__), vsetq_lane_f32( AKSIMD_GETELEMENT_V4F32( (__b__), 0 ), AKSIMD_SETZERO_V4F32(), 0 ) )

/// Multiplies the four single-precision, floating-point values
/// of a and b (see _mm_mul_ps)
#define AKSIMD_MUL_V4F32( __a__, __b__ ) vmulq_f32( (__a__), (__b__) )

/// Multiplies the four single-precision, floating-point values of a
/// by the single-precision, floating-point scalar b
#define AKSIMD_MUL_V4F32_SCALAR( __a__, __b__ ) vmulq_n_f32( (__a__), (__b__) )

/// Rough estimation of division
AkForceInline AKSIMD_V4F32 AKSIMD_DIV_V4F32( AKSIMD_V4F32 a, AKSIMD_V4F32 b ) 
{
	AKSIMD_V4F32 inv = vrecpeq_f32(b);
	AKSIMD_V4F32 restep = vrecpsq_f32(b, inv);
	inv = vmulq_f32(restep, inv);
	return vmulq_f32(a, inv);
}

/// Multiplies the two single-precision, floating-point values
/// of a and b
#define AKSIMD_MUL_V2F32( __a__, __b__ ) vmul_f32( (__a__), (__b__) )

/// Multiplies the two single-precision, floating-point values of a
/// by the single-precision, floating-point scalar b
#define AKSIMD_MUL_V2F32_SCALAR( __a__, __b__ ) vmul_n_f32( (__a__), (__b__) )

/// Multiplies the lower single-precision, floating-point values of
/// a and b; the upper three single-precision, floating-point values
/// are passed through from a.
/// r0 := a0 * b0; r1 := a1; r2 := a2; r3 := a3 (see _mm_add_ss)
#define AKSIMD_MUL_SS_V4F32( __a__, __b__ ) \
	vmulq_f32( (__a__), vsetq_lane_f32( AKSIMD_GETELEMENT_V4F32( (__b__), 0 ), AKSIMD_SETZERO_V4F32(), 0 ) )

/// Vector multiply-add operation.
#define AKSIMD_MADD_V4F32( __a__, __b__, __c__ ) vmlaq_f32( (__c__), (__a__), (__b__) )

/// Vector multiply-substract operation.  Careful: vmlsq_f32 does c-(a*b) and not the expected (a*b)-c
#define AKSIMD_MSUB_V4F32( __a__, __b__, __c__ ) \
	AKSIMD_SUB_V4F32( AKSIMD_MUL_V4F32( (__a__), (__b__) ), (__c__) )


#define AKSIMD_MADD_V2F32( __a__, __b__, __c__ ) \
	AKSIMD_ADD_V2F32( AKSIMD_MUL_V2F32( (__a__), (__b__) ), (__c__) )

#define AKSIMD_MSUB_V2F32( __a__, __b__, __c__ ) \
	AKSIMD_SUB_V2F32( AKSIMD_MUL_V2F32( (__a__), (__b__) ), (__c__) )

#define AKSIMD_MADD_V4F32_SCALAR( __a__, __b__, __c__ ) vmlaq_n_f32( (__c__), (__a__), (__b__) )
#define AKSIMD_MADD_V2F32_SCALAR( __a__, __b__, __c__ ) vmla_n_f32( (__c__), (__a__), (__b__) )

/// Vector multiply-add operation.
AkForceInline AKSIMD_V4F32 AKSIMD_MADD_SS_V4F32( const AKSIMD_V4F32& __a__, const AKSIMD_V4F32& __b__, const AKSIMD_V4F32& __c__ )
{
	return AKSIMD_ADD_SS_V4F32( AKSIMD_MUL_SS_V4F32( __a__, __b__ ), __c__ );
}

/// Computes the minima of the four single-precision, floating-point
/// values of a and b (see _mm_min_ps)
#define AKSIMD_MIN_V4F32( __a__, __b__ ) vminq_f32( (__a__), (__b__) )

/// Computes the minima of the two single-precision, floating-point
/// values of a and b
#define AKSIMD_MIN_V2F32( __a__, __b__ ) vmin_f32( (__a__), (__b__) )

/// Computes the maximums of the four single-precision, floating-point
/// values of a and b (see _mm_max_ps)
#define AKSIMD_MAX_V4F32( __a__, __b__ ) vmaxq_f32( (__a__), (__b__) )

/// Computes the maximums of the two single-precision, floating-point
/// values of a and b
#define AKSIMD_MAX_V2F32( __a__, __b__ ) vmax_f32( (__a__), (__b__) )

/// Returns absolute value
#define AKSIMD_ABS_V4F32( __a__ ) vabsq_f32((__a__))

/// Changes the sign
#define AKSIMD_NEG_V2F32( __a__ ) vneg_f32( (__a__) )
#define AKSIMD_NEG_V4F32( __a__ ) vnegq_f32( (__a__) )

/// Square root (4 floats)
#define AKSIMD_SQRT_V4F32( __vec__ ) vrecpeq_f32( vrsqrteq_f32( __vec__ ) )

/// Vector reciprocal square root approximation 1/sqrt(a), or equivalently, sqrt(1/a)
#define AKSIMD_RSQRT_V4F32( __a__ ) vrsqrteq_f32( (__a__) )

/// Square root (2 floats)
#define AKSIMD_SQRT_V2F32( __vec__ ) vrecpe_f32( vrsqrte_f32( __vec__ ) )

/// Reciprocal of x (1/x)
#define AKSIMD_RECIP_V4F32(__a__) vrecpeq_f32(__a__)

/// Faked in-place vector horizontal add. 
/// \akwarning
/// Don't expect this to be very efficient. 
/// \endakwarning
static AkForceInline AKSIMD_V4F32 AKSIMD_HORIZONTALADD_V4F32( AKSIMD_V4F32 vVec )
{   
	AKSIMD_V4F32 vAb = AKSIMD_SHUFFLE_V4F32(vVec, vVec, 0xB1);
	AKSIMD_V4F32 vHaddAb = AKSIMD_ADD_V4F32(vVec, vAb);
	AKSIMD_V4F32 vHaddCd = AKSIMD_SHUFFLE_V4F32(vHaddAb, vHaddAb, 0x4E);
	AKSIMD_V4F32 vHaddAbcd = AKSIMD_ADD_V4F32(vHaddAb, vHaddCd);
	return vHaddAbcd;
} 

/// Cross-platform SIMD multiplication of 2 complex data elements with interleaved real and imaginary parts
static AkForceInline AKSIMD_V4F32 AKSIMD_COMPLEXMUL_V4F32( AKSIMD_V4F32 vCIn1, AKSIMD_V4F32 vCIn2 )
{
#ifdef AKSIMD_DECLARE_V4F32
	static const AKSIMD_DECLARE_V4F32( vSign, -0.f, 0.f, -0.f, 0.f);
#else
	static const AKSIMD_V4F32 vSign = { -0.f, 0.f, -0.f, 0.f };
#endif

	float32x4x2_t vC2Ext = vtrnq_f32(vCIn2, vCIn2); // val[0] will be reals extended, val[1] will be imag
	vC2Ext.val[1] = AKSIMD_XOR_V4F32(vC2Ext.val[1], vSign);
	float32x4_t vC1Rev = vrev64q_f32(vCIn1);
	float32x4_t vMul = vmulq_f32(vCIn1, vC2Ext.val[0]);
	float32x4_t vFinal = vmlaq_f32(vMul, vC1Rev, vC2Ext.val[1]);
	return vFinal;
}

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD packing / unpacking
//@{

/// Interleaves the lower 4 signed or unsigned 16-bit integers in a with
/// the lower 4 signed or unsigned 16-bit integers in b (see _mm_unpacklo_epi16)
#define AKSIMD_UNPACKLO_VECTOR8I16( __a__, __b__ ) vreinterpretq_s32_s16( vzipq_s16( vreinterpretq_s16_s32(__a__), vreinterpretq_s16_s32(__b__) ).val[0] )

/// Interleaves the upper 4 signed or unsigned 16-bit integers in a with
/// the upper 4 signed or unsigned 16-bit integers in b (see _mm_unpackhi_epi16)
#define AKSIMD_UNPACKHI_VECTOR8I16( __a__, __b__ ) vreinterpretq_s32_s16( vzipq_s16( vreinterpretq_s16_s32(__a__), vreinterpretq_s16_s32(__b__) ).val[1] )

/// Selects and interleaves the lower two single-precision, floating-point
/// values from a and b (see _mm_unpacklo_ps)
AkForceInline AKSIMD_V4F32 AKSIMD_UNPACKLO_V4F32( const AKSIMD_V4F32& in_vec1, const AKSIMD_V4F32& in_vec2 )
{
	// sce_vectormath_xayb(in_vec1, in_vec2)
	float32x2_t xy = vget_low_f32( in_vec1 /*xyzw*/ );
	float32x2_t ab = vget_low_f32( in_vec2 /*abcd*/ );
	float32x2x2_t xa_yb = vtrn_f32( xy, ab );
	AKSIMD_V4F32 xayb = vcombine_f32( xa_yb.val[0], xa_yb.val[1] );
	return xayb;
}

/// Selects and interleaves the upper two single-precision, floating-point
/// values from a and b (see _mm_unpackhi_ps)
AkForceInline AKSIMD_V4F32 AKSIMD_UNPACKHI_V4F32( const AKSIMD_V4F32& in_vec1, const AKSIMD_V4F32& in_vec2 )
{
	//return sce_vectormath_zcwd( in_vec1, in_vec2 );
	float32x2_t zw = vget_high_f32( in_vec1 /*xyzw*/ );
	float32x2_t cd = vget_high_f32( in_vec2 /*abcd*/ );
	float32x2x2_t zc_wd = vtrn_f32( zw, cd );
	AKSIMD_V4F32 zcwd = vcombine_f32( zc_wd.val[0], zc_wd.val[1] );
	return zcwd;
}

/// Packs the 8 signed 32-bit integers from a and b into signed 16-bit
/// integers and saturates (see _mm_packs_epi32)
AkForceInline AKSIMD_V4I32 AKSIMD_PACKS_V4I32( const AKSIMD_V4I32& in_vec1, const AKSIMD_V4I32& in_vec2 )
{
	int16x4_t	vec1_16 = vqmovn_s32( in_vec1 );
	int16x4_t	vec2_16 = vqmovn_s32( in_vec2 );
	int16x8_t result = vcombine_s16( vec1_16, vec2_16 );
	return vreinterpretq_s32_s16( result );
}

/// V1 = {a,b}   =>   VR = {b,c}
/// V2 = {c,d}   =>
#define AKSIMD_HILO_V2F32( in_vec1, in_vec2 ) vreinterpret_f32_u32( vext_u32( vreinterpret_u32_f32( in_vec1 ), vreinterpret_u32_f32( in_vec2 ), 1 ) )

/// V1 = {a,b}   =>   V1 = {a,c}
/// V2 = {c,d}   =>   V2 = {b,d}
#define AKSIMD_TRANSPOSE_V2F32( in_vec1, in_vec2 ) vtrn_f32( in_vec1, in_vec2 )

#define AKSIMD_TRANSPOSE_V4F32( in_vec1, in_vec2 ) vtrnq_f32( in_vec1, in_vec2 )

/// V1 = {a,b}   =>   VR = {b,a}
#define AKSIMD_SWAP_V2F32( in_vec ) vrev64_f32( in_vec )

// Given four pointers, gathers 32-bits of data from each location,
// deinterleaves them as 16-bits of each, and sign-extends to 32-bits
// e.g. (*addr[0]) := (b a) 
// e.g. (*addr[1]) := (d c) 
// e.g. (*addr[2]) := (f e) 
// e.g. (*addr[3]) := (h g)
// return struct has
// val[0] := (g e c a)
// val[1] := (h f d b)
static AkForceInline AKSIMD_V4I32X2 AKSIMD_GATHER_V4I32_AND_DEINTERLEAVE_V4I32X2(AkInt16* addr3, AkInt16* addr2, AkInt16* addr1, AkInt16* addr0)
{
	int16x4x2_t ret16{ 
		vdup_n_s16(0), 
		vdup_n_s16(0)
	};

	ret16 = vld2_lane_s16(addr0, ret16, 0);
	ret16 = vld2_lane_s16(addr1, ret16, 1);
	ret16 = vld2_lane_s16(addr2, ret16, 2);
	ret16 = vld2_lane_s16(addr3, ret16, 3);

	AKSIMD_V4I32X2 ret{
		vmovl_s16(ret16.val[0]),
		vmovl_s16(ret16.val[1])
	};
	return ret;
}

// Given four pointers, gathers 64-bits of data from each location,
// deinterleaves them as 16-bits of each, and sign-extends to 32-bits
// e.g. (*addr[0]) := (d c b a) 
// e.g. (*addr[1]) := (h g f e) 
// e.g. (*addr[2]) := (l k j i) 
// e.g. (*addr[3]) := (p o n m) 
// return struct has
// val[0] := (m i e a)
// val[1] := (n j f b)
// val[2] := (o k g c)
// val[3] := (p l h d)

static AkForceInline AKSIMD_V4I32X4 AKSIMD_GATHER_V4I64_AND_DEINTERLEAVE_V4I32X4(AkInt16* addr3, AkInt16* addr2, AkInt16* addr1, AkInt16* addr0)
{
	int16x4x4_t ret16{
		vdup_n_s16(0),
		vdup_n_s16(0),
		vdup_n_s16(0),
		vdup_n_s16(0)
	};
		
	ret16 = vld4_lane_s16(addr0, ret16, 0);
	ret16 = vld4_lane_s16(addr1, ret16, 1);
	ret16 = vld4_lane_s16(addr2, ret16, 2);
	ret16 = vld4_lane_s16(addr3, ret16, 3);

	AKSIMD_V4I32X4 ret{
		vmovl_s16(ret16.val[0]),
		vmovl_s16(ret16.val[1]),
		vmovl_s16(ret16.val[2]),
		vmovl_s16(ret16.val[3])
	};
	return ret;
}

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD vector comparison
/// Apart from AKSIMD_SEL_GTEQ_V4F32, these implementations are limited to a few platforms. 
//@{

#define AKSIMD_CMP_CTRLMASK uint32x4_t

/// Compare each float element and return control mask.
#define AKSIMD_GTEQ_V4F32( __a__, __b__ ) vcgeq_f32( (__a__), (__b__))

/// Compare each float element and return control mask.
#define AKSIMD_GT_V4F32( __a__, __b__ ) vcgtq_f32( (__a__), (__b__))

/// Compare each float element and return control mask.
#define AKSIMD_LTEQ_V4F32( __a__, __b__ ) vcleq_f32( (__a__), (__b__))

/// Compare each float element and return control mask.
#define AKSIMD_LT_V4F32( __a__, __b__ ) vcltq_f32( (__a__), (__b__))

/// Compare each integer element and return control mask.
#define AKSIMD_GTEQ_V4I32( __a__, __b__ ) vcgeq_s32( (__a__), (__b__))

/// Compare each float element and return control mask.
#define AKSIMD_EQ_V4F32( __a__, __b__ ) vceqq_f32( (__a__), (__b__))

/// Compare each integer element and return control mask.
#define AKSIMD_EQ_V4I32( __a__, __b__ ) vceqq_s32( (__a__), (__b__))

/// Return a when control mask is 0, return b when control mask is non zero, control mask is in c and usually provided by above comparison operations
#define AKSIMD_VSEL_V4F32( __a__, __b__, __c__ ) vbslq_f32( (__c__), (__b__), (__a__) )

// (cond1 >= cond2) ? b : a.
#define AKSIMD_SEL_GTEQ_V4F32( __a__, __b__, __cond1__, __cond2__ ) AKSIMD_VSEL_V4F32( __a__, __b__, vcgeq_f32( __cond1__, __cond2__ ) )

// a >= 0 ? b : c ... Written, like, you know, the normal C++ operator syntax.
#define AKSIMD_SEL_GTEZ_V4F32( __a__, __b__, __c__ ) AKSIMD_VSEL_V4F32( (__c__), (__b__), vcgeq_f32( __a__, AKSIMD_SETZERO_V4F32() ) )

#define AKSIMD_SPLAT_V4F32(var, idx) vmovq_n_f32(vgetq_lane_f32(var, idx))

static AkForceInline int AKSIMD_MASK_V4F32( const AKSIMD_V4UI32& in_vec1 )
{
#ifdef AKSIMD_DECLARE_V4F32
	static const AKSIMD_DECLARE_V4I32(movemask, 1, 2, 4, 8);
	static const AKSIMD_DECLARE_V4I32(highbit, (int32_t)0x80000000, (int32_t)0x80000000, (int32_t)0x80000000, (int32_t)0x80000000);
#else
	static const uint32x4_t movemask = { 1, 2, 4, 8 };
	static const uint32x4_t highbit = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
#endif

	uint32x4_t t0 = in_vec1;
	uint32x4_t t1 = vtstq_u32(t0, highbit);
	uint32x4_t t2 = vandq_u32(t1, movemask);
	uint32x2_t t3 = vorr_u32(vget_low_u32(t2), vget_high_u32(t2));
	return vget_lane_u32(t3, 0) | vget_lane_u32(t3, 1);
}

#ifndef AK_WIN
static AkForceInline int AKSIMD_MASK_V4F32( const AKSIMD_V4F32& in_vec1 )
{
	return AKSIMD_MASK_V4F32( vreinterpretq_u32_f32(in_vec1) );
}
#endif

//@}
////////////////////////////////////////////////////////////////////////

#endif //_AKSIMD_ARM_NEON_H_

