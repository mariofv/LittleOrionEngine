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
/// AKSIMD - SSE implementation

#ifndef _AK_SIMD_SSE_H_
#define _AK_SIMD_SSE_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include <emmintrin.h>

////////////////////////////////////////////////////////////////////////
/// @name Platform specific defines for prefetching
//@{

#define AKSIMD_ARCHCACHELINESIZE	(64)				///< Assumed cache line width for architectures on this platform
#define AKSIMD_ARCHMAXPREFETCHSIZE	(512) 				///< Use this to control how much prefetching maximum is desirable (assuming 8-way cache)		
/// Cross-platform memory prefetch of effective address assuming non-temporal data
#define AKSIMD_PREFETCHMEMORY( __offset__, __add__ ) _mm_prefetch(((char *)(__add__))+(__offset__), _MM_HINT_NTA ) 

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name Platform specific memory size alignment for allocation purposes
//@{
#define AKSIMD_ALIGNSIZE( __Size__ ) (((__Size__) + 15) & ~15)
//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD types
//@{

typedef float	AKSIMD_F32;		///< 32-bit float
typedef __m128	AKSIMD_V4F32;	///< Vector of 4 32-bit floats
typedef AKSIMD_V4F32 AKSIMD_V4COND;	 ///< Vector of 4 comparison results
typedef AKSIMD_V4F32 AKSIMD_V4FCOND;	 ///< Vector of 4 comparison results

typedef __m128i	AKSIMD_V4I32;	///< Vector of 4 32-bit signed integers

struct AKSIMD_V4I32X2 {			///< Pair of 4 32-bit signed integers
	AKSIMD_V4I32 val[2];
};

struct AKSIMD_V4I32X4 {			///< Quartet of 4 32-bit signed integers
	AKSIMD_V4I32 val[4];
};

typedef AKSIMD_V4I32 AKSIMD_V4ICOND;

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD loading / setting
//@{

/// Loads four single-precision, floating-point values (see _mm_load_ps)
#define AKSIMD_LOAD_V4F32( __addr__ ) _mm_load_ps( (AkReal32*)(__addr__) )

/// Loads four single-precision floating-point values from unaligned
/// memory (see _mm_loadu_ps)
#define AKSIMD_LOADU_V4F32( __addr__ ) _mm_loadu_ps( (__addr__) )

/// Loads a single single-precision, floating-point value, copying it into
/// all four words (see _mm_load1_ps, _mm_load_ps1)
#define AKSIMD_LOAD1_V4F32( __scalar__ ) _mm_load1_ps( &(__scalar__) )

/// Sets the four single-precision, floating-point values to in_value (see
/// _mm_set1_ps, _mm_set_ps1)
#define AKSIMD_SET_V4F32( __scalar__ ) _mm_set_ps1( (__scalar__) )

/// Sets the four single-precision, floating-point values to zero (see
/// _mm_setzero_ps)
#define AKSIMD_SETZERO_V4F32() _mm_setzero_ps()

/// Loads a single-precision, floating-point value into the low word
/// and clears the upper three words.
/// r0 := *p; r1 := 0.0 ; r2 := 0.0 ; r3 := 0.0 (see _mm_load_ss)
#define AKSIMD_LOAD_SS_V4F32( __addr__ ) _mm_load_ss( (__addr__) )

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD storing
//@{

/// Stores four single-precision, floating-point values. The address
/// must be 16-byte aligned (see _mm_store_ps)
#define AKSIMD_STORE_V4F32( __addr__, __vec__ ) _mm_store_ps( (AkReal32*)(__addr__), (__vec__) )

/// Stores four single-precision, floating-point values. The address
/// does not need to be 16-byte aligned (see _mm_storeu_ps).
#define AKSIMD_STOREU_V4F32( __addr__, __vec__ ) _mm_storeu_ps( (AkReal32*)(__addr__), (__vec__) )

/// Stores the lower single-precision, floating-point value.
/// *p := a0 (see _mm_store_ss)
#define AKSIMD_STORE1_V4F32( __addr__, __vec__ ) _mm_store_ss( (AkReal32*)(__addr__), (__vec__) )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shuffling
//@{

