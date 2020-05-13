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

// AkSimdAvx.h

/// \file 
/// AKSIMD - AVX implementation

#ifndef _AK_SIMD_AVX_H_
#define _AK_SIMD_AVX_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Platforms/SSE/AkSimd.h>

#if defined(AKSIMD_AVX_SUPPORTED)

#include <immintrin.h>

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD types
//@{

typedef float	AKSIMD_F32;		///< 32-bit float
typedef __m256	AKSIMD_V8F32;	///< Vector of 8 32-bit floats
typedef AKSIMD_V8F32 AKSIMD_V8COND;	 ///< Vector of 8 comparison results
typedef AKSIMD_V8F32 AKSIMD_V8FCOND;	 ///< Vector of 8 comparison results

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD loading / setting
//@{

/// Loads eight single-precision, floating-point values. The address must be 
/// 32-byte aligned (see _mm_load_ps)
#define AKSIMD_LOAD_V8F32( __addr__ ) _mm256_load_ps( (AkReal32*)(__addr__) )

/// Loads eight single-precision floating-point values from unaligned
/// memory (see _mm_loadu_ps)
/// On every modern x86 processor this performs the same as an aligned load.
#define AKSIMD_LOADU_V8F32( __addr__ ) _mm256_loadu_ps( (__addr__) )

/// Loads a single single-precision, floating-point value, copying it into
/// all eight words (see _mm_load1_ps, _mm_load_ps1)
#define AKSIMD_LOAD1_V8F32( __scalar__ ) _mm256_broadcast_ss( &(__scalar__) )

/// Sets the eight single-precision, floating-point values to in_value (see
/// _mm_set1_ps, _mm_set_ps1)
#define AKSIMD_SET_V8F32( __scalar__ ) _mm256_set1_ps( (__scalar__) )

/// Populates the full vector with the 8 floating point values provided
#define AKSIMD_SETV_V8F32( _h, _g, _f, _e, _d, _c, _b, _a ) _mm256_set_ps( (_h), (_g), (_f), (_e), (_d), (_c), (_b), (_a) )

/// Sets the eight single-precision, floating-point values to zero (see
/// _mm_setzero_ps)
#define AKSIMD_SETZERO_V8F32() _mm256_setzero_ps()

/// Loads a single-precision, floating-point value into the low word
/// and clears the upper seven words.
/// r0 := *p; r1...r7 := 0.0 (see _mm_load_ss)
#define AKSIMD_LOAD_SS_V8F32( __addr__ ) _mm256_zextps128_ps256(_mm_load_ss( (__addr__) ))

/// Loads the two m128i's provided into the output m256i a
/// Note that this should be utilized instead of, e.g. adding & utilizing a macro "AKSIMD_INSERT_V8I32(m, i, idx)"
/// Because there is no direct corresponding instruction for an insert into 256. You should load into 128s
/// and use that. Some compilers do not handle _mm256_insert_epi32 (etc) well, or even include them
#define AKSIMD_SET_V2F128( m1, m2) _mm256_setr_m128(m1, m2)

#define AKSIMD_INSERT_V2F128( a, m128, idx) _mm256_insertf128_ps(a, m128, idx)

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD storing
//@{

/// Stores eight single-precision, floating-point values. The address
/// must be 32-byte aligned (see _mm_store_ps)
#define AKSIMD_STORE_V8F32( __addr__, __vec__ ) _mm256_store_ps( (AkReal32*)(__addr__), (__vec__) )

/// Stores eight single-precision, floating-point values. The address
/// does not need to be 32-byte aligned (see _mm_storeu_ps).
/// On every modern x86 processor this performs the same as an aligned store.
#define AKSIMD_STOREU_V8F32( __addr__, __vec__ ) _mm256_storeu_ps( (AkReal32*)(__addr__), (__vec__) )

/// Stores the lower single-precision, floating-point value.
/// *p := a0 (see _mm_store_ss)
#define AKSIMD_STORE1_V8F32( __addr__, __vec__ ) _mm_store_ss( (AkReal32*)(__addr__), _mm256_castps256_ps128( (__vec__) ) )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shuffling
//@{

