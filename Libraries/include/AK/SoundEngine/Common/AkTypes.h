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

// AkTypes.h

/// \file 
/// Data type definitions.

#ifndef _AK_DATA_TYPES_H_
#define _AK_DATA_TYPES_H_

// Platform-specific section.
//----------------------------------------------------------------------------------------------------
#include <AK/AkPlatforms.h>


//----------------------------------------------------------------------------------------------------

#include <AK/SoundEngine/Common/AkSoundEngineExport.h>

#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#if defined(AK_CPU_X86_64) || defined(AK_CPU_ARM_64)
#define AK_POINTER_64
#endif // #if defined(AK_CPU_X86_64) || defined(AK_CPU_ARM_64)

typedef AkUInt32		AkUniqueID;			 		///< Unique 32-bit ID
typedef AkUInt32		AkStateID;			 		///< State ID
typedef AkUInt32		AkStateGroupID;		 		///< State group ID
typedef AkUInt32		AkPlayingID;		 		///< Playing ID
typedef	AkInt32			AkTimeMs;			 		///< Time in ms
typedef AkUInt16		AkPortNumber;				///< Port number
typedef AkReal32		AkPitchValue;		 		///< Pitch value
typedef AkReal32		AkVolumeValue;		 		///< Volume value( also apply to LFE )
typedef AkUInt64		AkGameObjectID;		 		///< Game object ID
typedef AkReal32		AkLPFType;			 		///< Low-pass filter type
typedef AkInt32			AkMemPoolId;		 		///< Memory pool ID
typedef AkUInt32		AkPluginID;			 		///< Source or effect plug-in ID
typedef AkUInt32		AkCodecID;			 		///< Codec plug-in ID
typedef AkUInt32		AkAuxBusID;			 		///< Auxilliary bus ID
typedef AkInt16			AkPluginParamID;	 		///< Source or effect plug-in parameter ID
typedef AkInt8			AkPriority;			 		///< Priority
typedef AkUInt16        AkDataCompID;		 		///< Data compression format ID
typedef AkUInt16        AkDataTypeID;		 		///< Data sample type ID
typedef AkUInt8			AkDataInterleaveID;	 		///< Data interleaved state ID
typedef AkUInt32        AkSwitchGroupID;	 		///< Switch group ID
typedef AkUInt32        AkSwitchStateID;	 		///< Switch ID
typedef AkUInt32        AkRtpcID;			 		///< Real time parameter control ID
typedef AkReal32        AkRtpcValue;		 		///< Real time parameter control value
typedef AkUInt32        AkBankID;			 		///< Run time bank ID
typedef AkUInt32        AkFileID;			 		///< Integer-type file identifier
typedef AkUInt32        AkDeviceID;			 		///< I/O device ID
typedef AkUInt32		AkTriggerID;		 		///< Trigger ID
typedef AkUInt32		AkArgumentValueID;			///< Argument value ID
typedef AkUInt32		AkChannelMask;				///< Channel mask (similar to WAVE_FORMAT_EXTENSIBLE). Bit values are defined in AkSpeakerConfig.h.
typedef AkUInt32		AkModulatorID;				///< Modulator ID
typedef AkUInt32		AkAcousticTextureID;		///< Acoustic Texture ID
typedef AkUInt32		AkImageSourceID;			///< Image Source ID
typedef AkUInt64		AkOutputDeviceID;			///< Audio Output device ID

// Constants.
static const AkPluginID					AK_INVALID_PLUGINID					= (AkPluginID)-1;		///< Invalid FX ID
static const AkGameObjectID				AK_INVALID_GAME_OBJECT				= (AkGameObjectID)-1;	///< Invalid game object (may also mean all game objects)
static const AkUniqueID					AK_INVALID_UNIQUE_ID				=  0;					///< Invalid unique 32-bit ID
static const AkRtpcID					AK_INVALID_RTPC_ID					=  AK_INVALID_UNIQUE_ID;///< Invalid RTPC ID
static const AkPlayingID				AK_INVALID_PLAYING_ID				=  AK_INVALID_UNIQUE_ID;///< Invalid playing ID
static const AkUInt32					AK_DEFAULT_SWITCH_STATE				=  0;					///< Switch selected if no switch has been set yet
static const AkMemPoolId				AK_INVALID_POOL_ID					= -1;					///< Invalid pool ID
static const AkMemPoolId				AK_DEFAULT_POOL_ID					= -1;					///< Default pool ID, same as AK_INVALID_POOL_ID
static const AkAuxBusID					AK_INVALID_AUX_ID					=  AK_INVALID_UNIQUE_ID;///< Invalid auxiliary bus ID (or no Aux bus ID)
static const AkFileID					AK_INVALID_FILE_ID					= (AkFileID)-1;			///< Invalid file ID
static const AkDeviceID					AK_INVALID_DEVICE_ID				= (AkDeviceID)-1;		///< Invalid streaming device ID
static const AkBankID					AK_INVALID_BANK_ID					=  AK_INVALID_UNIQUE_ID;///< Invalid bank ID
static const AkArgumentValueID			AK_FALLBACK_ARGUMENTVALUE_ID		=  0;					///< Fallback argument value ID
static const AkChannelMask				AK_INVALID_CHANNELMASK				=  0;					///< Invalid channel mask
static const AkUInt32					AK_INVALID_OUTPUT_DEVICE_ID			=  AK_INVALID_UNIQUE_ID;///< Invalid Device ID

// Priority.
static const AkPriority					AK_DEFAULT_PRIORITY					=  50;					///< Default sound / I/O priority
static const AkPriority					AK_MIN_PRIORITY						=  0;					///< Minimal priority value [0,100]
static const AkPriority					AK_MAX_PRIORITY						=  100;					///< Maximal priority value [0,100]