// Macro for shuffle parameter for AKSIMD_SHUFFLE_V4F32() (see _MM_SHUFFLE)
#define AKSIMD_SHUFFLE( fp3, fp2, fp1, fp0 ) _MM_SHUFFLE( (fp3), (fp2), (fp1), (fp0) )

/// Selects four specific single-precision, floating-point values from
/// a and b, based on the mask i (see _mm_shuffle_ps)
// Usage: AKSIMD_SHUFFLE_V4F32( vec1, vec2, AKSIMD_SHUFFLE( z, y, x, w ) )
#define AKSIMD_SHUFFLE_V4F32( a, b, i ) _mm_shuffle_ps( a, b, i )

#define AKSIMD_SHUFFLE_V4I32( a, b, i ) _mm_castps_si128(_mm_shuffle_ps( _mm_castsi128_ps(a), _mm_castsi128_ps(b), i ))

/// Moves the upper two single-precision, floating-point values of b to
/// the lower two single-precision, floating-point values of the result.
/// The upper two single-precision, floating-point values of a are passed
/// through to the result.
/// r3 := a3; r2 := a2; r1 := b3; r0 := b2 (see _mm_movehl_ps)
#define AKSIMD_MOVEHL_V4F32( a, b ) _mm_movehl_ps( a, b )

/// Moves the lower two single-precision, floating-point values of b to
/// the upper two single-precision, floating-point values of the result.
/// The lower two single-precision, floating-point values of a are passed
/// through to the result.
/// r3 := b1 ; r2 := b0 ; r1 := a1 ; r0 := a0 (see _mm_movelh_ps)
#define AKSIMD_MOVELH_V4F32( a, b ) _mm_movelh_ps( a, b )

/// Swap the 2 lower floats together and the 2 higher floats together.	
#define AKSIMD_SHUFFLE_BADC( __a__ ) _mm_shuffle_ps( (__a__), (__a__), _MM_SHUFFLE(2,3,0,1))

/// Swap the 2 lower floats with the 2 higher floats.	
#define AKSIMD_SHUFFLE_CDAB( __a__ ) _mm_shuffle_ps( (__a__), (__a__), _MM_SHUFFLE(1,0,3,2))

/// Barrel-shift all floats by one.
#define AKSIMD_SHUFFLE_BCDA( __a__ ) AKSIMD_SHUFFLE_V4F32( (__a__), (__a__), _MM_SHUFFLE(0,3,2,1))

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
#define AKSIMD_SUB_V4F32( a, b ) _mm_sub_ps( a, b )

/// Subtracts the lower single-precision, floating-point values of a and b.
/// The upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 - b0 ; r1 := a1 ; r2 := a2 ; r3 := a3 (see _mm_sub_ss)
#define AKSIMD_SUB_SS_V4F32( a, b ) _mm_sub_ss( a, b )

/// Adds the four single-precision, floating-point values of
/// a and b (see _mm_add_ps)
#define AKSIMD_ADD_V4F32( a, b ) _mm_add_ps( a, b )

/// Adds the lower single-precision, floating-point values of a and b; the
/// upper three single-precision, floating-point values are passed through from a.
/// r0 := a0 + b0; r1 := a1; r2 := a2; r3 := a3 (see _mm_add_ss)
#define AKSIMD_ADD_SS_V4F32( a, b ) _mm_add_ss( a, b )

/// Multiplies the four single-precision, floating-point values
/// of a and b (see _mm_mul_ps)
#define AKSIMD_MUL_V4F32( a, b ) _mm_mul_ps( a, b )

#define AKSIMD_DIV_V4F32( a, b ) _mm_div_ps( a, b )

/// Multiplies the lower single-precision, floating-point values of
/// a and b; the upper three single-precision, floating-point values
/// are passed through from a.
/// r0 := a0 * b0; r1 := a1; r2 := a2; r3 := a3 (see _mm_add_ss)
#define AKSIMD_MUL_SS_V4F32( a, b ) _mm_mul_ss( a, b )

/// Vector multiply-add operation.
#define AKSIMD_MADD_V4F32( __a__, __b__, __c__ ) _mm_add_ps( _mm_mul_ps( (__a__), (__b__) ), (__c__) )
#define AKSIMD_MSUB_V4F32( __a__, __b__, __c__ ) _mm_sub_ps( _mm_mul_ps( (__a__), (__b__) ), (__c__) )

