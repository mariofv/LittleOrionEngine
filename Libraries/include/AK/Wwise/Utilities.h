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
/// Wwise SDK utilities.

#ifndef _AK_WWISE_UTILITIES_H
#define _AK_WWISE_UTILITIES_H

#include <AK/SoundEngine/Common/AkTypes.h>

//////////////////////////////////////////////////////////////////////////
// Populate Table macros
//////////////////////////////////////////////////////////////////////////

/// Starts the declaration of a "populate table" which is used
/// to bind controls such as checkboxes and radio buttons to
/// properties of your plug-in.
///
/// \param theName The name of the populate table. It must be unique within the current scope.
///
/// \sa
/// - \ref wwiseplugin_dialog_guide_poptable
/// - AK_POP_ITEM()
/// - AK_END_POPULATE_TABLE()
#define AK_BEGIN_POPULATE_TABLE(theName) AK::Wwise::PopulateTableItem theName[] = {

/// Declares an association between a dialog control and a plug-in
/// property within a "populate table".
///
/// \param theID The resource ID of the control (checkbox or radio button)
/// \param theProp The name of the property, as defined in your plug-in's
///        XML definition file (refer to \ref wwiseplugin_xml_properties_tag)
///
/// \sa
/// - \ref wwiseplugin_dialog_guide_poptable
/// - \ref wwiseplugin_xml_properties_tag
/// - AK_BEGIN_POPULATE_TABLE()
/// - AK_END_POPULATE_TABLE()
#define AK_POP_ITEM(theID, theProp) {theID, theProp },

/// Ends the declaration of a "populate table".
///
/// \sa
///	- \ref wwiseplugin_dialog_guide_poptable
/// - AK_BEGIN_POPULATE_TABLE()
/// - AK_POP_ITEM()
#define AK_END_POPULATE_TABLE() AK_POP_ITEM(0, NULL) };

//////////////////////////////////////////////////////////////////////////
// Utilities
//////////////////////////////////////////////////////////////////////////

// Audiokinetic namespace
namespace AK
{
	// Audiokinetic Wwise namespace
	namespace Wwise
	{
		/// License type.
		enum LicenseType
		{
			LicenseType_Trial = 1,		///< Used for both Trial and Evaluation License handling
			LicenseType_Purchased,		///< The license was purchased
			LicenseType_Academic		///< The license is for academic
		};

		/// License status.
		enum LicenseStatus
		{
			LicenseStatus_Unlicensed,	///< No license found
			LicenseStatus_Expired,		///< A license is found, but is expired
			LicenseStatus_Valid,		///< A license is found and is valid

			LicenseStatus_Incompatible	///< The plugin was made for an older version of Wwise
		};

		/// Log message severity.
		enum Severity
		{
			Severity_Success = -1,	///< operation was executed without errors or will not produce errors
			Severity_Message,		///< not impacting the integrity of the current operation
			Severity_Warning,		///< potentially impacting the integrity of the current operation
			Severity_Error,			///< impacting the integrity of the current operation
			Severity_FatalError,	///< impacting the completion of the current operation
					
		};

		/// Interface to let the plug in give us a string of any size.
		/// The pointer to the interface should not be kept.
		class IWriteString
		{
		public:
			virtual void WriteString( LPCWSTR in_szString, 
										int in_iStringLength ) = 0;
		};

		/// Interfaces used to set and get the properties from a plug in.
		class IReadOnlyProperties 
		{
		public:
			virtual bool GetValue( LPCWSTR in_szPropertyName, 
									VARIANT& out_rValue ) const = 0;
		};

		class IReadWriteProperties : public IReadOnlyProperties
		{
		public:
			virtual bool SetValue( LPCWSTR in_szPropertyName, 
										const VARIANT& in_rValue ) = 0;
		};

		class IProgress
		{
		public: 
            /// Call this to set the name of the operation currently done.
            /// If not called the operation will have an empty name in the UI.
            /// The name should be on a single line.
            virtual void SetCurrentOperationName( LPCWSTR in_szOperationName ) = 0;

			/// Should be called at the beginning of the operation to set the min and max value 
			virtual void SetRange( DWORD in_dwMinValue, DWORD in_dwMaxValue ) = 0;

			/// Notify of the advancement of the task.
			virtual void NotifyProgress( DWORD in_dwProgress ) = 0;

			/// Check if the user has cancelled the task
			virtual bool IsCancelled() = 0;

			/// Display an error message to the user.
			/// The message should be on a single line.
			virtual void ErrorMessage( const CStringW& in_rErrorText, Severity in_eSeverity = Severity_Warning ) = 0;
		};

