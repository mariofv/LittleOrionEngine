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

#ifndef __AK_PLATFORM_CONTEXT_WINDOWS_H__
#define __AK_PLATFORM_CONTEXT_WINDOWS_H__

#include <AK/SoundEngine/Common/IAkPlatformContext.h>

namespace AK
{
#if defined(AK_USE_UWP_API) || defined(DOXYGEN_INCLUDE)
	/// Context specific to the UWP port of Wwise SDK.
	class IAkUWPContext : public IAkPlatformContext
	{
	public:
#ifdef AK_UWP_CPP_CX
		/// Inverse lookup of AK::GetDeviceIDFromGamepad.
		/// \note This function is only available for project code using C++/CX.
		/// \return Reference to a gamepad, or nullptr if the device ID is no longer valid (such as if the gamepad was disconnected)
		virtual Windows::Gaming::Input::Gamepad^ GetGamepadFromDeviceID(AkDeviceID deviceID) = 0;
#endif
	};
#endif // !defined(AK_USE_UWP_API) || defined(DOXYGEN_INCLUDE)

#if !defined(AK_USE_UWP_API) || defined(DOXYGEN_INCLUDE)
	/// Context specific to the Win32 port of Wwise SDK.
	class IAkWin32Context : public IAkPlatformContext
	{
	};
#endif // !defined(AK_USE_UWP_API) || defined(DOXYGEN_INCLUDE)
}

#endif // __AK_PLATFORM_CONTEXT_MAC_H__