/// Vector multiply-add operation.
#define AKSIMD_MADD_SS_V4F32( __a__, __b__, __c__ ) _mm_add_ss( _mm_mul_ss( (__a__), (__b__) ), (__c__) )

/// Computes the minima of the four single-precision, floating-point
/// values of a and b (see _mm_min_ps)
#define AKSIMD_MIN_V4F32( a, b ) _mm_min_ps( a, b )

/// Computes the maximums of the four single-precision, floating-point
/// values of a and b (see _mm_max_ps)
#define AKSIMD_MAX_V4F32( a, b ) _mm_max_ps( a, b )

/// Computes the absolute value
#define AKSIMD_ABS_V4F32( a ) _mm_andnot_ps(_mm_set1_ps(-0.f), a)

/// Changes the sign
#define AKSIMD_NEG_V4F32( __a__ ) _mm_xor_ps(_mm_set1_ps(-0.f), __a__)

/// Vector square root aproximation (see _mm_sqrt_ps)
#define AKSIMD_SQRT_V4F32( __a__ ) _mm_sqrt_ps( (__a__) )

/// Vector reciprocal square root approximation 1/sqrt(a), or equivalently, sqrt(1/a)
#define AKSIMD_RSQRT_V4F32( __a__ ) _mm_rsqrt_ps( (__a__) )

/// Reciprocal of x (1/x)
#define AKSIMD_RECIP_V4F32(__a__) _mm_rcp_ps(__a__)

/// Binary xor for single-precision floating-point
#define AKSIMD_XOR_V4F32( a, b ) _mm_xor_ps(a,b)

/// Rounds to upper value
static AkForceInline AKSIMD_V4F32 AKSIMD_CEIL_V4F32(const AKSIMD_V4F32 & x)
{
	static const AKSIMD_V4F32 vEpsilon = { 0.49999f, 0.49999f, 0.49999f, 0.49999f };
	return _mm_cvtepi32_ps(_mm_cvtps_epi32(_mm_add_ps(x, vEpsilon)));
}

/// Faked in-place vector horizontal add - each element will represent sum of all elements
/// \akwarning
/// Don't expect this to be very efficient. 
/// \endakwarning
static AkForceInline AKSIMD_V4F32 AKSIMD_HORIZONTALADD_V4F32(AKSIMD_V4F32 vVec)
{   
	__m128 vAb = _mm_shuffle_ps(vVec, vVec, 0xB1);
	__m128 vHaddAb = _mm_add_ps(vVec, vAb);
	__m128 vHaddCd = _mm_shuffle_ps(vHaddAb, vHaddAb, 0x4E);
	__m128 vHaddAbcd = _mm_add_ps(vHaddAb, vHaddCd);
	return vHaddAbcd;
} 

static AkForceInline AKSIMD_V4F32 AKSIMD_DOTPRODUCT( AKSIMD_V4F32 & vVec, const AKSIMD_V4F32 & vfSigns )
{
	AKSIMD_V4F32 vfDotProduct = AKSIMD_MUL_V4F32( vVec, vfSigns );
	return AKSIMD_HORIZONTALADD_V4F32( vfDotProduct );
}

/// Cross-platform SIMD multiplication of 2 complex data elements with interleaved real and imaginary parts
static AkForceInline AKSIMD_V4F32 AKSIMD_COMPLEXMUL_V4F32( const AKSIMD_V4F32 vCIn1, const AKSIMD_V4F32 vCIn2 )
{
	static const AKSIMD_V4F32 vSign = { -0.f, 0.f, -0.f, 0.f }; 

	AKSIMD_V4F32 vTmp1 = AKSIMD_SHUFFLE_V4F32( vCIn1, vCIn1, AKSIMD_SHUFFLE(2,2,0,0));
	vTmp1 = AKSIMD_MUL_V4F32( vTmp1, vCIn2 );
	AKSIMD_V4F32 vTmp2 = AKSIMD_SHUFFLE_V4F32( vCIn1, vCIn1, AKSIMD_SHUFFLE(3,3,1,1));
	vTmp2 = AKSIMD_XOR_V4F32( vTmp2, vSign );
	vTmp2 = AKSIMD_MADD_V4F32( vTmp2, AKSIMD_SHUFFLE_BADC( vCIn2 ), vTmp1 );
	return vTmp2;
}

