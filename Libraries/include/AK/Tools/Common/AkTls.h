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

// AkTls.h -- thread-local storage abstraction

#pragma once

#include <AK/AkPlatforms.h>
#include <AK/SoundEngine/Common/AkSoundEngineExport.h>

#include <stdint.h>

#if defined(AK_WIN) || defined(AK_XBOX)
#include <AK/Tools/Win32/AkTls.h>

#elif defined (AK_SONY)
#include <AK/Tools/PS4/AkTls.h>

#elif defined (AK_NX)
#include <AK/Tools/NX/AkTls.h>

#else
#include <AK/Tools/POSIX/AkTls.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

	AK_EXTERNAPIFUNC( int, AkTlsAllocateSlot )(
		AkTlsSlot * out_pSlot
		);

	AK_EXTERNAPIFUNC( void, AkTlsFreeSlot )(
		AkTlsSlot in_slot
		);

	AK_EXTERNAPIFUNC( uintptr_t, AkTlsGetValue )(
		AkTlsSlot in_slot
		);

	AK_EXTERNAPIFUNC( void, AkTlsSetValue )(
		AkTlsSlot in_slot, 
		uintptr_t in_value
		);

#ifdef __cplusplus
}
#endif