/// Selects eight specific single-precision, floating-point values from
/// a and b, based on the mask i within 128-bit lanes (see _mm256_shuffle_ps)
/// This means that the AKSIMD_SHUFFLE operand still picks 1 of 4 32b components
/// inside of each of the 2 128b lanes.
// Usage: AKSIMD_SHUFFLE_V8F32( vec1, vec2, AKSIMD_SHUFFLE( z, y, x, w ) )
#define AKSIMD_SHUFFLE_V8F32( a, b, i ) _mm256_shuffle_ps( a, b, i )

/// For each 128b lane, Swap the 2 lower floats together and the 2 higher floats together. ( h g f e d c b a -> g h e f c d a b )
#define AKSIMD_SHUFFLE_V8_BADC( __a__ ) AKSIMD_SHUFFLE_V8F32( (__a__), (__a__), AKSIMD_SHUFFLE(2,3,0,1))

/// For each 128b lane, Swap the 2 lower floats with the 2 higher floats. ( h g f e d c b a -> f e h g b a d c )
#define AKSIMD_SHUFFLE_V8_CDAB( __a__ ) AKSIMD_SHUFFLE_V8F32( (__a__), (__a__), AKSIMD_SHUFFLE(1,0,3,2))

/// For each 128b lane, barrel-shift all floats by one. ( h g f e d c b a -> e h g f a d c b )
#define AKSIMD_SHUFFLE_V8_BCDA( __a__ ) AKSIMD_SHUFFLE_V8F32( (__a__), (__a__), AKSIMD_SHUFFLE(0,3,2,1))

/// For each 128b lane, duplicates the odd items into the even items ( h g f e d c b a -> h h f f d d b b )
#define AKSIMD_DUP_V8_ODD(__vv) AKSIMD_SHUFFLE_V8F32(__vv, __vv, AKSIMD_SHUFFLE(3,3,1,1))

/// For each 128b lane, duplicates the even items into the odd items ( h g f e d c b a -> g g e e c c a a )
#define AKSIMD_DUP_V8_EVEN(__vv) AKSIMD_SHUFFLE_V8F32(__vv, __vv, AKSIMD_SHUFFLE(2,2,0,0))

// Macro for selection parameter for AKSIMD_PERMUTE_2X128_V8F32()
#define AKSIMD_PERMUTE128( l1, l0 ) (((l1) << 4) | (l0))

/// For each 128b lane, select one of the four input 128b lanes across a and b,
/// based on the mask i. AKSIMD_SHUFFLE can still be directly used as a control
#define AKSIMD_PERMUTE_2X128_V8F32( a, b, i ) _mm256_permute2f128_ps(a, b, i)

/// Selects the lower of each of the 128b lanes in a and b to be the result ( B A ), ( D C ) -> ( C A )
#define AKSIMD_DEINTERLEAVELANES_LO_V8F32( a, b ) AKSIMD_PERMUTE_2X128_V8F32(a, b, AKSIMD_PERMUTE128(2, 0))

/// Selects the higher of each of the 128b lanes in a and b to be the result ( B A ), ( D C) -> ( D B )
#define AKSIMD_DEINTERLEAVELANES_HI_V8F32( a, b ) AKSIMD_PERMUTE_2X128_V8F32(a, b, AKSIMD_PERMUTE128(3, 1))


//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD arithmetic
//@{

/// Subtracts the eight single-precision, floating-point values of
/// a and b (a - b) (see _mm_sub_ps)
#define AKSIMD_SUB_V8F32( a, b ) _mm256_sub_ps( a, b )

/// Subtracts the lower single-precision, floating-point values of a and b.
/// The upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 - b0 ; r1...r7 := a1...a7 (see _mm_sub_ss)
#define AKSIMD_SUB_SS_V8F32( a, b ) _mm256_sub_ps( a, _mm256_and_ps(b, _mm256_setr_epi32( -1, 0, 0, 0, 0, 0, 0, 0 ) ) )

/// Adds the eight single-precision, floating-point values of
/// a and b (see _mm_add_ps)
#define AKSIMD_ADD_V8F32( a, b ) _mm256_add_ps( a, b )

/// Adds the lower single-precision, floating-point values of a and b; the
/// upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 + b0; r1...r7 := a1...a7 (see _mm_add_ss)
#define AKSIMD_ADD_SS_V8F32( a, b ) _mm256_add_ps( a, _mm256_and_ps(b, _mm256_setr_epi32( -1, 0, 0, 0, 0, 0, 0, 0 ) ) )