#ifdef AK_SSE3

#include <pmmintrin.h>

/// Cross-platform SIMD multiplication of 2 complex data elements with interleaved real and imaginary parts
static AKSIMD_V4F32 AKSIMD_COMPLEXMUL_SSE3( const AKSIMD_V4F32 vCIn1, const AKSIMD_V4F32 vCIn2 )
{
	AKSIMD_V4F32 vXMM0 = _mm_moveldup_ps(vCIn1);	// multiplier real  (a1,   a1,   a0,   a0) 
	vXMM0 = AKSIMD_MUL_V4F32(vXMM0, vCIn2);			// temp1            (a1d1, a1c1, a0d0, a0c0) 
	AKSIMD_V4F32 xMM1 = _mm_shuffle_ps(vCIn2, vCIn2, 0xB1);	// shuf multiplicand(c1,   d1,   c0,   d0)  
	AKSIMD_V4F32 xMM2 = _mm_movehdup_ps(vCIn1);		// multiplier imag  (b1,   b1,   b0,   b0) 
	xMM2 = AKSIMD_MUL_V4F32( xMM2, xMM1);			// temp2            (b1c1, b1d1, b0c0, b0d0) 
	AKSIMD_V4F32 vCOut = _mm_addsub_ps(vXMM0, xMM2);		// b1c1+a1d1, a1c1-b1d1, a0d0+b0d0, a0c0-b0c0 
	return vCOut;
}

#endif

#if defined _MSC_VER && ( _MSC_VER <= 1600 )
	#define AKSIMD_ASSERTFLUSHZEROMODE	AKASSERT( _MM_GET_FLUSH_ZERO_MODE(dummy) == _MM_FLUSH_ZERO_ON )
#else
	#define AKSIMD_ASSERTFLUSHZEROMODE	AKASSERT( _MM_GET_FLUSH_ZERO_MODE() == _MM_FLUSH_ZERO_ON )
#endif

//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD integer arithmetic
//@{

/// Adds the four integer values of a and b
#define AKSIMD_ADD_V4I32( a, b ) _mm_add_epi32( a, b )

#define AKSIMD_CMPLT_V4I32( a, b ) _mm_cmplt_epi32(a,b)
#define AKSIMD_CMPGT_V4I32( a, b ) _mm_cmpgt_epi32(a,b)
#define AKSIMD_OR_V4I32( a, b ) _mm_or_si128(a,b)
#define AKSIMD_OR_V4F32( a, b ) _mm_or_ps(a,b)
#define AKSIMD_XOR_V4I32( a, b ) _mm_xor_si128(a,b)
#define AKSIMD_SUB_V4I32( a, b ) _mm_sub_epi32(a,b)

/// Multiplies the low 16bits of a by b and stores it in V4I32 (no overflow)
#define AKSIMD_MULLO16_V4I32( a , b) _mm_mullo_epi16(a, b)
//@}
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD packing / unpacking
//@{

/// Selects and interleaves the lower two single-precision, floating-point
/// values from a and b (see _mm_unpacklo_ps)
#define AKSIMD_UNPACKLO_V4F32( a, b ) _mm_unpacklo_ps( a, b )