// Default bank I/O settings.
static const AkPriority					AK_DEFAULT_BANK_IO_PRIORITY			= AK_DEFAULT_PRIORITY;	///<  Default bank load I/O priority
static const AkReal32					AK_DEFAULT_BANK_THROUGHPUT			= 1*1024*1024/1000.f;	///<  Default bank load throughput (1 Mb/ms)

// Bank version
static const AkUInt32					AK_SOUNDBANK_VERSION =				135;					///<  Version of the soundbank reader

/// Standard function call result.
enum AKRESULT
{
    AK_NotImplemented			= 0,	///< This feature is not implemented.
    AK_Success					= 1,	///< The operation was successful.
    AK_Fail						= 2,	///< The operation failed.
    AK_PartialSuccess			= 3,	///< The operation succeeded partially.
    AK_NotCompatible			= 4,	///< Incompatible formats
    AK_AlreadyConnected			= 5,	///< The stream is already connected to another node.
    AK_InvalidFile				= 7,	///< An unexpected value causes the file to be invalid.
    AK_AudioFileHeaderTooLarge	= 8,	///< The file header is too large.
    AK_MaxReached				= 9,	///< The maximum was reached.
    AK_InvalidID				= 14,	///< The ID is invalid.
    AK_IDNotFound				= 15,	///< The ID was not found.
    AK_InvalidInstanceID		= 16,	///< The InstanceID is invalid.
    AK_NoMoreData				= 17,	///< No more data is available from the source.
	AK_InvalidStateGroup		= 20,	///< The StateGroup is not a valid channel.
	AK_ChildAlreadyHasAParent	= 21,	///< The child already has a parent.
	AK_InvalidLanguage			= 22,	///< The language is invalid (applies to the Low-Level I/O).
	AK_CannotAddItseflAsAChild	= 23,	///< It is not possible to add itself as its own child.
	AK_InvalidParameter			= 31,	///< Something is not within bounds.
	AK_ElementAlreadyInList		= 35,	///< The item could not be added because it was already in the list.
	AK_PathNotFound				= 36,	///< This path is not known.
	AK_PathNoVertices			= 37,	///< Stuff in vertices before trying to start it
	AK_PathNotRunning			= 38,	///< Only a running path can be paused.
	AK_PathNotPaused			= 39,	///< Only a paused path can be resumed.
	AK_PathNodeAlreadyInList	= 40,	///< This path is already there.
	AK_PathNodeNotInList		= 41,	///< This path is not there.
	AK_DataNeeded				= 43,	///< The consumer needs more.
	AK_NoDataNeeded				= 44,	///< The consumer does not need more.
	AK_DataReady				= 45,	///< The provider has available data.
	AK_NoDataReady				= 46,	///< The provider does not have available data.
	AK_InsufficientMemory		= 52,	///< Memory error.
	AK_Cancelled				= 53,	///< The requested action was cancelled (not an error).
	AK_UnknownBankID			= 54,	///< Trying to load a bank using an ID which is not defined.
	AK_BankReadError			= 56,	///< Error while reading a bank.
	AK_InvalidSwitchType		= 57,	///< Invalid switch type (used with the switch container)
    AK_FormatNotReady           = 63,   ///< Source format not known yet.
	AK_WrongBankVersion			= 64,	///< The bank version is not compatible with the current bank reader.
    AK_FileNotFound             = 66,   ///< File not found.
    AK_DeviceNotReady           = 67,   ///< Specified ID doesn't match a valid hardware device: either the device doesn't exist or is disabled.
	AK_BankAlreadyLoaded		= 69,	///< The bank load failed because the bank is already loaded.
	AK_RenderedFX				= 71,	///< The effect on the node is rendered.
	AK_ProcessNeeded			= 72,	///< A routine needs to be executed on some CPU.
	AK_ProcessDone				= 73,	///< The executed routine has finished its execution.
	AK_MemManagerNotInitialized	= 74,	///< The memory manager should have been initialized at this point.
	AK_StreamMgrNotInitialized	= 75,	///< The stream manager should have been initialized at this point.
	AK_SSEInstructionsNotSupported = 76,///< The machine does not support SSE instructions (required on PC).
	AK_Busy						= 77,	///< The system is busy and could not process the request.
	AK_UnsupportedChannelConfig = 78,	///< Channel configuration is not supported in the current execution context.
	AK_PluginMediaNotAvailable  = 79,	///< Plugin media is not available for effect.
	AK_MustBeVirtualized		= 80,	///< Sound was Not Allowed to play.
	AK_CommandTooLarge			= 81,	///< SDK command is too large to fit in the command queue.
	AK_RejectedByFilter			= 82,	///< A play request was rejected due to the MIDI filter parameters.
	AK_InvalidCustomPlatformName= 83,	///< Detecting incompatibility between Custom platform of banks and custom platform of connected application
	AK_DLLCannotLoad			= 84,	///< Plugin DLL could not be loaded, either because it is not found or one dependency is missing.
	AK_DLLPathNotFound			= 85,	///< Plugin DLL search path could not be found.
	AK_NoJavaVM					= 86,	///< No Java VM provided in AkInitSettings.
	AK_OpenSLError				= 87,	///< OpenSL returned an error.  Check error log for more details.
	AK_PluginNotRegistered		= 88,	///< Plugin is not registered.  Make sure to implement a AK::PluginRegistration class for it and use AK_STATIC_LINK_PLUGIN in the game binary.
	AK_DataAlignmentError		= 89,	///< A pointer to audio data was not aligned to the platform's required alignment (check AkTypes.h in the platform-specific folder)
	AK_DeviceNotCompatible		= 90,	///< Incompatible Audio device.
	AK_DuplicateUniqueID		= 91,	///< Two Wwise objects share the same ID.
	AK_InitBankNotLoaded		= 92,	///< The Init bank was not loaded yet, the sound engine isn't completely ready yet.
	AK_DeviceNotFound			= 93,	///< The specified device ID does not match with any of the output devices that the sound engine is currently using.
	AK_PlayingIDNotFound		= 94,	///< Calling a function with a playing ID that is not known.
	AK_InvalidFloatValue		= 95,	///< One parameter has a invalid float value such as NaN, INF or FLT_MAX.
	AK_FileFormatMismatch       = 96,   ///< Media file format unexpected
};

