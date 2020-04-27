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
/// Memory Manager namespace.

#ifndef _AKMEMORYMGR_H_
#define _AKMEMORYMGR_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkSoundEngineExport.h>

//#define AK_MEMDEBUG

struct AkMemSettings;

/// Memory category IDs.
enum AkMemID
{
	AkMemID_Object,					///< Generic placeholder for allocations tied to the Wwise project.
	AkMemID_Event,					///< Events from the Wwise project.
	AkMemID_Structure,				///< Structures from the Wwise project.
	AkMemID_Media,					///< Media from the Wwise project.
	AkMemID_GameObject,				///< Game Objects and related.
	AkMemID_Processing,				///< Anything tied to instancing and processing of the DSP graph.
	AkMemID_ProcessingPlugin,		///< Plug-in allocations related to the DSP graph.
	AkMemID_Streaming,				///< Streaming Manager objects.
	AkMemID_StreamingIO,			///< Streaming Manager I/O memory.
	AkMemID_SpatialAudio,			///< Spatial audio.
	AkMemID_SpatialAudioGeometry,	///< Spatial audio geometry data.
	AkMemID_SpatialAudioPaths,		///< Spatial audio paths data.
	AkMemID_GameSim,				///< Game Simulator allocations.
	AkMemID_MonitorQueue,			///< Monitor Queue.
	AkMemID_Profiler,				///< Profiler.
	AkMemID_FilePackage,			///< File packager.
	AkMemID_SoundEngine,			///< Base sound engine allocations (managers, etc).

	AkMemID_NUM,					///< Category count.
	AkMemID_MASK = 0x7FFFFFFF,		///< Mask for category IDs.

	AkMemType_Device = 0x80000000	///< Device memory type bit.
};

namespace AK
{
	/// Memory Manager namespace.
	/// \remarks The functions in this namespace are thread-safe, unless stated otherwise.
	/// \sa
	/// - \ref memorymanager
	namespace MemoryMgr
	{
		/// Memory category statistics.
		/// \remarks These statistics are not collected in the Release configuration of
		/// the default memory manager implementation.
		/// \sa
		/// - AK::MemoryMgr::GetCategoryStats()
		/// - \ref memorymanager
		struct CategoryStats
		{
			// Current state
			AkUInt64 uUsed;			///< Used memory (in bytes)

			// Statistics
			AkUInt64 uPeakUsed;		///< Peak used memory (in bytes)
			AkUInt32 uAllocs;		///< Number of allocation calls since initialization
			AkUInt32 uFrees;		///< Number of free calls since initialization
		};

		struct GlobalStats
		{
			AkUInt64 uUsed;
			AkUInt64 uDeviceUsed;
			AkUInt64 uReserved;
			AkUInt64 uMax;
		};

		////////////////////////////////////////////////////////////////////////
		/// @name Initialization
		//@{

		/// Query whether the Memory Manager has been sucessfully initialized.
		/// \warning This function is not thread-safe. It should not be called at the same time as MemoryMgr::Init or MemoryMgr::Term.
		/// \return True if the Memory Manager is initialized, False otherwise
		/// \sa
		/// - AK::MemoryMgr::Init()
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( bool, IsInitialized )();

		/// Terminate the Memory Manager.
		/// \warning This function is not thread-safe. It is not valid to allocate memory or otherwise interact with the memory manager during or after this call.
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void, Term )();

		/// Performs whatever steps are required to initialize a thread for use with the memory manager.
		/// For example initializing thread local storage that the allocator requires to work.
		/// The default implementation of the memory manager performs thread initialization automatically and therefore this call is optional.
		/// For implementations where the cost of automatically initializing a thread for use with an allocator would be prohibitively expensive
		/// this call allows you to perform the initialization once during, for example, thread creation.
		/// \sa
		/// - AkMemInitForThread
		AK_EXTERNAPIFUNC( void, InitForThread )();