/// Selects and interleaves the upper two single-precision, floating-point
/// values from a and b (see _mm_unpackhi_ps)
#define AKSIMD_UNPACKHI_V4F32( a, b ) _mm_unpackhi_ps( a, b )

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
	__m128i data[4] = {
		_mm_set1_epi32(*(AkInt32*)addr0),
		_mm_set1_epi32(*(AkInt32*)addr1),
		_mm_set1_epi32(*(AkInt32*)addr2),
		_mm_set1_epi32(*(AkInt32*)addr3),
	};

	__m128i group[2] = {
		_mm_unpacklo_epi32(data[0], data[1]),
		_mm_unpacklo_epi32(data[2], data[3]),
	};

	__m128i shuffle = _mm_unpacklo_epi64(group[0], group[1]);

	AKSIMD_V4I32X2 ret{
		 _mm_srai_epi32(_mm_slli_epi32(shuffle, 16), 16),
		 _mm_srai_epi32(shuffle, 16)
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
	__m128i data[4] = {
		_mm_set1_epi64x(*(AkInt64*)addr0),
		_mm_set1_epi64x(*(AkInt64*)addr1),
		_mm_set1_epi64x(*(AkInt64*)addr2),
		_mm_set1_epi64x(*(AkInt64*)addr3),
	};
	
	__m128i group[2] = {
		_mm_unpacklo_epi64(data[0], data[1]),
		_mm_unpacklo_epi64(data[2], data[3]),
	};

	__m128i shuffle[2] = {
		_mm_castps_si128 (_mm_shuffle_ps(_mm_castsi128_ps(group[0]), _mm_castsi128_ps(group[1]), 0x88)),
		_mm_castps_si128 (_mm_shuffle_ps(_mm_castsi128_ps(group[0]), _mm_castsi128_ps(group[1]), 0xDD)),
	};

	AKSIMD_V4I32X4 ret{
		_mm_srai_epi32(_mm_slli_epi32(shuffle[0],16),16),
		_mm_srai_epi32(shuffle[0],16),
		_mm_srai_epi32(_mm_slli_epi32(shuffle[1],16),16),
		_mm_srai_epi32(shuffle[1],16),
	};
	return ret;
}

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD vector comparison
/// Apart from AKSIMD_SEL_GTEQ_V4F32, these implementations are limited to a few platforms. 
//@{

#define AKSIMD_CMP_CTRLMASK __m128

/// Vector "<=" operation (see _mm_cmple_ps)
#define AKSIMD_LTEQ_V4F32( __a__, __b__ ) _mm_cmple_ps( (__a__), (__b__) )

#define AKSIMD_LT_V4F32( __a__, __b__ ) _mm_cmplt_ps( (__a__), (__b__) )

/// Vector ">=" operation (see _mm_cmple_ps)
#define AKSIMD_GTEQ_V4F32( __a__, __b__ ) _mm_cmpge_ps( (__a__), (__b__) )

#define AKSIMD_GT_V4F32( __a__, __b__ ) _mm_cmpgt_ps( (__a__), (__b__) )

/// Vector "==" operation (see _mm_cmpeq_ps)
#define AKSIMD_EQ_V4F32( __a__, __b__ ) _mm_cmpeq_ps( (__a__), (__b__) )

/// Return a when control mask is 0, return b when control mask is non zero, control mask is in c and usually provided by above comparison operations
static AkForceInline AKSIMD_V4F32 AKSIMD_VSEL_V4F32( AKSIMD_V4F32 vA, AKSIMD_V4F32 vB, AKSIMD_V4F32 vMask )
{
    vB = _mm_and_ps( vB, vMask );
    vA= _mm_andnot_ps( vMask, vA );
    return _mm_or_ps( vA, vB );
}

// (cond1 >= cond2) ? b : a.
#define AKSIMD_SEL_GTEQ_V4F32( __a__, __b__, __cond1__, __cond2__ ) AKSIMD_VSEL_V4F32( __a__, __b__, AKSIMD_GTEQ_V4F32( __cond1__, __cond2__ ) )

// a >= 0 ? b : c ... Written, like, you know, the normal C++ operator syntax.
#define AKSIMD_SEL_GTEZ_V4F32( __a__, __b__, __c__ ) AKSIMD_VSEL_V4F32( (__c__), (__b__), AKSIMD_GTEQ_V4F32( __a__, _mm_set1_ps(0) ) )

#define AKSIMD_SPLAT_V4F32(var, idx) AKSIMD_SHUFFLE_V4F32(var,var, AKSIMD_SHUFFLE(idx,idx,idx,idx))

#define AKSIMD_MASK_V4F32( __a__ ) _mm_movemask_ps( __a__ )

//@}
////////////////////////////////////////////////////////////////////////

/// Loads unaligned 128-bit value (see _mm_loadu_si128)
#define AKSIMD_LOADU_V4I32( __addr__ ) _mm_loadu_si128( (__addr__) )

/// Loads aligned 128-bit value (see _mm_loadu_si128)
#define AKSIMD_LOAD_V4I32( __addr__ ) _mm_load_si128( (__addr__) )

