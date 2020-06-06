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

// AkSimdAvx2.h

/// \file 
/// AKSIMD - AVX2 implementation

#ifndef _AK_SIMD_AVX2_H_
#define _AK_SIMD_AVX2_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Platforms/SSE/AkSimd.h>

#if !defined(__AVX2__)
#error "Inclusion of AkSimdAvx2.h requires AVX2 instruction sets to be defined on platform"
#endif

#include <AK/SoundEngine/Platforms/SSE/AkSimdAvx.h>


////////////////////////////////////////////////////////////////////////
/// @name AKSIMD arithmetic
//@{

/// Cross-platform SIMD multiplication of 8 complex data elements with interleaved real and imaginary parts, 
/// and taking advantage of fused-multiply-add instructions
static AkForceInline AKSIMD_V8F32 AKSIMD_COMPLEXMUL_AVX2(const AKSIMD_V8F32 cIn1, const AKSIMD_V8F32 cIn2)
{
	__m256 real1Ext = _mm256_moveldup_ps(cIn1);				// reals extended		(a3, a3, a2, a2, a1, a1, a0, a0) 
	__m256 in2Shuf = _mm256_shuffle_ps(cIn2, cIn2, 0xB1);		// shuf multiplicand	(c3, d3, c2, d2, c1, d1, c0, d0)
	__m256 imag1Ext = _mm256_movehdup_ps(cIn1);				// multiplier imag		(b3, b3, b2, b2, b1, b1, b0, b0) 
	__m256 temp = _mm256_mul_ps(imag1Ext, in2Shuf);				// temp				(b3c3, b3d3, b2c2, b2d2, b1c1, b1d1, b0c0, b0d0)
	__m256 out = _mm256_fmaddsub_ps(real1Ext, cIn2, temp); // final				(a3d3+b3c3, a3c3-b3d3, a2d2+b2c2, a2c2-b2d2, a1d1+b1c1, a1c1-b1d1, a0d0+b0c0, a0c0-b0d0)
	return out;
}

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shuffling
//@{

/// For each 8b value in a, move it to the designated location in each 128b lane specified by the
/// corresponding control byte in b (or, if the control byte is >=16, set the dest to zero) (see _mm_shuffle_epi8)
#define AKSIMD_SHUFFLEB_V8I32(a, b) _mm256_shuffle_epi8(a, b)

/// For each 16b integer, select one of the values from a and b using the provided control mask - if the 
/// nth bit is false, the nth value from a will be selected; if true, the value from b will be selected.
/// (the mask applies to each 128b lane identically)
#define AKSIMD_BLEND_V16I16(a, b, i) _mm256_blend_epi16(a, b, i)

#define AKSIMD_INSERT_V2I128( a, m128, idx) _mm256_inserti128_si256(a, m128, idx)

/// For each 128b lane, select one of the four input 128b lanes across a and b,
/// based on the mask i. AKSIMD_SHUFFLE can still be directly used as a control
#define AKSIMD_PERMUTE_2X128_V8I32( a, b, i ) _mm256_permute2x128_si256(a, b, i)

/// Selects the lower of each of the 128b lanes in a and b to be the result ( B A ), ( D C ) -> ( C A )
#define AKSIMD_DEINTERLEAVELANES_LO_V8I32( a, b ) AKSIMD_PERMUTE_2X128_V8I32(a, b, AKSIMD_PERMUTE128(2, 0))

/// Selects the higher of each of the 128b lanes in a and b to be the result ( B A ), ( D C) -> ( D B )
#define AKSIMD_DEINTERLEAVELANES_HI_V8I32( a, b ) AKSIMD_PERMUTE_2X128_V8I32(a, b, AKSIMD_PERMUTE128(3, 1))

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD conversion
//@{

/// Converts the eight signed 16b integer values of a to signed 32-bit integer values
#define AKSIMD_CONVERT_V8I16_TO_V8I32( __vec__ ) _mm256_cvtepi16_epi32( (__vec__) )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD integer arithmetic
//@{

/// Adds the eight integer values of a and b
#define AKSIMD_ADD_V8I32( a, b ) _mm256_add_epi32( a, b )

#define AKSIMD_CMPLT_V8I32( a, b ) _mm256_cmpgt_epi32( b, a )
#define AKSIMD_CMPGT_V8I32( a, b ) _mm256_cmpgt_epi32( a, b )
#define AKSIMD_OR_V8I32( a, b ) _mm256_or_si256(a,b)
#define AKSIMD_XOR_V8I32( a, b ) _mm256_xor_si256(a,b)
#define AKSIMD_SUB_V8I32( a, b ) _mm256_sub_epi32(a,b)

/// Computes the bitwise AND of the 256-bit value in a and the
/// 256-bit value in b (see _mm_and_si128)
#define AKSIMD_AND_V8I32( __a__, __b__ ) _mm256_and_si256( (__a__), (__b__) )

/// Multiplies each 32-bit int value of a by b and returns the lower 32b of the result (no overflow or clamp)
#define AKSIMD_MULLO_V8I32( a , b) _mm256_mullo_epi32(a, b)

/// Multiplies the low 16bits of a by b and stores it in V8I32 (no overflow)
#define AKSIMD_MULLO16_V8I32( a , b) _mm256_mullo_epi16(a, b)