/// Game sync group type
enum AkGroupType
{
	// should stay set as Switch = 0 and State = 1
	AkGroupType_Switch	= 0, ///< Type switch
	AkGroupType_State	= 1  ///< Type state
};

/// Configured audio settings
struct AkAudioSettings
{
	AkUInt32			uNumSamplesPerFrame;		///< Number of samples per audio frame (256, 512, 1024 or 2048).
	AkUInt32			uNumSamplesPerSecond;		///< Number of samples per second.
};

enum AkAudioDeviceState
{
	AkDeviceState_Unknown = 0,         ///< The audio device state is unknown or invalid.
	AkDeviceState_Active = 1 << 0,	   ///< The audio device is active That is, the audio adapter that connects to the endpoint device is present and enabled.
	AkDeviceState_Disabled = 1 << 1,   ///< The audio device is disabled.
	AkDeviceState_NotPresent = 1 << 2, ///< The audio device is not present because the audio adapter that connects to the endpoint device has been removed from the system.
	AkDeviceState_Unplugged = 1 << 3,  ///< The audio device is unplugged.
	AkDeviceState_All = AkDeviceState_Active | AkDeviceState_Disabled | AkDeviceState_NotPresent | AkDeviceState_Unplugged, ///< Includes audio devices in all states.
};

struct AkDeviceDescription
{
	AkUInt32 idDevice;											///< Device ID for Wwise. This is the same as what is returned from AK::GetDeviceID and AK::GetDeviceIDFromName. Use it to specify the main device in AkPlatformInitSettings.idAudioDevice or in AK::SoundEngine::AddSecondaryOutput. 
	AkOSChar deviceName[AK_MAX_PATH];							///< The user-friendly name for the device.
	AkAudioDeviceState deviceStateMask = AkDeviceState_Unknown;	///< Bitmask used to filter the device based on their state.
	bool isDefaultDevice = false;								///< Identify default device. Always false when not supported.
};

/// This structure allows the game to provide audio files to fill the external sources. See \ref AK::SoundEngine::PostEvent
/// You can specify a streaming file or a file in-memory, regardless of the "Stream" option in the Wwise project.  
/// \akwarning
/// Make sure that only one of szFile, pInMemory or idFile is non-null. if both idFile and szFile are set, idFile is passed to low-level IO and szFile is used as stream name (for profiling purposes).
/// \endakwarning
struct AkExternalSourceInfo
{
	AkUInt32 iExternalSrcCookie;	///< Cookie identifying the source, given by hashing the name of the source given in the project.  See \ref AK::SoundEngine::GetIDFromString. \aknote If an event triggers the playback of more than one external source, they must be named uniquely in the project therefore have a unique cookie) in order to tell them apart when filling the AkExternalSourceInfo structures. \endaknote
	AkCodecID idCodec;				///< Codec ID for the file.  One of the audio formats defined in AkTypes.h (AKCODECID_XXX)
	AkOSChar * szFile;				///< File path for the source.  If not NULL, the source will be streaming from disk.  Set pInMemory to NULL. If idFile is set, this field is used as stream name (for profiling purposes).
	void* pInMemory;				///< Pointer to the in-memory file.  If not NULL, the source will be read from memory.  Set szFile and idFile to NULL.
	AkUInt32 uiMemorySize;			///< Size of the data pointed by pInMemory
	AkFileID idFile;				///< File ID.  If not zero, the source will be streaming from disk.  This ID can be anything.  Note that you must override the low-level IO to resolve this ID to a real file.  See \ref streamingmanager_lowlevel for more information on overriding the Low Level IO.

	/// Default constructor.
	AkExternalSourceInfo()
		: iExternalSrcCookie( 0 )
		, idCodec( 0 )
		, szFile( 0 )
		, pInMemory( 0 )
		, uiMemorySize( 0 )
		, idFile( 0 ) {}

	/// Constructor: specify source by memory.
	AkExternalSourceInfo( 
		void* in_pInMemory,				///< Pointer to the in-memory file.
		AkUInt32 in_uiMemorySize,		///< Size of data.
		AkUInt32 in_iExternalSrcCookie,	///< Cookie.
		AkCodecID in_idCodec			///< Codec ID.
		)
		: iExternalSrcCookie( in_iExternalSrcCookie )
		, idCodec( in_idCodec )
		, szFile( 0 )
		, pInMemory( in_pInMemory )
		, uiMemorySize( in_uiMemorySize )
		, idFile( 0 ) {}

	/// Constructor: specify source by streaming file name.
	AkExternalSourceInfo( 
		AkOSChar * in_pszFileName,		///< File name.
		AkUInt32 in_iExternalSrcCookie,	///< Cookie.
		AkCodecID in_idCodec			///< Codec ID.
		)
		: iExternalSrcCookie( in_iExternalSrcCookie )
		, idCodec( in_idCodec )
		, szFile( in_pszFileName )
		, pInMemory( 0 )
		, uiMemorySize( 0 )
		, idFile( 0 ) {}