/// Sets the four 32-bit integer values to zero (see _mm_setzero_si128)
#define AKSIMD_SETZERO_V4I32() _mm_setzero_si128()

#define AKSIMD_SET_V4I32( __scalar__ ) _mm_set1_epi32( (__scalar__) )

#define AKSIMD_SETV_V4I32( _d, _c, _b, _a ) _mm_set_epi32( (_d), (_c), (_b), (_a) )

#define AKSIMD_SETV_V2I64( _b, _a ) _mm_set_epi64x( (_b), (_a) )

/// Sets the 32b integer i at the location specified by index in a
#define AKSIMD_INSERT_V4I32( a, i, index) _mm_insert_epi32(a, i, index)

/// Sets the 64b integer i at the location specified by index in a
#define AKSIMD_INSERT_V2I64( a, i, index) _mm_insert_epi64(a, i, index)

/// Stores four 32-bit integer values. 
#define AKSIMD_STORE_V4I32( __addr__, __vec__ ) _mm_store_si128( (__m128i*)(__addr__), (__vec__) )

/// Stores four 32-bit integer values. The address
/// does not need to be 16-byte aligned (see _mm_storeu_si128).
#define AKSIMD_STOREU_V4I32( __addr__, __vec__ ) _mm_storeu_si128( (__m128i*)(__addr__), (__vec__) )

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD conversion
//@{

/// Converts the four signed 32-bit integer values of a to single-precision,
/// floating-point values (see _mm_cvtepi32_ps)
#define AKSIMD_CONVERT_V4I32_TO_V4F32( __vec__ ) _mm_cvtepi32_ps( (__vec__) )

/// Converts the four single-precision, floating-point values of a to signed
/// 32-bit integer values by rounding (see _mm_cvtps_epi32)
#define AKSIMD_ROUND_V4F32_TO_V4I32( __vec__ ) _mm_cvtps_epi32( (__vec__) )

/// Converts the four single-precision, floating-point values of a to signed
/// 32-bit integer values by truncating (see _mm_cvttps_epi32)
#define AKSIMD_TRUNCATE_V4F32_TO_V4I32( __vec__ ) _mm_cvttps_epi32( (__vec__) )

/// Computes the bitwise AND of the 128-bit value in a and the
/// 128-bit value in b (see _mm_and_si128)
#define AKSIMD_AND_V4I32( __a__, __b__ ) _mm_and_si128( (__a__), (__b__) )

/// Compares the 8 signed 16-bit integers in a and the 8 signed
/// 16-bit integers in b for greater than (see _mm_cmpgt_epi16)
#define AKSIMD_CMPGT_V8I16( __a__, __b__ ) _mm_cmpgt_epi16( (__a__), (__b__) )

/// Converts the 4 half-precision floats in the lower 64-bits of the provided
/// vector to 4 full-precision floats 
#define AKSIMD_CONVERT_V4F16_TO_V4F32_LO(__vec__) AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER( _mm_unpacklo_epi16(_mm_setzero_si128(), __vec__))

/// Converts the 4 half-precision floats in the upper 64-bits of the provided
/// vector to 4 full-precision floats 
#define AKSIMD_CONVERT_V4F16_TO_V4F32_HI(__vec__) AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER( _mm_unpackhi_epi16(_mm_setzero_si128(), __vec__))

static AkForceInline AKSIMD_V4F32 AKSIMD_CONVERT_V4F16_TO_V4F32_HELPER(AKSIMD_V4I32 vec)
{
	__m128i expMantData = _mm_and_si128(vec, _mm_set1_epi32(0x7fff0000));
	__m128i expMantShifted = _mm_srli_epi32(expMantData, 3); // shift so that the float16 exp/mant is now split along float32's bounds
	
	// magic number to get scale fp16 exp range into fp32 exp range (also renormalize any denorms)
	__m128i expMantFloat = _mm_castps_si128(_mm_mul_ps(_mm_castsi128_ps(expMantShifted), _mm_castsi128_ps(_mm_set1_epi32(0x77800000))));
	
	// if fp16 val was inf or nan, preserve the inf/nan exponent field (we can just 'or' the new inf-bits into the attempt at scaling from inf previously)
	__m128i infnanCheck = _mm_cmpgt_epi32(expMantData, _mm_set1_epi32(0x7bffffff));
	__m128i infnanExp = _mm_and_si128(infnanCheck, _mm_set1_epi32(255 << 23));
	__m128i expMantWithInfNan = _mm_or_si128(expMantFloat, infnanExp);
	
	// reincorporate the sign
	__m128i signData = _mm_and_si128(vec, _mm_set1_epi32(0x80000000));
	__m128 assembledFloat = _mm_castsi128_ps(_mm_or_si128(signData, expMantWithInfNan));
	return assembledFloat;
}

