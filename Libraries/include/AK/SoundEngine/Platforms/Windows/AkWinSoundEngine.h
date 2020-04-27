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

// AkWinSoundEngine.h

/// \file 
/// Main Sound Engine interface, specific WIN32.

#ifndef _AK_WIN_SOUND_ENGINE_H_
#define _AK_WIN_SOUND_ENGINE_H_

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

///< API used for audio output
///< Use with AkPlatformInitSettings to select the API used for audio output.  
///< Use AkAPI_Default, it will select the more appropriate API depending on the computer's capabilities.  Other values should be used for testing purposes.
///< \sa AK::SoundEngine::Init
typedef enum AkAudioAPIWindows
{
	AkAPI_Wasapi = 1 << 0,								///< Use Wasapi 
	AkAPI_XAudio2 = 1 << 1,								///< Use XAudio2 (this is the preferred API on Windows)
	AkAPI_DirectSound = 1 << 2,							///< Use DirectSound
	AkAPI_Default = AkAPI_Wasapi | AkAPI_XAudio2 | AkAPI_DirectSound,	///< Default value, will select the more appropriate API (XAudio2 is the default)	
} AkAudioAPI;

struct IXAudio2;

/// Platform specific initialization settings
/// \sa AK::SoundEngine::Init
/// \sa AK::SoundEngine::GetDefaultPlatformInitSettings

struct AkPlatformInitSettings
{
    // Direct sound.
    HWND			    hWnd;					///< Handle to the window associated to the audio.
												///< Each game must specify the HWND that will be passed to DirectSound initialization.
												///< The value returned by GetDefaultPlatformInitSettings is the foreground HWND at 
												///< the moment of the initialization of the sound engine and may not be the correct one for your game.
												///< It is required that each game provides the correct HWND to be used.
									

    // Threading model.
    AkThreadProperties  threadLEngine;			///< Lower engine threading properties
	AkThreadProperties  threadOutputMgr;		///< Ouput thread threading properties
	AkThreadProperties  threadBankManager;		///< Bank manager threading properties (its default priority is AK_THREAD_PRIORITY_NORMAL)
	AkThreadProperties  threadMonitor;			///< Monitor threading properties (its default priority is AK_THREAD_PRIORITY_ABOVENORMAL). This parameter is not used in Release build.

	// Voices.
	AkUInt16            uNumRefillsInVoice;		///< Number of refill buffers in voice buffer. 2 == double-buffered, defaults to 4.

	AkUInt32			uSampleRate;			///< Sampling Rate. Default is 48000 Hz. Use 24000hz for low quality. Any positive reasonable sample rate is supported. However be careful setting a custom value. Using an odd or really low sample rate may result in malfunctionning sound engine.


	AkAudioAPI			eAudioAPI;				///< Main audio API to use. Leave to AkAPI_Default for the default sink (default value).
												///< If a valid audioDeviceShareset plug-in is provided, the AkAudioAPI will be Ignored.
												///< \ref AkAudioAPI
	
	bool				bGlobalFocus;			///< Corresponding to DSBCAPS_GLOBALFOCUS. If using the AkAPI_DirectSound AkAudioAPI type, sounds will be muted if set to false when the game loses the focus.
												///< This setting is ignored when using other AkAudioAPI types.

	IXAudio2*			pXAudio2;				///< XAudio2 instance to use for the Wwise sound engine.  If NULL (default) Wwise will initialize its own instance.  Used only if the sink type is XAudio2 in AkInitSettings.outputType.

	bool				bEnableAvxSupport;		///< Enables run-time detection of AVX and AVX2 SIMD support in the engine and plug-ins. Disabling this may improve CPU performance by allowing for higher CPU clockspeeds.
};

struct IDirectSound8;
struct IXAudio2;
struct IMMDevice;
struct IUnknown;

namespace AK
{
	/// Get instance of XAudio2 created by the sound engine at initialization.
	/// \return Non-addref'd pointer to XAudio2 interface. NULL if sound engine is not initialized or XAudio2 is not used.
	/// The returned pointer can be of either XAudio 2.7, XAudio 2.8, Xaudio 2.9 depending on the Windows version the game is running on.  Use QueryInterface to identify which one and cast appropriately
	AK_EXTERNAPIFUNC( IUnknown *, GetWwiseXAudio2Interface)();

	/// Get instance of DirectSound created by the sound engine at initialization.
	/// \return Non-addref'd pointer to DirectSound interface. NULL if sound engine is not initialized or DirectSound is not used.
	AK_EXTERNAPIFUNC( IDirectSound8 *, GetDirectSoundInstance )();

	/// Finds the device ID for particular Audio Endpoint. 
	/// \note CoInitialize must have been called for the calling thread.  See Microsoft's documentation about CoInitialize for more details.
	/// \return A device ID to use with AkPlatformInitSettings.idAudioDevice
	AK_EXTERNAPIFUNC( AkUInt32, GetDeviceID ) (IMMDevice* in_pDevice);