	/// Constructor: specify source by streaming file ID.
	AkExternalSourceInfo( 
		AkFileID in_idFile,				///< File ID.
		AkUInt32 in_iExternalSrcCookie,	///< Cookie.
		AkCodecID in_idCodec			///< Codec ID.
		)
		: iExternalSrcCookie( in_iExternalSrcCookie )
		, idCodec( in_idCodec )
		, szFile( 0 )
		, pInMemory( 0 )
		, uiMemorySize( 0 )
		, idFile( in_idFile ) {}
};

/// Nature of the connection binding an input to a bus.
enum AkConnectionType
{
	ConnectionType_Direct = 0x0,			///< Direct (main, dry) connection.
	ConnectionType_GameDefSend = 0x1,		///< Connection by a game-defined send.
	ConnectionType_UserDefSend = 0x2,		///< Connection by a user-defined send.
	ConnectionType_ReflectionsSend = 0x3,	///< Connection by a early reflections send.
};

/// 3D vector.
struct AkVector
{
	inline AkVector operator+(const AkVector& b) const
	{
		AkVector v;

		v.X = X + b.X;
		v.Y = Y + b.Y;
		v.Z = Z + b.Z;

		return v;
	}

	inline void Zero()
	{
		X = 0; Y = 0; Z = 0;
	}

    AkReal32		X;	///< X Position
    AkReal32		Y;	///< Y Position
    AkReal32		Z;	///< Z Position
};

/// Position and orientation of game objects.
class AkTransform
{
public:
	//
	// Getters.
	//

	/// Get position vector.
	inline const AkVector & Position() const
	{
		return position;
	}

	/// Get orientation front vector.
	inline const AkVector & OrientationFront() const
	{
		return orientationFront;
	}

	/// Get orientation top vector.
	inline const AkVector & OrientationTop() const
	{
		return orientationTop;
	}

	//
	// Setters.
	//

	/// Set position and orientation. Orientation front and top should be orthogonal and normalized.
	inline void Set(
		const AkVector & in_position,			///< Position vector.
		const AkVector & in_orientationFront,	///< Orientation front
		const AkVector & in_orientationTop		///< Orientation top
		)
	{
		position = in_position;
		orientationFront = in_orientationFront;
		orientationTop = in_orientationTop;
	}

	/// Set position and orientation. Orientation front and top should be orthogonal and normalized.
	inline void Set(
		AkReal32 in_positionX,					///< Position x
		AkReal32 in_positionY,					///< Position y
		AkReal32 in_positionZ,					///< Position z
		AkReal32 in_orientFrontX,				///< Orientation front x
		AkReal32 in_orientFrontY,				///< Orientation front y
		AkReal32 in_orientFrontZ,				///< Orientation front z
		AkReal32 in_orientTopX,					///< Orientation top x
		AkReal32 in_orientTopY,					///< Orientation top y
		AkReal32 in_orientTopZ					///< Orientation top z
		)
	{
		position.X = in_positionX;
		position.Y = in_positionY;
		position.Z = in_positionZ;
		orientationFront.X = in_orientFrontX;
		orientationFront.Y = in_orientFrontY;
		orientationFront.Z = in_orientFrontZ;
		orientationTop.X = in_orientTopX;
		orientationTop.Y = in_orientTopY;
		orientationTop.Z = in_orientTopZ;
	}
	
	/// Set position.
	inline void SetPosition(
		const AkVector & in_position			///< Position vector.
		)
	{
		position = in_position;
	}

	/// Set position.
	inline void SetPosition(
		AkReal32 in_x,							///< x
		AkReal32 in_y,							///< y
		AkReal32 in_z							///< z
		)
	{
		position.X = in_x;
		position.Y = in_y;
		position.Z = in_z;
	}

	/// Set orientation. Orientation front and top should be orthogonal and normalized.
	inline void SetOrientation(
		const AkVector & in_orientationFront,	///< Orientation front
		const AkVector & in_orientationTop		///< Orientation top
		)
	{
		orientationFront = in_orientationFront;	
		orientationTop = in_orientationTop;
	}	

	/// Set orientation. Orientation front and top should be orthogonal and normalized.
	inline void SetOrientation(
		AkReal32 in_orientFrontX,				///< Orientation front x
		AkReal32 in_orientFrontY,				///< Orientation front y
		AkReal32 in_orientFrontZ,				///< Orientation front z
		AkReal32 in_orientTopX,					///< Orientation top x
		AkReal32 in_orientTopY,					///< Orientation top y
		AkReal32 in_orientTopZ					///< Orientation top z
		)
	{
		orientationFront.X = in_orientFrontX;
		orientationFront.Y = in_orientFrontY;
		orientationFront.Z = in_orientFrontZ;
		orientationTop.X = in_orientTopX;
		orientationTop.Y = in_orientTopY;
		orientationTop.Z = in_orientTopZ;
	}

private:
	AkVector		orientationFront;	///< Orientation of the listener
	AkVector		orientationTop;		///< Top orientation of the listener
	AkVector		position;			///< Position of the listener
};

/// Positioning information for a sound.
typedef AkTransform AkSoundPosition;

/// Positioning information for a listener.
typedef AkTransform AkListenerPosition;

/// Obstruction/occlusion pair for a position
struct AkObstructionOcclusionValues
{
	AkReal32 occlusion;    ///< OcclusionLevel: [0.0f..1.0f]
	AkReal32 obstruction;  ///< ObstructionLevel: [0.0f..1.0f]
};

/// Positioning information for a sound, with specified subset of its channels.
struct AkChannelEmitter
{
	AkTransform		position;		///< Emitter position.
	AkChannelMask	uInputChannels;	///< Channels to which the above position applies.
};

/// Polar coordinates.
struct AkPolarCoord
{
	AkReal32		r;				///< Norm/distance
	AkReal32		theta;			///< Azimuth
};