/// Converts the 4 full-precision floats vector to 4 half-precision floats 
/// occupying the lower bits and leaving the upper bits as zero
static AkForceInline AKSIMD_V4I32 AKSIMD_CONVERT_V4F32_TO_V4F16(AKSIMD_V4F32 vec)
{
	__m128i signData = _mm_and_si128(_mm_castps_si128(vec), _mm_set1_epi32(0x80000000));
	__m128i unsignedVec = _mm_and_si128(_mm_castps_si128(vec), _mm_set1_epi32(0x7fffffff));

	// do the processing for values that will be denormed in float16
	// Add 0.5 to get value within range, and rounde; then move mantissa data up
	__m128 denormedVec = _mm_add_ps(_mm_castsi128_ps(unsignedVec), _mm_set1_ps(0.5f));
	__m128i denormResult = _mm_slli_epi32(_mm_castps_si128(denormedVec), 16);

	// processing for values that will be normal in float16
	__m128i subnormMagic = _mm_set1_epi32(0xC8000FFF); // -131072 + rounding bias
	__m128i normRoundPart1 = _mm_add_epi32(unsignedVec, subnormMagic);
	__m128i mantLsb = _mm_slli_epi32(unsignedVec, 31 - 13);
	__m128i mantSignExtendLsb = _mm_srai_epi32(mantLsb, 31); // Extend Lsb so that it's -1 when set
	__m128i normRoundPart2 = _mm_sub_epi32(normRoundPart1, mantSignExtendLsb); // and subtract the sign-extended bit to finish rounding up
	__m128i normResult = _mm_slli_epi32(normRoundPart2, 3);

	// Combine the norm and subnorm paths together
	__m128i normalMinimum = _mm_set1_epi32((127 - 14) << 23); // smallest float32 that yields a normalized float16
	__m128i denormMask = _mm_cmpgt_epi32(normalMinimum, unsignedVec);

	__m128i nonNanFloat = _mm_or_si128(_mm_and_si128(denormMask, denormResult), _mm_andnot_si128(denormMask, normResult));

	// apply inf/nan check
	__m128i isNotInfNanMask = _mm_cmplt_epi32(unsignedVec, _mm_set1_epi32(0x47800000)); // test if the value will be greater than the max representable by float16
	__m128i mantissaData = _mm_and_si128(unsignedVec, _mm_set1_epi32(0x007fffff));
	__m128i isNanMask = _mm_cmpgt_epi32(unsignedVec, _mm_set1_epi32(0x7F800000)); // mark the parts of the vector where we have a mantissa (i.e. NAN) as 0xffffffff
	__m128i nantissaBit = _mm_and_si128(isNanMask, _mm_set1_epi32(0x02000000)); // set the NaN mantissa bit if mantissa suggests this is NaN
	__m128i infData = _mm_andnot_si128(mantissaData, _mm_set1_epi32(0x7c000000)); // grab the exponent data from unsigned vec with no mantissa
	__m128i infNanFloat = _mm_or_si128(infData, nantissaBit); // if we have a non-zero mantissa, add the NaN mantissa bit

	__m128i resultWithInfNan = _mm_or_si128(_mm_and_si128(isNotInfNanMask, nonNanFloat), _mm_andnot_si128(isNotInfNanMask, infNanFloat));

	// reincorporate the original sign
	__m128i signedResult = _mm_or_si128(signData, resultWithInfNan);

	// store results packed in lower 64 bits, and set upper 64 to zero
	__m128i resultEpi16Lo = _mm_shufflelo_epi16(signedResult, 0xD); // move 16b ints (x,x,x,x,d,c,b,a) down to (x,x,x,x,x,x,d,b)
	__m128i resultEpi16Hi = _mm_shufflehi_epi16(signedResult, 0xD); //  move 16b ints (h,g,f,e,x,x,x,x) down to (x,x,h,f,x,x,x,x)
	__m128 resultEpi16 = _mm_shuffle_ps(_mm_castsi128_ps(resultEpi16Lo), _mm_castsi128_ps(resultEpi16Hi), 0xE4); // combine - (x, x, h, f, x, x, d, b)
	__m128i result = _mm_castps_si128(_mm_shuffle_ps(resultEpi16, _mm_setzero_ps(), 0x8)); // reshuffle with zero - (0,0,0,0,h,f,d,b)

	return result;
}