	/// Finds an audio endpoint that matches the token in the device name or device ID and returns an ID.  
	/// This is a helper function that searches in the device ID (as returned by IMMDevice->GetId) and 
	/// in the property PKEY_Device_FriendlyName.  The token parameter is case-sensitive.  If you need to do matching on different conditions, use IMMDeviceEnumerator directly and AK::GetDeviceID.
	/// \note CoInitialize must have been called for the calling thread.  See Microsoft's documentation about CoInitialize for more details.
	/// \return The device ID as returned by IMMDevice->GetId, hashed by AK::SoundEngine::GetIDFromName()
	AK_EXTERNAPIFUNC( AkUInt32, GetDeviceIDFromName )(wchar_t* in_szToken);

	/// Get the user-friendly name for the specified device.  Call repeatedly with index starting at 0 and increasing to get all available devices, including disabled and unplugged devices, until the returned string is null and out_uDeviceID is zero.
	/// The number of indexable devices for the given uDeviceStateMask can be fetched by calling AK::SoundEngine::GetWindowsDeviceCount().
	/// You can also get the default device information by specifying index=-1.  The default device is the one with a green checkmark in the Audio Playback Device panel in Windows.
	/// The returned out_uDeviceID parameter is the Device ID to use with Wwise.  Use it to specify the main device in AkPlatformInitSettings.idAudioDevice. 
	/// \note CoInitialize must have been called for the calling thread.  See Microsoft's documentation about CoInitialize for more details.
	/// \return The name of the device at the "index" specified.  The pointer is valid until the next call to GetWindowsDeviceName.
	AK_EXTERNAPIFUNC(const wchar_t*, GetWindowsDeviceName) (
		AkInt32 index,			 ///< Index of the device in the array.  -1 to get information on the default device.
		AkUInt32 &out_uDeviceID, ///< Device ID for Wwise.  This is the same as what is returned from AK::GetDeviceID and AK::GetDeviceIDFromName.  Use it to specify the main device in AkPlatformInitSettings.idAudioDevice. 
		AkAudioDeviceState uDeviceStateMask = AkDeviceState_All ///< Optional bitmask used to filter the device based on their state.
		);

	/// Get the number of Audio Endpoints available for the specified device state mask.
	/// \note CoInitialize must have been called for the calling thread.  See Microsoft's documentation about CoInitialize for more details.
	/// \return The number of Audio Endpoints available for the specified device state mask.
	AK_EXTERNAPIFUNC( AkUInt32, GetWindowsDeviceCount ) (
		AkAudioDeviceState uDeviceStateMask = AkDeviceState_All ///< Optional bitmask used to filter the device based on their state.
		);

	/// Get the Audio Endpoint for the specified device index.  Call repeatedly with index starting at 0 and increasing to get all available devices, including disabled and unplugged devices, until the false is returned.
	/// You can also get the default device information by specifying index=-1.  The default device is the one with a green checkmark in the Audio Playback Device panel in Windows.
	/// The returned out_uDeviceID parameter is the Device ID to use with Wwise.  Use it to specify the main device in AkPlatformInitSettings.idAudioDevice. 
	/// The returned out_ppDevice is a pointer to a pointer variable to which the method writes the address of the IMMDevice. out_ppDevice is optional; if it is null, then no action is taken.
	/// If the method returns false, *out_ppDevice is null. If the method successed, *out_ppDevice will be a counted reference to the interface, and the caller is responsible for releasing the interface when it is no longer needed, by calling Release(), or encapsulating the device in a COM Smart Pointer. 
	/// \note CoInitialize must have been called for the calling thread.  See Microsoft's documentation about CoInitialize for more details.
	/// \return Whether or not a device was found at the given index.
	AK_EXTERNAPIFUNC( bool, GetWindowsDevice) (
		AkInt32 in_index,			///< Index of the device in the array.  -1 to get information on the default device.
		AkUInt32& out_uDeviceID,	///< Device ID for Wwise.  This is the same as what is returned from AK::GetDeviceID and AK::GetDeviceIDFromName.  Use it to specify the main device in AkPlatformInitSettings.idAudioDevice. 
		IMMDevice** out_ppDevice,	///< pointer to a pointer variable to which the method writes the address of the IMMDevice in question.
		AkAudioDeviceState uDeviceStateMask = AkDeviceState_All ///< Optional bitmask used to filter the device based on their state.
		);

#ifdef AK_UWP_CPP_CX

	/// Get the device ID corresponding to a Universal Windows Platform Gamepad reference. This device ID can be used to add/remove motion output for that gamepad.
	/// \note The ID returned is unique to Wwise and does not correspond to any sensible value outside of Wwise.
	/// \note This function is only available for project code using C++/CX.
	/// \return Unique device ID, or AK_INVALID_DEVICE_ID if the reference is no longer valid (such as if the gamepad was disconnected)
	AK_EXTERNAPIFUNC(AkDeviceID, GetDeviceIDFromGamepad) (Windows::Gaming::Input::Gamepad^ rGamepad);

#endif

};

#endif //_AK_WIN_SOUND_ENGINE_H_