/// Spherical coordinates.
struct AkSphericalCoord : public AkPolarCoord
{
	AkReal32		phi;			///< Elevation
};

/// Emitter-listener pair: Positioning data pertaining to a single pair of emitter and listener.
class AkEmitterListenerPair
{
public:
	/// Constructor.
	AkEmitterListenerPair()
		: fDistance(0.f)
		, fEmitterAngle(0.f)
		, fListenerAngle(0.f)	
		, fDryMixGain(1.f)
		, fGameDefAuxMixGain(1.f)
		, fUserDefAuxMixGain(1.f)
		, fOcclusion(0.f)
		, fObstruction(0.f)
		, fSpread(0.f)
		, fFocus(0.f)
		, uEmitterChannelMask(0xFFFFFFFF)
		, m_uListenerID(0)
	{
	}
	/// Destructor.
	~AkEmitterListenerPair() {}

	/// Get distance.
	inline AkReal32 Distance() const { return fDistance; }

	/// Get the absolute angle, in radians between 0 and pi, of the emitter's orientation relative to 
	/// the line that joins the emitter and the listener.
	inline AkReal32 EmitterAngle() const { return fEmitterAngle; }

	/// Get the absolute angle, in radians between 0 and pi, of the listener's orientation relative to
	/// the line that joins the emitter and the listener
	inline AkReal32 ListenerAngle() const { return fListenerAngle; }

	/// Get the occlusion factor for this emitter-listener pair
	inline AkReal32 Occlusion() const { return fOcclusion; }

	/// Get the obstruction factor for this emitter-listener pair
	inline AkReal32 Obstruction() const { return fObstruction; }	

	/// Get the emitter-listener-pair-specific gain (due to distance and cone attenuation), linear [0,1], for a given connection type.
	inline AkReal32 GetGainForConnectionType(AkConnectionType in_eType) const
	{
		if (in_eType == ConnectionType_Direct)
			return fDryMixGain;
		else if (in_eType == ConnectionType_GameDefSend)
			return fGameDefAuxMixGain;
		else if (in_eType == ConnectionType_UserDefSend)
			return fUserDefAuxMixGain;
		else
			return 1.0f;
	}

	/// Get listener ID associated with the emitter-listener pair.
	inline AkGameObjectID ListenerID() const { return m_uListenerID; }
	
	AkTransform emitter;				/// Emitter position.
	AkReal32 fDistance;					/// Distance between emitter and listener.
	AkReal32 fEmitterAngle;				/// Angle between position vector and emitter orientation.
	AkReal32 fListenerAngle;			/// Angle between position vector and listener orientation.	
	AkReal32 fDryMixGain;				/// Emitter-listener-pair-specific gain (due to distance and cone attenuation) for direct connections.
	AkReal32 fGameDefAuxMixGain;		/// Emitter-listener-pair-specific gain (due to distance and cone attenuation) for game-defined send connections.
	AkReal32 fUserDefAuxMixGain;		/// Emitter-listener-pair-specific gain (due to distance and cone attenuation) for user-defined send connections.
	AkReal32 fOcclusion;				/// Emitter-listener-pair-specific occlusion factor
	AkReal32 fObstruction;				/// Emitter-listener-pair-specific obstruction factor
	AkReal32 fSpread;					/// Emitter-listener-pair-specific spread
	AkReal32 fFocus;					/// Emitter-listener-pair-specific focus
	AkChannelMask uEmitterChannelMask;	/// Channels of the emitter that apply to this ray.
protected:
	AkGameObjectID m_uListenerID;		/// Listener game object ID.
};

/// Listener information.
struct AkListener
{
	AkListener()
		: fScalingFactor( 1.0f )
		, bSpatialized( true )
	{}
	AkListenerPosition	position;		/// Listener position (see AK::SoundEngine::SetPosition()).
	AkReal32			fScalingFactor;	/// Listener scaling factor (see AK::SoundEngine::SetListenerScalingFactor()).
	bool				bSpatialized;	/// Whether listener is spatialized or not (see AK::SoundEngine::SetListenerSpatialization()).
};

// If you modify AkCurveInterpolation, don't forget to modify WAAPI validation schema accordingly.

/// Curve interpolation types
enum AkCurveInterpolation
{
//DONT GO BEYOND 15! (see below for details)
//Curves from 0 to LastFadeCurve NEED TO BE A MIRROR IMAGE AROUND LINEAR (eg. Log3 is the inverse of Exp3)
    AkCurveInterpolation_Log3			= 0, ///< Log3
    AkCurveInterpolation_Sine			= 1, ///< Sine
    AkCurveInterpolation_Log1			= 2, ///< Log1
    AkCurveInterpolation_InvSCurve		= 3, ///< Inversed S Curve
    AkCurveInterpolation_Linear			= 4, ///< Linear (Default)
    AkCurveInterpolation_SCurve			= 5, ///< S Curve
    AkCurveInterpolation_Exp1			= 6, ///< Exp1
    AkCurveInterpolation_SineRecip		= 7, ///< Reciprocal of sine curve
    AkCurveInterpolation_Exp3			= 8, ///< Exp3
	AkCurveInterpolation_LastFadeCurve  = 8, ///< Update this value to reflect last curve available for fades
	AkCurveInterpolation_Constant		= 9  ///< Constant ( not valid for fading values )
//DONT GO BEYOND 15! The value is stored on 5 bits,
//but we can use only 4 bits for the actual values, keeping
//the 5th bit at 0 to void problems when the value is
//expanded to 32 bits.
};
#define AKCURVEINTERPOLATION_NUM_STORAGE_BIT 5 ///< Internal storage restriction, for internal use only.