/// Multiplies the eight single-precision, floating-point values
/// of a and b (see _mm_mul_ps)
#define AKSIMD_MUL_V8F32( a, b ) _mm256_mul_ps( a, b )

#define AKSIMD_DIV_V8F32( a, b ) _mm256_div_ps( a, b )

/// Multiplies the lower single-precision, floating-point values of
/// a and b; the upper three single-precision, floating-point values
/// are passed through from a.
/// r0 := a0 * b0; r1...r7 := a1...a7 (see _mm_mul_ss)
#define AKSIMD_MUL_SS_V8F32( a, b ) _mm256_mul_ps( a, _mm256_blend_ps(b, _mm256_set1_ps(1.0f), 0xfe ) )

/// Vector multiply-add operation.
#define AKSIMD_MADD_V8F32( __a__, __b__, __c__ ) _mm256_add_ps( _mm256_mul_ps( (__a__), (__b__) ), (__c__) )
#define AKSIMD_MSUB_V8F32( __a__, __b__, __c__ ) _mm256_sub_ps( _mm256_mul_ps( (__a__), (__b__) ), (__c__) )

/// Vector multiply-add operation.
#define AKSIMD_MADD_SS_V8F32( __a__, __b__, __c__ ) AKSIMD_ADD_SS_V8F32( AKSIMD_MUL_SS_V8F32( (__a__), (__b__) ), (__c__) )

/// Computes the minima of the eight single-precision, floating-point
/// values of a and b (see _mm_min_ps)
#define AKSIMD_MIN_V8F32( a, b ) _mm256_min_ps( a, b )

/// Computes the maximums of the eight single-precision, floating-point
/// values of a and b (see _mm_max_ps)
#define AKSIMD_MAX_V8F32( a, b ) _mm256_max_ps( a, b )

/// Computes the absolute value
#define AKSIMD_ABS_V8F32( a ) _mm256_andnot_ps(_mm256_set1_ps(-0.f), a)

/// Changes the sign
#define AKSIMD_NEG_V8F32( __a__ ) _mm256_xor_ps(_mm256_set1_ps(-0.f), __a__)

/// Vector square root aproximation (see _mm_sqrt_ps)
#define AKSIMD_SQRT_V8F32( __a__ ) _mm256_sqrt_ps( (__a__) )

/// Vector reciprocal square root approximation 1/sqrt(a), or equivalently, sqrt(1/a)
#define AKSIMD_RSQRT_V8F32( __a__ ) _mm256_rsqrt_ps( (__a__) )

/// Vector reciprocal 
#define AKSIMD_RECIP_V8F32( __a__ ) _mm256_rcp_ps( (__a__) )

/// Vector ceil
#define AKSIMD_CEIL_V8F32( __a__ ) _mm256_ceil_ps( (__a__) )

#define AKSIMD_XOR_V8F32( a, b ) _mm256_xor_ps(a,b)
#define AKSIMD_OR_V8F32( a, b ) _mm256_or_ps(a,b)

/// horizontal add across the entire vector -  vVec will be updated to contain the sum of every input element of vVec
/// \akwarning
/// Don't expect this to be very efficient. 
/// \endakwarning
static AkForceInline AKSIMD_V8F32 AKSIMD_HORIZONTALADD_V8F32(AKSIMD_V8F32 vVec)
{   
	__m256 vHaddAb = _mm256_hadd_ps(vVec, vVec);
	__m256 vHaddAbcd = _mm256_hadd_ps(vHaddAb, vHaddAb);
	__m256 vHaddEfgh = _mm256_permute2f128_ps(vHaddAbcd, vHaddAbcd, 0x01);
	__m256 vHaddAll = _mm256_add_ps(vHaddAbcd, vHaddEfgh);
	return vHaddAll;
} 

