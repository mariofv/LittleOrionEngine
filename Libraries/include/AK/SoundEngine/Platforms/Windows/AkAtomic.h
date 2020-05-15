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

// AkAtomic.h

#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile long		AkAtomic32;
typedef volatile long long	AkAtomic64;
typedef volatile void*		AkAtomicPtr;

#if defined( __arm__ ) || defined( _ARM )
#define						AK_ATOMIC_FENCE_FULL_BARRIER()												_Memory_barrier();
#else
#define						AK_ATOMIC_FENCE_FULL_BARRIER()												MemoryBarrier();
#endif

#if defined( __arm__ ) || defined( _ARM )
__forceinline	long		AkAtomicLoad32( AkAtomic32* pSrc )											{ _Memory_barrier(); return *pSrc; }
__forceinline	void		AkAtomicStore32( AkAtomic32* pDest, long value )							{ *pDest = value; _Memory_barrier(); }
#else
__forceinline	long		AkAtomicLoad32( AkAtomic32* pSrc )											{ return *pSrc; }
__forceinline	void		AkAtomicStore32( AkAtomic32* pDest, long value )							{ *pDest = value; }
#endif

__forceinline	long		AkAtomicInc32( AkAtomic32* pValue )											{ return InterlockedExchangeAdd( pValue, 1 ) + 1; }
__forceinline	long		AkAtomicDec32( AkAtomic32* pValue )											{ return InterlockedExchangeAdd( pValue, -1 ) - 1; }
__forceinline	long		AkAtomicExchange32( AkAtomic32* pDest, long value )							{ return InterlockedExchange( pDest, value ); }
__forceinline	long		AkAtomicAdd32( AkAtomic32* pDest, long value )								{ return InterlockedExchangeAdd( pDest, value ) + value; }
__forceinline	long		AkAtomicSub32( AkAtomic32* pDest, long value )								{ return InterlockedExchangeAdd( pDest, -value ) - value; }
__forceinline	int			AkAtomicCas32( AkAtomic32* pDest, long proposed, long expected )			{ return InterlockedCompareExchange( pDest, proposed, expected ) == expected ? 1 : 0; }

#ifdef _WIN64
#if defined( __arm__ ) || defined( _ARM )
__forceinline	long long	AkAtomicLoad64( AkAtomic64* pSrc )											{ _Memory_barrier(); return *pSrc; }
__forceinline	void		AkAtomicStore64( AkAtomic64* pDest, long long value )						{ *pDest = value; _Memory_barrier(); }
#else
__forceinline	long long	AkAtomicLoad64( AkAtomic64* pSrc )											{ return *pSrc; }
__forceinline	void		AkAtomicStore64( AkAtomic64* pDest, long long value )						{ *pDest = value; }
#endif
#else
__forceinline	long long	AkAtomicLoad64( AkAtomic64* pSrc )											{ return InterlockedCompareExchange64( pSrc, 0, 0 ); }
__forceinline	void		AkAtomicStore64( AkAtomic64* pDest, long long value )						{ long long tmp; do { tmp = *pDest; } while ( InterlockedCompareExchange64( pDest, value, tmp ) != tmp ); }
#endif

__forceinline	long long	AkAtomicInc64( AkAtomic64* pValue )											{ return InterlockedExchangeAdd64( pValue, 1 ) + 1; }
__forceinline	long long	AkAtomicDec64( AkAtomic64* pValue )											{ return InterlockedExchangeAdd64( pValue, - 1 ) - 1; }
__forceinline	long long	AkAtomicExchange64( AkAtomic64* pDest, long long value )					{ return InterlockedExchange64( pDest, value ); }
__forceinline	long long	AkAtomicAdd64( AkAtomic64* pDest, long long value )							{ return InterlockedExchangeAdd64( pDest, value ) + value; }
__forceinline	long long	AkAtomicSub64( AkAtomic64* pDest, long long value )							{ return InterlockedExchangeAdd64( pDest, -value ) - value; }
__forceinline	int			AkAtomicCas64( AkAtomic64* pDest, long long proposed, long long expected )	{ return InterlockedCompareExchange64( pDest, proposed, expected ) == expected ? 1 : 0; }

#if defined( __arm__ ) || defined( _ARM )
__forceinline	void*		AkAtomicLoadPtr( AkAtomicPtr* pSrc )										{ _Memory_barrier(); return ( void* )*pSrc; }
__forceinline	void		AkAtomicStorePtr( AkAtomicPtr* pDest, void* value )							{ *pDest = value; _Memory_barrier(); }
#else
__forceinline	void*		AkAtomicLoadPtr( AkAtomicPtr* pSrc )										{ return ( void* )*pSrc; }
__forceinline	void		AkAtomicStorePtr( AkAtomicPtr* pDest, void* value )							{ *pDest = value; }
#endif

#ifdef _WIN64
__forceinline	void*		AkAtomicExchangePtr( AkAtomicPtr* pDest, void* value )						{ return InterlockedExchangePointer( ( volatile PVOID* )pDest, value ); }
__forceinline	int			AkAtomicCasPtr( AkAtomicPtr* pDest, void* proposed, void* expected )		{ return InterlockedCompareExchangePointer( ( volatile PVOID* )pDest, proposed, expected ) == expected ? 1 : 0; }
#else
__forceinline	void*		AkAtomicExchangePtr(AkAtomicPtr* pDest, void* value)						{ return InterlockedExchangePointer( ( void** )pDest, value ); }
__forceinline	int			AkAtomicCasPtr(AkAtomicPtr* pDest, void* proposed, void* expected)			{ return InterlockedCompareExchangePointer( ( void** )pDest, proposed, expected ) == expected ? 1 : 0; }
#endif

#ifdef __cplusplus
}
#endif