//@}
////////////////////////////////////////////////////////////////////////

/// Interleaves the lower 4 signed or unsigned 16-bit integers in a with
/// the lower 4 signed or unsigned 16-bit integers in b (see _mm_unpacklo_epi16)
#define AKSIMD_UNPACKLO_VECTOR8I16( a, b ) _mm_unpacklo_epi16( a, b )

/// Interleaves the upper 4 signed or unsigned 16-bit integers in a with
/// the upper 4 signed or unsigned 16-bit integers in b (see _mm_unpackhi_epi16)
#define AKSIMD_UNPACKHI_VECTOR8I16( a, b ) _mm_unpackhi_epi16( a, b )

/// Packs the 8 signed 32-bit integers from a and b into signed 16-bit
/// integers and saturates (see _mm_packs_epi32)
#define AKSIMD_PACKS_V4I32( a, b ) _mm_packs_epi32( a, b )

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shifting
//@{

/// Shifts the 4 signed or unsigned 32-bit integers in a left by
/// in_shiftBy bits while shifting in zeros (see _mm_slli_epi32)
#define AKSIMD_SHIFTLEFT_V4I32( __vec__, __shiftBy__ ) \
	_mm_slli_epi32( (__vec__), (__shiftBy__) )

/// Shifts the 4 signed 32-bit integers in a right by in_shiftBy
/// bits while shifting in the sign bit (see _mm_srai_epi32)
#define AKSIMD_SHIFTRIGHTARITH_V4I32( __vec__, __shiftBy__ ) \
	_mm_srai_epi32( (__vec__), (__shiftBy__) )

//@}
////////////////////////////////////////////////////////////////////////

#if defined( AK_CPU_X86 ) /// MMX

typedef __m64	AKSIMD_V2F32;	///< Vector of 2 32-bit floats

#define AKSIMD_SETZERO_V2F32() _mm_setzero_si64();

#define AKSIMD_CMPGT_V2I32( a, b ) _mm_cmpgt_pi16(a,b)

/// Interleaves the lower 2 signed or unsigned 16-bit integers in a with
/// the lower 2 signed or unsigned 16-bit integers in b (see _mm_unpackhi_epi16)
#define AKSIMD_UNPACKLO_VECTOR4I16( a, b ) _mm_unpacklo_pi16( a, b )

/// Interleaves the upper 2 signed or unsigned 16-bit integers in a with
/// the upper 2 signed or unsigned 16-bit integers in b (see _mm_unpackhi_epi16)
#define AKSIMD_UNPACKHI_VECTOR4I16( a, b ) _mm_unpackhi_pi16( a, b )

/// Shifts the 2 signed or unsigned 32-bit integers in a left by
/// in_shiftBy bits while shifting in zeros (see _mm_slli_epi32)
#define AKSIMD_SHIFTLEFT_V2I32( __vec__, __shiftBy__ ) \
	_mm_slli_pi32( (__vec__), (__shiftBy__) )

/// Shifts the 2 signed 32-bit integers in a right by in_shiftBy
/// bits while shifting in the sign bit (see _mm_srai_epi32)
#define AKSIMD_SHIFTRIGHTARITH_V2I32( __vec__, __shiftBy__ ) \
	_mm_srai_pi32( (__vec__), (__shiftBy__) )

/// Used when ending a block of code that utilizes any MMX construct on x86 code
/// so that the x87 FPU can be used again
#define AKSIMD_MMX_EMPTY _mm_empty();

#endif


#endif //_AK_SIMD_SSE_H_