/// Cross-platform SIMD multiplication of 8 complex data elements with interleaved real and imaginary parts
static AkForceInline AKSIMD_V8F32 AKSIMD_COMPLEXMUL_V8F32(const AKSIMD_V8F32 cIn1, const AKSIMD_V8F32 cIn2)
{
	__m256 real1Ext = _mm256_moveldup_ps(cIn1);				// reals extended		(a3, a3, a2, a2, a1, a1, a0, a0) 
	__m256 in2Shuf = _mm256_shuffle_ps(cIn2, cIn2, 0xB1);		// shuf multiplicand	(c3, d3, c2, d2, c1, d1, c0, d0)
	__m256 imag1Ext = _mm256_movehdup_ps(cIn1);				// multiplier imag		(b3, b3, b2, b2, b1, b1, b0, b0) 
	__m256 temp = _mm256_mul_ps(imag1Ext, in2Shuf);				// temp				(b3c3, b3d3, b2c2, b2d2, b1c1, b1d1, b0c0, b0d0)
	__m256 mul = _mm256_mul_ps(real1Ext, cIn2);	//				(a3d3, a3c3, a2d2, a2c2, a1d1, a1c1, a0d0, a0c0)
	__m256 out = _mm256_addsub_ps(mul, temp); // final				(a3d3+b3c3, a3c3-b3d3, a2d2+b2c2, a2c2-b2d2, a1d1+b1c1, a1c1-b1d1, a0d0+b0c0, a0c0-b0d0)
	return out;
}

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD packing / unpacking
//@{

/// Selects and interleaves the lower two single-precision, floating-point
/// values from each 128-bit lane in a and b (see _mm_unpacklo_ps)
/// i.e. r0 := a0, r1 := b0, r2 := a1, r3 := b1, r4 := a4, r5 := b4, r6 := a5, r7 := b5
#define AKSIMD_UNPACKLO_V8F32( a, b ) _mm256_unpacklo_ps( a, b )

/// Selects and interleaves the upper two single-precision, floating-point
/// values from each 128-bit lane a and b (see _mm_unpackhi_ps)
/// i.e. r0 := a2, r1 := b2, r2 := a3, r3 := b3, r4 := a6, r5 := b6, r6 := a7, r7 := b7
#define AKSIMD_UNPACKHI_V8F32( a, b ) _mm256_unpackhi_ps( a, b )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD vector comparison
/// Apart from AKSIMD_SEL_GTEQ_V8F32, these implementations are limited to a few platforms. 
//@{

#define AKSIMD_CMP_CTRLMASKV8 __m256

/// Vector "<=" operation (see _mm_cmple_ps)
#define AKSIMD_LTEQ_V8F32( __a__, __b__ ) _mm256_cmp_ps( (__a__), (__b__), _CMP_LE_OS )

#define AKSIMD_LT_V8F32( __a__, __b__ ) _mm256_cmp_ps( (__a__), (__b__) _CMP_LT_OS )

/// Vector ">=" operation (see _mm_cmple_ps)
#define AKSIMD_GTEQ_V8F32( __a__, __b__ ) _mm256_cmp_ps( (__a__), (__b__) _CMP_GE_OS )

#define AKSIMD_GT_V8F32( __a__, __b__ ) _mm256_cmp_ps( (__a__), (__b__) _CMP_GT_OS )

/// Vector "==" operation (see _mm_cmpeq_ps)
#define AKSIMD_EQ_V8F32( __a__, __b__ ) _mm256_cmp_ps( (__a__), (__b__) _CMP_EQ_OS )

/// Return a when control mask is 0, return b when control mask is non zero, control mask is in c and usually provided by above comparison operations
static AkForceInline AKSIMD_V8F32 AKSIMD_VSEL_V8F32( AKSIMD_V8F32 vA, AKSIMD_V8F32 vB, AKSIMD_V8F32 vMask )
{
	return _mm256_blendv_ps(vA, vB, vMask);
}

// (cond1 >= cond2) ? b : a.
#define AKSIMD_SEL_GTEQ_V8F32( __a__, __b__, __cond1__, __cond2__ ) AKSIMD_VSEL_V8F32( __a__, __b__, AKSIMD_GTEQ_V8F32( __cond1__, __cond2__ ) )

// a >= 0 ? b : c ... Written, like, you know, the normal C++ operator syntax.
#define AKSIMD_SEL_GTEZ_V8F32( __a__, __b__, __c__ ) AKSIMD_VSEL_V8F32( (__c__), (__b__), AKSIMD_GTEQ_V8F32( __a__, _mm256_set1_ps(0) ) )