/// Subtracts each 16b integer of a by b
#define AKSIMD_SUB_V16I16( a, b ) _mm256_sub_epi16( a, b )

/// Compares the 16 signed 16-bit integers in a and the 16 signed
/// 16-bit integers in b for greater than (see _mm_cmpgt_epi16)
#define AKSIMD_CMPGT_V16I16( __a__, __b__ ) _mm256_cmpgt_epi16( (__a__), (__b__) )
//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD packing / unpacking
//@{

/// Interleaves the lower 4 signed or unsigned 16-bit integers in each lane of a
/// with the lower 4 signed or unsigned 16-bit integers in each lane of b
/// (see _mm_unpacklo_epi16)
#define AKSIMD_UNPACKLO_VECTOR16I16( a, b ) _mm256_unpacklo_epi16( a, b )

/// Interleaves the upper 8 signed or unsigned 16-bit integers in each lane of a
/// with the upper 8 signed or unsigned 16-bit integers in each lane of b
/// (see _mm_unpackhi_epi16)
#define AKSIMD_UNPACKHI_VECTOR16I16( a, b ) _mm256_unpackhi_epi16( a, b )

/// Packs the 8 signed 32-bit integers from a and b into 16 signed 16-bit
/// integers and saturates (see _mm_packs_epi32)
#define AKSIMD_PACKS_V8I32( a, b ) _mm256_packs_epi32( a, b )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD shifting
//@{

/// Shifts the 8 signed or unsigned 32-bit integers in a left by
/// in_shiftBy bits while shifting in zeros (see _mm_slli_epi32)
#define AKSIMD_SHIFTLEFT_V8I32( __vec__, __shiftBy__ ) \
	_mm256_slli_epi32( (__vec__), (__shiftBy__) )

/// Shifts the 8 signed 32-bit integers in a right by in_shiftBy
/// bits while shifting in the sign bit (see _mm_srai_epi32)
#define AKSIMD_SHIFTRIGHTARITH_V8I32( __vec__, __shiftBy__ ) \
	_mm256_srai_epi32( (__vec__), (__shiftBy__) )

//@}
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
/// @name AKSIMD gather
//@{

/// To use these, provide a base_ptr, and an expression that calculates an
/// array index for the provided base_ptr. The expression can be a lambda,
/// such as follows:
///		AKSIMD_V8I32 viData = AKSIMD_GATHER_EPI32(src,  [uIndex, uStep](int i)
///			{ return (uIndex + uStep * i); });
/// This tends to perform better than a native VGATHER on most CPUs

template <typename T, typename Function>
inline AKSIMD_V8I32 AKSIMD_GATHER_EPI32(const T* __restrict base_ptr, Function expr)
{
	__m256i vals = _mm256_setzero_si256();
	__m128i valsTemp[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
#define _GATHER_SIM_FETCH(_x) \
    {\
        AkInt32 val = *(AkInt32*)(base_ptr + expr(_x)); \
        valsTemp[_x/4] = _mm_insert_epi32(valsTemp[_x/4],  val, _x%4);\
    }

	_GATHER_SIM_FETCH(0);
	_GATHER_SIM_FETCH(1);
	_GATHER_SIM_FETCH(2);
	_GATHER_SIM_FETCH(3);
	_GATHER_SIM_FETCH(4);
	_GATHER_SIM_FETCH(5);
	_GATHER_SIM_FETCH(6);
	_GATHER_SIM_FETCH(7);
#undef _GATHER_SIM_FETCH
	vals = _mm256_setr_m128i(valsTemp[0], valsTemp[1]);
	return vals;
}

template <typename T, typename Function>
inline AKSIMD_V8I32 AKSIMD_GATHER_EPI64(const T* base_ptr, Function expr)
{
	__m256i vals = _mm256_setzero_si256();
	__m128i valsTemp[2] = { _mm_setzero_si128(),_mm_setzero_si128() };
#define _GATHER_SIM_FETCH(_x) \
    {\
        AkInt64 val = *(AkInt64*)(base_ptr + expr(_x)); \
        valsTemp[_x/2] = _mm_insert_epi64(valsTemp[_x/2],  val, _x%2);\
    }

	_GATHER_SIM_FETCH(0);
	_GATHER_SIM_FETCH(1);
	_GATHER_SIM_FETCH(2);
	_GATHER_SIM_FETCH(3);
#undef _GATHER_SIM_FETCH
	vals = _mm256_setr_m128i(valsTemp[0], valsTemp[1]);
	return vals;
}

template <typename T, typename Function>
inline AKSIMD_V8F32 AKSIMD_GATHER_PS(const T* base_ptr, Function expr)
{
	return _mm256_castsi256_ps(AKSIMD_GATHER_EPI32(base_ptr, expr));
}

template <typename T, typename Function>
inline AKSIMD_V8F32 AKSIMD_GATHER_PD(const T* base_ptr, Function expr)
{
	return _mm256_castsi256_pd(AKSIMD_GATHER_EPI64(base_ptr, expr));
}

//@}
////////////////////////////////////////////////////////////////////////


#endif //_AK_SIMD_AVX2_H_
