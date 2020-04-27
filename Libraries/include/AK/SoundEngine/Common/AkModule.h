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

/// \file 
/// Audiokinetic's definitions and factory of overridable Memory Manager module.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>

/// \name Audiokinetic Memory Manager's implementation-specific definitions.
//@{

typedef void ( *AkMemInitForThread ) (
	);

typedef void ( *AkMemTermForThread ) (
	);

typedef void* ( *AkMemMalloc ) (
	AkMemPoolId poolId,
	size_t uSize
	);

typedef void* ( *AkMemMalign ) (
	AkMemPoolId poolId,
	size_t uSize,
	AkUInt32 uAlignment
	);

typedef void* ( *AkMemRealloc ) (
	AkMemPoolId poolId,
	void* pAddress,
	size_t uSize
	);

typedef void ( *AkMemFree ) (
	AkMemPoolId poolId,
	void* pAddress
	);

typedef void ( *AkMemFalign ) (
	AkMemPoolId poolId,
	void* pAddress
	);

typedef size_t ( *AkMemTotalReservedMemorySize ) (
	);

typedef size_t ( *AkMemSizeOfMemory ) (
	AkMemPoolId poolId,
	void* pAddress
	);

typedef void ( *AkMemDebugMalloc ) (
	AkMemPoolId poolId,
	size_t uSize,
	void* pAddress,
	char const* pszFile,
	AkUInt32 uLine
	);

typedef void ( *AkMemDebugMalign ) (
	AkMemPoolId poolId,
	size_t uSize,
	AkUInt32 uAlignment,
	void* pAddress,
	char const* pszFile,
	AkUInt32 uLine
	);

typedef void ( *AkMemDebugRealloc ) (
	AkMemPoolId poolId,
	void* pOldAddress,
	size_t uSize,
	void* pNewAddress,
	char const* pszFile,
	AkUInt32 uLine
	);

typedef void ( *AkMemDebugFree ) (
	AkMemPoolId poolId,
	void* pAddress
	);

typedef void ( *AkMemDebugFalign ) (
	AkMemPoolId poolId,
	void* pAddress
	);

typedef AKRESULT ( *AkMemDebugCheckForOverwrite ) (
	);

typedef void * ( *AkMemAllocVM ) (
	size_t size,
	size_t* extra
	);

typedef void ( *AkMemFreeVM ) (
	void* address, 
	size_t size, 
	size_t extra,
	size_t release
	);

/// Initialization settings for the default implementation of the Memory Manager. For more details, see \ref memorymanager_init.
/// \sa AK::MemoryMgr
struct AkMemSettings
{
	/// @name High-level memory allocation hooks. When not NULL, redirect allocations normally forwarded to rpmalloc.
	//@{
	AkMemInitForThread				pfInitForThread;				///< (Optional) Thread-specific allocator initialization hook.
	AkMemTermForThread				pfTermForThread;				///< (Optional) Thread-specific allocator termination hook.
	AkMemMalloc						pfMalloc;						///< (Optional) Memory allocation hook.
	AkMemMalign						pfMalign;						///< (Optional) Memory allocation hook.
	AkMemRealloc					pfRealloc;						///< (Optional) Memory allocation hook.
	AkMemFree						pfFree;							///< (Optional) Memory allocation hook.
	AkMemFalign						pfFalign;						///< (Optional) Memory allocation hook.
	AkMemTotalReservedMemorySize	pfTotalReservedMemorySize;		///< (Optional) Memory allocation statistics hook.
	AkMemSizeOfMemory				pfSizeOfMemory;					///< (Optional) Memory allocation statistics hook.
	//@}

	/// @name Configuration.
	//@{
	AkUInt64						uMemAllocationSizeLimit;		///< When non-zero, limits the total amount of virtual and device memory allocated by AK::MemoryMgr. 
	//@}

	/// @name Memory allocation debugging hooks.
	//@{
	AkMemDebugMalloc				pfDebugMalloc;					///< (Optional) Memory allocation debugging hook.
	AkMemDebugMalign				pfDebugMalign;					///< (Optional) Memory allocation debugging hook.
	AkMemDebugRealloc				pfDebugRealloc;					///< (Optional) Memory allocation debugging hook.
	AkMemDebugFree					pfDebugFree;					///< (Optional) Memory allocation debugging hook.
	AkMemDebugFalign				pfDebugFalign;					///< (Optional) Memory allocation debugging hook.
	AkMemDebugCheckForOverwrite		pfDebugCheckForOverwrite;		///< (Optional) Memory allocation debugging hook.
	//@}

	/// @name Page allocation hooks, used by rpmalloc. Default to AKPLATFORM::AllocVM et al.
	//@{
	AkMemAllocVM					pfAllocVM;						///< Virtual page allocation hook.
	AkMemFreeVM						pfFreeVM;						///< Virtual page allocation hook.
	AkMemAllocVM					pfAllocDevice;					///< Device page allocation hook.
	AkMemFreeVM						pfFreeDevice;					///< Device page allocation hook.
	//@}
};
//@}

namespace AK
{
    /// \name Audiokinetic implementation-specific modules factories.
    //@{
	namespace MemoryMgr
	{
		/// @name Initialization
		//@{

	    /// Initialize the default implementation of the Memory Manager.
	    /// \sa AK::MemoryMgr
		AK_EXTERNAPIFUNC( AKRESULT, Init ) (
			AkMemSettings * in_pSettings        ///< Memory manager initialization settings.
			);

	    /// Obtain the default initialization settings for the default implementation of the Memory Manager.
		AK_EXTERNAPIFUNC( void, GetDefaultSettings ) (
			AkMemSettings & out_pMemSettings	///< Memory manager default initialization settings.
			);

	    //@}
	}
    //@}
}
