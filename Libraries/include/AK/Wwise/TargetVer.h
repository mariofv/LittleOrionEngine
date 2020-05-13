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
/// Windows target version specification.

#pragma once

#ifndef NTDDI_VERSION				// Specifies that the minimum required platform is Windows 8.0.
#define NTDDI_VERSION 0x06020000	// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef WINVER					// Specifies that the minimum required platform is Windows 8.0.
#define WINVER 0x0602			// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT			// Specifies that the minimum required platform is Windows 8.0.
#define _WIN32_WINNT 0x0602		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS			// Specifies that the minimum required platform is Windows Me.
#define _WIN32_WINDOWS 0x0490	// This is already the latest possible version on this deprecated value.
#endif

#ifndef _WIN32_IE				// Specifies that the minimum required platform is Internet Explorer 10. (This is conservative but we don't need IE)
#define _WIN32_IE 0x0A00		// Change this to the appropriate value to target other versions of IE.
#endif