/// Auxiliary bus sends information per game object per given auxiliary bus.
struct AkAuxSendValue
{
	AkGameObjectID listenerID;	///< Game object ID of the listener associated with this send. Use AK_INVALID_GAME_OBJECT as a wildcard to set the auxiliary send to all connected listeners (see AK::SoundEngine::SetListeners).
	AkAuxBusID auxBusID;		///< Auxiliary bus ID.
	AkReal32 fControlValue;		///< Represents the attenuation or amplification factor applied to the volume of the sound going through the auxiliary bus. A value in the range [0.0f:1.0f[ will attenuate the sound, the sound being completely attenuated at 0.0f and unchanged at 1.0f. A value greater than 1.0f will amplify the sound by a factor equal to this value.
};

/// Volume ramp specified by end points "previous" and "next".
struct AkRamp
{
	AkRamp() : fPrev( 1.f ), fNext( 1.f ) {}
	AkRamp( AkReal32 in_fPrev, AkReal32 in_fNext ) : fPrev( in_fPrev ), fNext( in_fNext ) {}
	AkRamp & operator*=(const AkRamp& in_rRhs) { fPrev *= in_rRhs.fPrev; fNext *= in_rRhs.fNext; return *this; }

	AkReal32 fPrev;
	AkReal32 fNext;	
};
inline AkRamp operator*(const AkRamp& in_rLhs, const AkRamp& in_rRhs) 
{
	AkRamp result(in_rLhs);
	result *= in_rRhs;
	return result;
}

// ---------------------------------------------------------------
// Languages
// ---------------------------------------------------------------
#define AK_MAX_LANGUAGE_NAME_SIZE	(32)

// ---------------------------------------------------------------
// File Type ID Definitions
// ---------------------------------------------------------------

// These correspond to IDs specified in the conversion plug-ins' XML
// files. Audio sources persist them to "remember" their format.
// DO NOT CHANGE THEM without talking to someone in charge of persistence!

// Company ID for plugin development.
#define AKCOMPANYID_PLUGINDEV_MIN		(64)
#define AKCOMPANYID_PLUGINDEV_MAX		(255)

// Vendor ID.
#define AKCOMPANYID_AUDIOKINETIC        (0)     ///< Audiokinetic inc.
#define AKCOMPANYID_AUDIOKINETIC_EXTERNAL (1)   ///< Audiokinetic inc.
#define AKCOMPANYID_MCDSP				(256)	///< McDSP
#define AKCOMPANYID_WAVEARTS			(257)	///< WaveArts
#define AKCOMPANYID_PHONETICARTS		(258)	///< Phonetic Arts
#define AKCOMPANYID_IZOTOPE				(259)	///< iZotope
#define AKCOMPANYID_CRANKCASEAUDIO		(261)	///< Crankcase Audio
#define AKCOMPANYID_IOSONO				(262)	///< IOSONO
#define AKCOMPANYID_AUROTECHNOLOGIES	(263)	///< Auro Technologies
#define AKCOMPANYID_DOLBY				(264)	///< Dolby
#define AKCOMPANYID_TWOBIGEARS			(265)	///< Two Big Ears
#define AKCOMPANYID_OCULUS				(266)	///< Oculus
#define AKCOMPANYID_BLUERIPPLESOUND		(267)	///< Blue Ripple Sound
#define AKCOMPANYID_ENZIEN				(268)	///< Enzien Audio
#define AKCOMPANYID_KROTOS				(269)	///< Krotos (Dehumanizer)
#define AKCOMPANYID_NURULIZE			(270) 	///< Nurulize
#define AKCOMPANYID_SUPERPOWERED		(271)	///< Super Powered
#define AKCOMPANYID_GOOGLE				(272)	///< Google
//#define AKCOMPANYID_NVIDIA			(273)	///< NVIDIA		// Commented out to avoid redefinition, provider is already defining it.
//#define AKCOMPANYID_RESERVED			(274)	///< Reserved	// Commented out to avoid redefinition, provider is already defining it.
//#define AKCOMPANYID_MICROSOFT			(275)	///< Microsoft	// Commented out to avoid redefinition, provider is already defining it.
//#define AKCOMPANYID_YAMAHA			(276)	///< YAMAHA		// Commented out to avoid redefinition, provider is already defining it.
#define AKCOMPANYID_VISISONICS			(277)	///< Visisonics

// File/encoding types of Audiokinetic.
#define AKCODECID_BANK                  (0)		///< Bank encoding
#define AKCODECID_PCM                   (1)		///< PCM encoding
#define AKCODECID_ADPCM                 (2)		///< ADPCM encoding
#define AKCODECID_XMA                   (3)		///< XMA encoding
#define AKCODECID_VORBIS				(4)		///< Vorbis encoding
#define AKCODECID_WIIADPCM				(5)		///< ADPCM encoding on the Wii
#define AKCODECID_PCMEX					(7)		///< Standard PCM WAV file parser for Wwise Authoring
#define AKCODECID_EXTERNAL_SOURCE       (8)		///< External Source (unknown encoding)
#define AKCODECID_XWMA					(9)		///< xWMA encoding
#define AKCODECID_AAC					(10)	///< AAC encoding (only available on Apple devices) -- see AkAACFactory.h
#define AKCODECID_FILE_PACKAGE			(11)	///< File package files generated by the File Packager utility.
#define AKCODECID_ATRAC9				(12)	///< ATRAC-9 encoding
#define AKCODECID_VAG					(13)	///< VAG/HE-VAG encoding
#define AKCODECID_PROFILERCAPTURE		(14)	///< Profiler capture file (.prof) as written through AK::SoundEngine::StartProfilerCapture
#define AKCODECID_ANALYSISFILE			(15)	///< Analysis file
#define AKCODECID_MIDI					(16)	///< MIDI file
#define AKCODECID_OPUSNX                (17)    ///< OpusNX encoding
#define AKCODECID_CAF					(18)	///< CAF file
#define AKCODECID_AKOPUS                (19)    ///< Opus encoding