		/// Represents the association between a dialog control (such as
		/// a checkbox or radio button) and a plug-in property.
		/// \aknote
		/// You should not need to use this structure directly. Instead, use the 
		/// AK_BEGIN_POPULATE_TABLE(), AK_POP_ITEM(), and AK_END_POPULATE_TABLE() macros.
		/// \endaknote
		/// \sa
		/// - \ref wwiseplugin_dialog_guide_poptable
		struct PopulateTableItem
		{
			UINT uiID;				///< The dialog control resource ID
			LPCWSTR pszProp;		///< The property name
		};

		/// Base interface for all Wwise plug-ins.
		/// \akwarning
		/// The functions in this interface are not thread-safe, unless stated otherwise.
		/// \endakwarning
		/// \sa
		/// - \ref AK::Wwise::IAudioPlugin
		class IPluginBase
		{
		public:
			/// This will be called to delete the plug-in. The object
			/// is responsible for deleting itself when this method
			/// is called.
			/// \sa
			/// - \ref wwiseplugin_destroy
			virtual void Destroy() = 0;
		};

		/// Conversion error code.
		enum ConversionResult
		{
			ConversionSuccess			= 0,
			ConversionWarning			= 1,
			ConversionFailed			= 2,
		};

        /// Interface used to write data that can be converted, if needed, for the target
		/// platform.
		/// \aknote
		/// All functions perform the appropriate platform-specific byte reordering
		/// except where noted otherwise.
		/// \endaknote
		/// \akwarning
		/// The functions in this interface are not thread-safe, unless stated otherwise.
		/// \endakwarning
		/// \sa
		/// - \ref wwiseplugin_bank
		/// - AK::Wwise::IAudioPlugin::GetBankParameters()
        class IWriteData
        {
        public:
            /// Writes a block of data.
			/// \akcaution This data will always be written as-is, with no
			///            platform-specific conversion. \endakcaution
			/// \return True if all the data could be written, False otherwise
            virtual bool WriteData(
				LPCVOID in_pData,			///< A pointer to the buffer containing the data to be written
				UINT32 in_cBytes,			///< The number of bytes to write
				UINT32 & out_cWritten		///< The number of bytes actually written
			) = 0;

			[[deprecated("Not supported anymore. Please use WriteUtf16String or WriteUtf8String")]]
			virtual bool WritePascalString(
				LPCWSTR in_szString,		///< The string to be written; conversion is made internally
				UINT32 in_uiStringLength	///< The string length, in number of characters
			)
			{
				return false;
			}

			/// Writes a null-terminated utf-16 string (characters are 2 bytes wide).
			/// Handles endianness according to destination platform.
			/// \aknote
			/// "String" properties (as defined in the plugin's XML Description File - refer to \ref plugin_xml 
			/// for more details) are utf-16 encoded. While you are free to store this string in soundbanks as
			/// as an ansi string (using WritePascalString()), AK::IAkPluginParam::SetParam() will be passed
			/// an utf-16 string when you connect the authoring tool to the sound engine. Thus, WriteUtf16String() 
			/// is the preferred method for sending strings to a plug-in.
			/// \endaknote
			/// \return True if successful, False otherwise
            virtual bool WriteUtf16String(
				const wchar_t * in_szString			///< The string to be written (null-terminated).
			) = 0;

			/// Writes a null-terminated utf-8 string (multibyte characters).
			/// \return True if successful, False otherwise
            virtual bool WriteUtf8String(
				const char * in_szString		///< The string to be written (null-terminated).
			) = 0;

			/// Writes a boolean value.
			/// \return True if successful, False otherwise
            virtual bool WriteBool(
				bool in_bBool				///< Value to be written
			) = 0;

			/// Writes a byte value.
			/// \return True if successful, False otherwise
            virtual bool WriteByte(
				BYTE in_bByte				///< Value to be written
			) = 0;

			/// Writes a 16-bit integer.
			/// \return True if successful, False otherwise
            virtual bool WriteInt16(
				UINT16 in_uiInt16			///< Value to be written
			) = 0;

			/// Writes a 32-bit integer.
			/// \return True if successful, False otherwise
            virtual bool WriteInt32(
				UINT32 in_uiInt32			///< Value to be written
			) = 0;

			/// Writes a 64-bit integer.
			/// \return True if successful, False otherwise
            virtual bool WriteInt64(
				UINT64 in_uiInt64			///< Value to be written
			) = 0;

			/// Writes a 32-bit, single-precision floating point value.
			/// \return True if successful, False otherwise
            virtual bool WriteReal32(
				float in_fReal32			///< Value to be written
			) = 0;

			/// Writes a 64-bit, double-precision floating point value.
			/// \return True if successful, False otherwise
            virtual bool WriteReal64(
				double in_dblReal64			///< Value to be written
			) = 0;
        };
	}
}

#endif // _WWISE_UTILITIES_H