		/// Allows you to manually terminate a thread for use with the memory manager.
		/// The default implementation of the memory manager requires that all threads that interact with the memory manager call this function prior
		/// to either their termination or the termination of the memory manager. Threads not created by the sound engine itself will not have this
		/// function called for them automatically.
		/// Take care to call this function for any thread, not owned by wwise, that may have interacted with the memory manager. For example job system workers.
		/// \sa
		/// - AkMemTermForThread
		AK_EXTERNAPIFUNC( void, TermForThread )();

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Memory Allocation
		//@{

#if defined (AK_MEMDEBUG)
		/// Allocate memory: debug version.
		/// \return A pointer to the start of the allocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void *, dMalloc )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			size_t		in_uSize,				///< Number of bytes to allocate
			const char *in_pszFile,				///< Debug file name
			AkUInt32	in_uLine				///< Debug line number
			);
#endif
		/// Allocate memory.
		/// \return A pointer to the start of the allocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void *, Malloc )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			size_t		in_uSize 				///< Number of bytes to allocate
			);

#if defined (AK_MEMDEBUG)
		/// Reallocate memory: debug version.
		/// \return A pointer to the start of the reallocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void*, dRealloc )(
			AkMemPoolId	in_poolId,
			void		*in_pAlloc,
			size_t		in_uSize,
			const char	*in_pszFile,
			AkUInt32	in_uLine
			);
#endif

		/// Reallocate memory.
		/// \return A pointer to the start of the reallocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void *, Realloc )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			void *		in_pAlloc,				///< Pointer to the start of the allocated memory
			size_t		in_uSize 				///< Number of bytes to allocate
			);

		/// Free memory allocated with either AK::MemoryMgr::Malloc or AKMemoryMgr::Realloc.
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void, Free )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			void *		in_pMemAddress			///< Pointer to the start of memory allocated with Malloc or Realloc
			);

#if defined (AK_MEMDEBUG)
		/// Allocate memory with a specific alignment. Needs to be used
		/// in conjunction with AK::MemoryMgr::Falign. debug version.
		/// \return A pointer to the start of the allocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void *, dMalign )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			size_t		in_uSize,				///< Number of bytes to allocate
			AkUInt32	in_uAlignment, 			///< Alignment (in bytes)
			const char*	 in_pszFile,			///< Debug file name
			AkUInt32	in_uLine				///< Debug line number
			);
#endif

		/// Allocate memory with a specific alignment. Needs to be used
		/// in conjunction with AK::MemoryMgr::Falign.
		/// \return A pointer to the start of the allocated memory (NULL if the allocation could not be completed)
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void *, Malign )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			size_t		in_uSize, 				///< Number of bytes to allocate
			AkUInt32	in_uAlignment 			///< Alignment (in bytes)
			);

		/// Free memory allocated with AK::MemoryMgr::Malign.
		/// \sa
		/// - \ref memorymanager
		AK_EXTERNAPIFUNC( void, Falign )(
			AkMemPoolId in_poolId,				///< ID of the memory category (AkMemID)
			void *		in_pMemAddress			///< Pointer to the start of memory allocated with Malign
			);

		//@}

		////////////////////////////////////////////////////////////////////////
		/// @name Memory Profiling
		//@{

		/// Get statistics for a given memory category.
		/// \note Be aware of the potentially incoherent nature of reporting such information during concurrent modification by multiple threads.
		AK_EXTERNAPIFUNC( void, GetCategoryStats )(
			AkMemPoolId	in_poolId,				///< ID of the memory category (AkMemID)
			CategoryStats& out_poolStats		///< Returned statistics.
			);

		/// Get statistics for overall memory manager usage.
		/// \note Be aware of the potentially incoherent nature of reporting such information during concurrent modification by multiple threads.
		AK_EXTERNAPIFUNC( void, GetGlobalStats )(
			GlobalStats& out_stats				///< Returned statistics.
			);

		/// Called to start profiling memory usage for one thread (the calling thread).
		/// \note Not implementing this will result in the Soundbank tab of the Wwise Profiler to show 0 bytes for memory usage.
		AK_EXTERNAPIFUNC( void, StartProfileThreadUsage )(
			);

		/// Called to stop profiling memory usage for the current thread.
		/// \return The amount of memory allocated by this thread since StartProfileThreadUsage was called.
		/// \note Not implementing this will result in the Soundbank tab of the Wwise Profiler to show 0 bytes for memory usage.
		AK_EXTERNAPIFUNC( AkUInt64, StopProfileThreadUsage )(
			);

		//@}
	}
}

#endif // _AKMEMORYMGR_H_