#define AKPLUGINID_METER				(129)  ///< Meter Plugin
#define AKPLUGINID_RECORDER				(132)  ///< Recorder Plugin

#define AKEXTENSIONID_SPATIALAUDIO		(800)	///< Spatial Audio
#define AKEXTENSIONID_INTERACTIVEMUSIC	(801)	///< Interactive Music
#define AKEXTENSIONID_EVENTMGRTHREAD	(900)	///< Profiling: Event Manager

//The following are internally defined
#define	AK_WAVE_FORMAT_VAG				0xFFFB
#define	AK_WAVE_FORMAT_AT9				0xFFFC
#define	AK_WAVE_FORMAT_VORBIS  			0xFFFF
#define	AK_WAVE_FORMAT_AAC				0xAAC0
#define AK_WAVE_FORMAT_OPUSNX           0x3039
#define AK_WAVE_FORMAT_OPUS             0x3040
#define WAVE_FORMAT_XMA2				0x166

//-----------------------------------------------------------------------------
// Codecs
//-----------------------------------------------------------------------------

class IAkSoftwareCodec;
class IAkFileCodec;
class IAkGrainCodec;
/// Registered file source creation function prototype.
AK_CALLBACK( IAkSoftwareCodec*, AkCreateFileSourceCallback )( void* in_pCtx );
/// Registered bank source node creation function prototype.
AK_CALLBACK( IAkSoftwareCodec*, AkCreateBankSourceCallback )( void* in_pCtx );
/// Registered FileCodec creation function prototype.
AK_CALLBACK( IAkFileCodec*, AkCreateFileCodecCallback )();
/// Registered IAkGrainCodec creation function prototype.
AK_CALLBACK( IAkGrainCodec*, AkCreateGrainCodecCallback )();

struct AkCodecDescriptor
{
	AkCreateFileSourceCallback pFileSrcCreateFunc;       // File VPL source.
	AkCreateBankSourceCallback pBankSrcCreateFunc;       // Bank VPL source.
	AkCreateFileCodecCallback pFileCodecCreateFunc;      // FileCodec utility.
	AkCreateGrainCodecCallback pGrainCodecCreateFunc;    // GrainCodec utility.
};


//-----------------------------------------------------------------------------
// Positioning
//-----------------------------------------------------------------------------

namespace AK
{
	namespace SoundEngine
	{
		// If you modify MultiPositionType, don't forget to modify WAAPI validation schema accordingly.

		/// MultiPositionType.
		/// \sa
		/// - AK::SoundEngine::SetMultiplePosition()
		/// - \ref soundengine_3dpositions_multiplepos
		enum MultiPositionType
		{
			MultiPositionType_SingleSource,		///< Used for normal sounds, not expected to pass to AK::SoundEngine::SetMultiplePosition() (if done, only the first position will be used).
			MultiPositionType_MultiSources,		///< Simulate multiple sources in one sound playing, adding volumes. For instance, all the torches on your level emitting using only one sound.
			MultiPositionType_MultiDirections	///< Simulate one sound coming from multiple directions. Useful for repositionning sounds based on wall openings or to simulate areas like forest or rivers ( in combination with spreading in the attenuation of the sounds ).
		};
	}
}

#define AK_PANNER_NUM_STORAGE_BITS 3
/// Speaker panning type: type of panning logic when object is not 3D spatialized (i.e. when Ak3DSpatializationMode is AK_SpatializationMode_None).
enum AkSpeakerPanningType
{
	AK_DirectSpeakerAssignment	= 0,	///< No panning: route to matching channels between input and output.
	AK_BalanceFadeHeight		= 1		///< Balance-Fade-Height: Traditional "box" or "car"-like panner.
};

#define AK_POSSOURCE_NUM_STORAGE_BITS 3
/// 3D position type: defines what acts as the emitter position for computing spatialization against the listener. Used when Ak3DSpatializationMode is AK_SpatializationMode_PositionOnly or AK_SpatializationMode_PositionAndOrientation.
enum Ak3DPositionType
{
	AK_3DPositionType_Emitter = 0,					///< 3D spatialization is computed directly from the emitter game object position.
	AK_3DPositionType_EmitterWithAutomation = 1,	///< 3D spatialization is computed from the emitter game object position, translated by user-defined automation.
	AK_3DPositionType_ListenerWithAutomation = 2	///< 3D spatialization is computed from the listener game object position, translated by user-defined automation.
};

/// Headphone / speakers panning rules
enum AkPanningRule
{
	AkPanningRule_Speakers		= 0,	///< Left and right positioned 60 degrees apart (by default - see AK::SoundEngine::GetSpeakerAngles()).
	AkPanningRule_Headphones 	= 1		///< Left and right positioned 180 degrees apart.
};

#define AK_SPAT_NUM_STORAGE_BITS 3
/// 3D spatialization mode.
enum Ak3DSpatializationMode
{
	AK_SpatializationMode_None = 0,						///< No spatialization
	AK_SpatializationMode_PositionOnly = 1,				///< Spatialization based on emitter position only.
	AK_SpatializationMode_PositionAndOrientation = 2	///< Spatialization based on both emitter position and emitter orientation.
};

/// Bus type bit field.
enum AkBusHierarchyFlags
{
	AkBusHierarchy_Primary		= 1 << 0,	///< Flag is set to indicate the primary bus hierarchy.
	AkBusHierarchy_Secondary	= 1 << 1,	///< Flag is set to indicate the secondary bus hierarchy.
	AkBusHierarchy_IsMaster		= 1 << 7	///< Flag is set to indicate a master bus (may be used in combination with other flags).
};