#define AKSIMD_SPLAT_V8F32(var, idx) AKSIMD_SHUFFLE_V8F32(var,var, AKSIMD_SHUFFLE(idx,idx,idx,idx))

#define AKSIMD_MASK_V8F32( __a__ ) _mm256_movemask_ps( __a__ )

//@}
////////////////////////////////////////////////////////////////////////

typedef __m256i	AKSIMD_V8I32;	///< Vector of 8 32-bit signed integers

typedef AKSIMD_V8I32 AKSIMD_V8ICOND;

/// Loads unaligned 256-bit value (see _mm_loadu_si128)
/// On every modern x86 processor this performs the same as an aligned load.
#define AKSIMD_LOADU_V8I32( __addr__ ) _mm256_loadu_si256( (__addr__) )

/// Loads aligned 256-bit value. The address must be 
/// 32-byte aligned (see _mm_loadu_si128).
#define AKSIMD_LOAD_V8I32( __addr__ ) _mm256_load_si256( (__addr__) )

/// Sets the eight 32-bit integer values to zero (see _mm_setzero_si128)
#define AKSIMD_SETZERO_V8I32() _mm256_setzero_si256()

/// Sets the provided scalar value at the first index of the vector, and zeroes everything else
#define AKSIMD_SET_V8I32( __scalar__ ) _mm256_set1_epi32( (__scalar__) )

/// Populates the full vector with the 8 values provided
#define AKSIMD_SETV_V8I32( _h, _g, _f, _e, _d, _c, _b, _a ) _mm256_set_epi32( (_h), (_g), (_f), (_e), (_d), (_c), (_b), (_a) )

/// Loads the two m128i's provided into the output m256i a
/// Note that this should be utilized instead of, e.g. adding & utilizing a macro "AKSIMD_INSERT_V8I32(m, i, idx)"
/// Because there is no direct corresponding instruction for an insert into 256. You should load into 128s
/// and use that. Some compilers do not handle _mm256_insert_epi32 (etc) well, or even include them
#define AKSIMD_SET_V2I128(m1, m2) _mm256_setr_m128i(m1, m2)

/// Stores eight 32-bit integer values. The address
/// must be 32-byte aligned
#define AKSIMD_STORE_V8I32( __addr__, __vec__ ) _mm256_store_si256( (__addr__), (__vec__) )

/// Stores eight 32-bit integer values. The address
/// does not need to be 32-byte aligned (see _mm_storeu_si128).
/// On every modern x86 processor this performs the same as an aligned load.
#define AKSIMD_STOREU_V8I32( __addr__, __vec__ ) _mm256_storeu_si256( (__addr__), (__vec__) )

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD conversion
//@{

/// Converts the eight signed 32-bit integer values of a to single-precision,
/// floating-point values (see _mm_cvtepi32_ps)
#define AKSIMD_CONVERT_V8I32_TO_V8F32( __vec__ ) _mm256_cvtepi32_ps( (__vec__) )

/// Converts the eight single-precision, floating-point values of a to signed
/// 32-bit integer values by rounding (see _mm_cvtps_epi32)
#define AKSIMD_CONVERT_V8F32_TO_V8I32( __vec__ ) _mm256_cvtps_epi32( (__vec__) )

/// Converts the eight single-precision, floating-point values of a to signed
/// 32-bit integer values by truncating (see _mm_cvttps_epi32)
#define AKSIMD_TRUNCATE_V8F32_TO_V8I32( __vec__ ) _mm256_cvttps_epi32( (__vec__) )

/// Converts the eight half-precision floating-point values of vec to
/// eight full-precision floating-point values
/// WARNING: Using this requires F16C support, which is not guaranteed on AVX
#define AKSIMD_CONVERT_V8F16_TO_V8F32( __vec__ ) _mm256_cvtph_ps( (__vec__) )

/// Converts the eight single-precision, floating-point values of vec to
/// eight half-precision floating-point values
/// WARNING: Using this requires F16C support, which is not guaranteed on AVX
#define AKSIMD_CONVERT_V8F32_TO_V8F16( __vec__ ) _mm256_cvtps_ph(__vec__, (_MM_FROUND_TO_NEAREST_INT ) )

//@}
////////////////////////////////////////////////////////////////////////
#endif //_AK_SIMD_AVX_H_
#endif