#define AK_MAX_BITS_METERING_FLAGS	(5)	// Keep in sync with AkMeteringFlags.

/// Metering flags. Used for specifying bus metering, through AK::SoundEngine::RegisterBusVolumeCallback() or AK::IAkMixerPluginContext::SetMeteringFlags().
enum AkMeteringFlags
{
	AK_NoMetering				= 0,			///< No metering.
	AK_EnableBusMeter_Peak		= 1 << 0,		///< Enable computation of peak metering.
	AK_EnableBusMeter_TruePeak	= 1 << 1,		///< Enable computation of true peak metering (most CPU and memory intensive).
	AK_EnableBusMeter_RMS		= 1 << 2,		///< Enable computation of RMS metering.
	// 1 << 3 is reserved.
	AK_EnableBusMeter_KPower	= 1 << 4,		///< Enable computation of K-weighted power metering (used as a basis for computing loudness, as defined by ITU-R BS.1770).
	AK_EnableBusMeter_3DMeter = 1 << 5
};

/// Plug-in type.
/// \sa
/// - AkPluginInfo
enum AkPluginType
{
	AkPluginTypeNone = 0,	///< Unknown/invalid plug-in type.
	AkPluginTypeCodec = 1,	///< Compressor/decompressor plug-in (allows support for custom audio file types).
	AkPluginTypeSource = 2,	///< Source plug-in: creates sound by synthesis method (no input, just output).
	AkPluginTypeEffect = 3,	///< Effect plug-in: applies processing to audio data.
	//AkPluginTypeMotionDevice = 4,	///< Motion Device plug-in: feeds movement data to devices. Deprecated by Motion refactor.
	//AkPluginTypeMotionSource = 5,	///< Motion Device source plug-in: feeds movement data to device busses. Deprecated by Motion refactor.
	AkPluginTypeMixer = 6,	///< Mixer plug-in: mix voices at the bus level.
	AkPluginTypeSink = 7,	///< Sink plug-in: implement custom sound engine end point.
	AkPluginTypeGlobalExtension = 8,	///< Global Extension plug-in: (e.g. Spatial Audio, Interactive Music)
	AkPluginTypeMask = 0xf 	///< Plug-in type mask is 4 bits.
};

////////////////////////////////////////////////////////////////////////////////
// Wwise ID system
////////////////////////////////////////////////////////////////////////////////
enum AkNodeType
{
	AkNodeType_Default,
	AkNodeType_Bus
};

struct WwiseObjectIDext
{
public:

	bool operator == ( const WwiseObjectIDext& in_rOther ) const
	{
		return in_rOther.id == id && in_rOther.bIsBus == bIsBus;
	}

	AkNodeType GetType()
	{
		return bIsBus ? AkNodeType_Bus : AkNodeType_Default;
	}

	AkUniqueID	id;
	bool		bIsBus;
};

struct WwiseObjectID : public WwiseObjectIDext
{
	WwiseObjectID()
	{
		id = AK_INVALID_UNIQUE_ID;
		bIsBus = false;
	}

	WwiseObjectID( AkUniqueID in_ID )
	{
		id = in_ID;
		bIsBus = false;
	}

	WwiseObjectID( AkUniqueID in_ID, bool in_bIsBus )
	{
		id = in_ID;
		bIsBus = in_bIsBus;
	}

	WwiseObjectID( AkUniqueID in_ID, AkNodeType in_eNodeType )
	{
		id = in_ID;
		bIsBus = in_eNodeType == AkNodeType_Bus;
	}
};

/// Public data structures for converted file format.
namespace AkFileParser
{
#pragma pack(push, 1)
	/// Analyzed envelope point.
	struct EnvelopePoint
	{
		AkUInt32 uPosition;		/// Position of this point in samples at the source rate.
		AkUInt16 uAttenuation;	/// Approximate _attenuation_ at this location relative to this source's maximum, in dB (absolute value).
	};
#pragma pack(pop)
}

#ifndef AK_OS_STRUCT_ALIGN
#define AK_OS_STRUCT_ALIGN	4				///< OS Structures need to be aligned at 4 bytes.
#endif

#ifndef AK_64B_OS_STRUCT_ALIGN
#define AK_64B_OS_STRUCT_ALIGN	8				///< OS Structures need to be aligned at 8 bytes.
#endif

#ifndef AK_ALIGN
#define AK_ALIGN(__declaration__, uAlignmentSize) __declaration__ ///< Default definition for declaration that might need to be aligned on some platforms. By default, no alignment will be done.
#endif

#if !defined(AK_ENDIANNESS_LITTLE) && !defined(AK_ENDIANNESS_BIG)
#define AK_ENDIANNESS_LITTLE
#endif

#ifndef AK_UNALIGNED
#define AK_UNALIGNED						///< Refers to the __unaligned compilation flag available on some platforms. Note that so far, on the tested platform this should always be placed before the pointer symbol *.
#endif

#if __cplusplus <= 199711L
#define AK_FINAL
#else
#define AK_FINAL		final					///< Refers to the C++11 final keyword
#endif

#ifndef AK_ASYNC_OPEN_DEFAULT
#define AK_ASYNC_OPEN_DEFAULT	(false)				///< Refers to asynchronous file opening in default low-level IO.
#endif

#ifndef AK_COMM_DEFAULT_DISCOVERY_PORT
#define AK_COMM_DEFAULT_DISCOVERY_PORT 24024	///< Default discovery port for most platforms using IP sockets for communication.
#endif

#ifndef AK_CAPTURE_TYPE_FLOAT
typedef AkInt16		AkCaptureType;			///< Default value: capture type is short.
#endif

#define AkRegister

#endif  //_AK_DATA_TYPES_H_
