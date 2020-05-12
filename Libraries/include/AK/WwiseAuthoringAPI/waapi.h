/*

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

*/
namespace ak
{
	namespace soundengine
	{
		/*! Sets multiple positions for a single game object. Setting multiple positions for a single game object is a way to simulate multiple emission sources while using the resources of only one voice. This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area. See <tt>AK::SoundEngine::SetMultiplePositions</tt>. */
		static const char* setMultiplePositions = "ak.soundengine.setMultiplePositions";
		/*! Sets the scaling factor of a game object. You can modify the attenuation computations on this game object to simulate sounds with a larger or smaller affected areas. See <tt>AK::SoundEngine::SetScalingFactor</tt>. */
		static const char* setScalingFactor = "ak.soundengine.setScalingFactor";
		/*! Seeks inside all playing objects that are referenced in Play Actions of the specified Event. See <tt>AK::SoundEngine::SeekOnEvent</tt>. */
		static const char* seekOnEvent = "ak.soundengine.seekOnEvent";
		/*! Sets the Auxiliary Busses to route the specified game object. See <tt>AK::SoundEngine::SetGameObjectAuxSendValues</tt>. */
		static const char* setGameObjectAuxSendValues = "ak.soundengine.setGameObjectAuxSendValues";
		/*! Register a game object. Registering a game object twice does nothing. Unregistering it once unregisters it no matter how many times it has been registered. See <tt>AK::SoundEngine::RegisterGameObj</tt>. */
		static const char* registerGameObj = "ak.soundengine.registerGameObj";
		/*! Unregisters a game object. Registering a game object twice does nothing. Unregistering it once unregisters it no matter how many times it has been registered. Unregistering a game object while it is in use is allowed, but the control over the parameters of this game object is lost. For example, say a sound associated with this game object is a 3D moving sound. It stops moving when the game object is unregistered, and there is no way to regain control over the game object. See <tt>AK::SoundEngine::UnregisterGameObj</tt>. */
		static const char* unregisterGameObj = "ak.soundengine.unregisterGameObj";
		/*! Display a message in the Profiler's Capture Log view. */
		static const char* postMsgMonitor = "ak.soundengine.postMsgMonitor";
		/*! Stop playing the current content associated to the specified game object ID. If no game object is specified, all sounds are stopped. See <tt>AK::SoundEngine::StopAll</tt>. */
		static const char* stopAll = "ak.soundengine.stopAll";
		/*! Sets the State of a State Group. See <tt>AK::SoundEngine::SetState</tt>. */
		static const char* setState = "ak.soundengine.setState";
		/*! Sets a listener's spatialization parameters. This lets you define listener-specific volume offsets for each audio channel. See <tt>AK::SoundEngine::SetListenerSpatialization</tt>. */
		static const char* setListenerSpatialization = "ak.soundengine.setListenerSpatialization";
		/*! Sets the default active listeners for all subsequent game objects that are registered. See <tt>AK::SoundEngine::SetDefaultListeners</tt>. */
		static const char* setDefaultListeners = "ak.soundengine.setDefaultListeners";
		/*! Executes an action on all nodes that are referenced in the specified event in a Play action. See <tt>AK::SoundEngine::ExecuteActionOnEvent</tt>. */
		static const char* executeActionOnEvent = "ak.soundengine.executeActionOnEvent";
		/*! Sets the value of a real-time parameter control. See <tt>AK::SoundEngine::SetRTPCValue</tt>. */
		static const char* setRTPCValue = "ak.soundengine.setRTPCValue";
		/*! Sets a single game object's active listeners. By default, all new game objects have no listeners active, but this behavior can be overridden with <tt>SetDefaultListeners()</tt>. Inactive listeners are not computed. See <tt>AK::SoundEngine::SetListeners</tt>. */
		static const char* setListeners = "ak.soundengine.setListeners";
		/*! Sets the position of a game object. See <tt>AK::SoundEngine::SetPosition</tt>. */
		static const char* setPosition = "ak.soundengine.setPosition";
		/*! Sets the State of a Switch Group. See <tt>AK::SoundEngine::SetSwitch</tt>. */
		static const char* setSwitch = "ak.soundengine.setSwitch";
		/*! Set a game object's obstruction and occlusion levels. This function is used to affect how an object should be heard by a specific listener. See <tt>AK::SoundEngine::SetObjectObstructionAndOcclusion</tt>. */
		static const char* setObjectObstructionAndOcclusion = "ak.soundengine.setObjectObstructionAndOcclusion";
		/*! Stops the current content, associated to the specified playing ID, from playing. See <tt>AK::SoundEngine::StopPlayingID</tt>. */
		static const char* stopPlayingID = "ak.soundengine.stopPlayingID";
		/*! Asynchronously post an Event to the sound engine (by event ID). See <tt>AK::SoundEngine::PostEvent</tt>. */
		static const char* postEvent = "ak.soundengine.postEvent";
		/*! Set the output bus volume (direct) to be used for the specified game object. See <tt>AK::SoundEngine::SetGameObjectOutputBusVolume</tt>. */
		static const char* setGameObjectOutputBusVolume = "ak.soundengine.setGameObjectOutputBusVolume";
		/*! Resets the value of a real-time parameter control to its default value, as specified in the Wwise project. See <tt>AK::SoundEngine::ResetRTPCValue</tt>. */
		static const char* resetRTPCValue = "ak.soundengine.resetRTPCValue";
		/*! Posts the specified Trigger. See <tt>AK::SoundEngine::PostTrigger</tt>. */
		static const char* postTrigger = "ak.soundengine.postTrigger";
	}
	namespace wwise
	{
		namespace debug
		{
			/*! Private use only. */
			static const char* testAssert = "ak.wwise.debug.testAssert";
			/*! Sent when an assert has failed. This is only available in Debug builds. */
			static const char* assertFailed = "ak.wwise.debug.assertFailed";
			/*! Private use only. */
			static const char* testCrash = "ak.wwise.debug.testCrash";
			/*! Enables or disables the automation mode for Wwise. This reduces the potential interruptions caused by message boxes and dialogs. For instance, enabling the automation mode silently accepts: project migration, project load log, EULA acceptance, project licence display and generic message boxes. */
			static const char* enableAutomationMode = "ak.wwise.debug.enableAutomationMode";
			/*! Enables debug assertions. Every call to enableAsserts with 'false' increments the ref count. Calling with true decrements the ref count. This is only available with Debug builds. */
			static const char* enableAsserts = "ak.wwise.debug.enableAsserts";
		}
		namespace core
		{
			namespace audioSourcePeaks
			{
				/*! Gets the min/max peak pairs, in the given region of an audio source, as a collection of binary strings (one per channel). The strings are base-64 encoded, 16-bit signed int arrays, with min and max values being interleaved. If getCrossChannelPeaks is true, only one binary string represents the peaks across all channels globally. */
				static const char* getMinMaxPeaksInRegion = "ak.wwise.core.audioSourcePeaks.getMinMaxPeaksInRegion";
				/*! Gets the min/max peak pairs in the entire trimmed region of an audio source, for each channel, as an array of binary strings (one per channel). The strings are base-64 encoded, 16-bit signed int arrays, with min and max values being interleaved. If getCrossChannelPeaks is true, there is only one binary string representing peaks across all channels globally. */
				static const char* getMinMaxPeaksInTrimmedRegion = "ak.wwise.core.audioSourcePeaks.getMinMaxPeaksInTrimmedRegion";
			}
			namespace remote
			{
				/*! Retrieves the connection status. */
				static const char* getConnectionStatus = "ak.wwise.core.remote.getConnectionStatus";
				/*! Retrieves all consoles available for connecting Wwise Authoring to a Sound Engine instance. */
				static const char* getAvailableConsoles = "ak.wwise.core.remote.getAvailableConsoles";
				/*! Disconnects the Wwise Authoring application from a connected Wwise Sound Engine running executable. */
				static const char* disconnect = "ak.wwise.core.remote.disconnect";
				/*! Connects the Wwise Authoring application to a Wwise Sound Engine running executable. The host must be running code with communication enabled. If only "host" is provided, Wwise connects to the first Sound Engine instance found. To distinguish between different instances, you can also provide the name of the application to connect to. */
				static const char* connect = "ak.wwise.core.remote.connect";
			}
			namespace log
			{
				/*! Sent when an item is added to the log. This could be used to retrieve items added to the SoundBank generation log. To retrieve the complete log, refer to \ref ak_wwise_core_log_get. */
				static const char* itemAdded = "ak.wwise.core.log.itemAdded";
				/*! Retrieves the latest log for a specific channel. Refer to \ref ak_wwise_core_log_itemadded to be notified when a item is added to the log. */
				static const char* get = "ak.wwise.core.log.get";
			}
			/*! Retrieve global Wwise information. */
			static const char* getInfo = "ak.wwise.core.getInfo";
			namespace object
			{
				/*! Sent when an object reference is changed. */
				static const char* referenceChanged = "ak.wwise.core.object.referenceChanged";
				/*! Moves an object to the given parent. Returns the moved object. */
				static const char* move = "ak.wwise.core.object.move";
				/*! Sent when an attenuation curve's link/unlink is changed. */
				static const char* attenuationCurveLinkChanged = "ak.wwise.core.object.attenuationCurveLinkChanged";
				/*! Sent when an object is added as a child to another object. */
				static const char* childAdded = "ak.wwise.core.object.childAdded";
				/*! Retrieves the list of all object types registered in Wwise's object model. This function returns the equivalent of \ref wobjects_index . */
				static const char* getTypes = "ak.wwise.core.object.getTypes";
				/*! Sent when the watched property of an object changes. */
				static const char* propertyChanged = "ak.wwise.core.object.propertyChanged";
				/*! Creates an object of type 'type', as a child of 'parent'. Refer to \ref waapi_import for more information about creating objects. Also refer to \ref ak_wwise_core_audio_import to import audio files to Wwise. */
				static const char* create = "ak.wwise.core.object.create";
				/*! Performs a query and returns specified data for each object in query result. Refer to \ref waapi_query for more information. */
				static const char* get = "ak.wwise.core.object.get";
				/*! Sent prior to an object's deletion. */
				static const char* preDeleted = "ak.wwise.core.object.preDeleted";
				/*! Sent when an object is renamed. Publishes the renamed object. */
				static const char* nameChanged = "ak.wwise.core.object.nameChanged";
				/*! Sent following an object's deletion. */
				static const char* postDeleted = "ak.wwise.core.object.postDeleted";
				/*! Sent when the object's notes are changed. */
				static const char* notesChanged = "ak.wwise.core.object.notesChanged";
				/*! Retrieves information about an object property. */
				static const char* getPropertyInfo = "ak.wwise.core.object.getPropertyInfo";
				/*! Renames an object. */
				static const char* setName = "ak.wwise.core.object.setName";
				/*! Sets the object's notes. */
				static const char* setNotes = "ak.wwise.core.object.setNotes";
				/*! Sets the specified attenuation curve for a given attenuation object. */
				static const char* setAttenuationCurve = "ak.wwise.core.object.setAttenuationCurve";
				/*! Sets a property value of an object for a specific platform. Refer to \ref wobjects_index for more information on the properties available on each object type. Refer to \ref ak_wwise_core_object_setreference to set a reference to an object. */
				static const char* setProperty = "ak.wwise.core.object.setProperty";
				/*! Copies an object to the given parent. */
				static const char* copy = "ak.wwise.core.object.copy";
				/*! Returns true if a property is enabled based on the values of the properties it depends on. */
				static const char* isPropertyEnabled = "ak.wwise.core.object.isPropertyEnabled";
				/*! Sets the randomizer values of a property of an object for a specific platform. Refer to \ref wobjects_index for more information on the properties available on each object type. */
				static const char* setRandomizer = "ak.wwise.core.object.setRandomizer";
				/*! Sets an object's reference value. Refer to \ref wobjects_index for more information on the references available on each object type. */
				static const char* setReference = "ak.wwise.core.object.setReference";
				/*! Sent when an attenuation curve is changed. */
				static const char* attenuationCurveChanged = "ak.wwise.core.object.attenuationCurveChanged";
				/*! Sent when an object is created. */
				static const char* created = "ak.wwise.core.object.created";
				/*! Sent when an object is removed from the children of another object. */
				static const char* childRemoved = "ak.wwise.core.object.childRemoved";
				/*! Retrieves the list of property and reference names for an object. */
				static const char* getPropertyNames = "ak.wwise.core.object.getPropertyNames";
				/*! Gets the specified attenuation curve for a given attenuation object. */
				static const char* getAttenuationCurve = "ak.wwise.core.object.getAttenuationCurve";
				/*! Sent when one or many curves are changed. */
				static const char* curveChanged = "ak.wwise.core.object.curveChanged";
				/*! Deletes the specified object. */
				static const char* delete_ = "ak.wwise.core.object.delete";
				/*! Retrieves the list of property and reference names for an object. */
				static const char* getPropertyAndReferenceNames = "ak.wwise.core.object.getPropertyAndReferenceNames";
			}
			namespace undo
			{
				/*! Ends the last undo group. */
				static const char* endGroup = "ak.wwise.core.undo.endGroup";
				/*! Cancels the last undo group. Please note that this does not revert the operations made since the last \ref ak_wwise_core_undo_begingroup call. */
				static const char* cancelGroup = "ak.wwise.core.undo.cancelGroup";
				/*! Begins an undo group. Make sure to call \ref ak_wwise_core_undo_endgroup exactly once for every ak.wwise.core.beginUndoGroup call you make. Calls to ak.wwise.core.undo.beginGroup can be nested. */
				static const char* beginGroup = "ak.wwise.core.undo.beginGroup";
			}
			namespace profiler
			{
				/*! Returns the current time of the specified profiler cursor, in milliseconds. */
				static const char* getCursorTime = "ak.wwise.core.profiler.getCursorTime";
				/*! Starts the profiler capture and returns the time at the beginning of the capture, in milliseconds. */
				static const char* startCapture = "ak.wwise.core.profiler.startCapture";
				/*! Retrieves all parameters affecting voice volume, highpass and lowpass for a voice path, resolved from pipeline IDs. */
				static const char* getVoiceContributions = "ak.wwise.core.profiler.getVoiceContributions";
				/*! Retrieves the voices at a specific profiler capture time. */
				static const char* getVoices = "ak.wwise.core.profiler.getVoices";
				/*! Retrieves the busses at a specific profiler capture time. */
				static const char* getBusses = "ak.wwise.core.profiler.getBusses";
				/*! Stops the profiler capture and returns the time at the end of the capture, in milliseconds. */
				static const char* stopCapture = "ak.wwise.core.profiler.stopCapture";
				/*! Retrieves active RTPCs at a specific profiler capture time. */
				static const char* getRTPCs = "ak.wwise.core.profiler.getRTPCs";
			}
			namespace project
			{
				/*! Sent when the after the project is completely closed. */
				static const char* postClosed = "ak.wwise.core.project.postClosed";
				/*! Sent when the project has been successfully loaded. */
				static const char* loaded = "ak.wwise.core.project.loaded";
				/*! Sent when the project begins closing. */
				static const char* preClosed = "ak.wwise.core.project.preClosed";
				/*! Saves the current project. */
				static const char* save = "ak.wwise.core.project.save";
				/*! Sent when the project has been saved. */
				static const char* saved = "ak.wwise.core.project.saved";
			}
			namespace transport
			{
				/*! Gets the state of the given transport object. */
				static const char* getState = "ak.wwise.core.transport.getState";
				/*! Sent when the transport's state has changed. */
				static const char* stateChanged = "ak.wwise.core.transport.stateChanged";
				/*! Creates a transport object for the given Wwise object. The return transport object can be used to play, stop, pause and resume the Wwise object via the other transport functions. */
				static const char* create = "ak.wwise.core.transport.create";
				/*! Returns the list of transport objects. */
				static const char* getList = "ak.wwise.core.transport.getList";
				/*! Destroys the given transport object. */
				static const char* destroy = "ak.wwise.core.transport.destroy";
				/*! Executes an action on the given transport object, or all transport objects if none is specified. */
				static const char* executeAction = "ak.wwise.core.transport.executeAction";
			}
			namespace soundbank
			{
				/*! Retrieves a SoundBank's inclusion list. */
				static const char* getInclusions = "ak.wwise.core.soundbank.getInclusions";
				/*! Sent when all soundbanks are generated. */
				static const char* generationDone = "ak.wwise.core.soundbank.generationDone";
				/*! Sent when a single SoundBank is generated. This could be sent multiple times during SoundBank generation, for every SoundBank generated and for every platform. To generate SoundBanks, refer to \ref ak_wwise_ui_commands_execute with one of the SoundBank generation commands. Refer to \ref globalcommandsids for the list of commands. */
				static const char* generated = "ak.wwise.core.soundbank.generated";
				/*! Modifies a SoundBank's inclusion list. The 'operation' argument determines how the 'inclusions' argument modifies the SoundBank's inclusion list; 'inclusions' may be added to / removed from / replace the SoundBank's inclusion list. */
				static const char* setInclusions = "ak.wwise.core.soundbank.setInclusions";
				/*! Generate a list of SoundBank in memory without writing them to disk, subscribe to ak.wwise.core.soundbank.generated to get SoundBank structure info and the bank data as base64. */
				static const char* generate = "ak.wwise.core.soundbank.generate";
			}
			namespace audio
			{
				/*! Creates Wwise objects and imports audio files. This function uses the same importation processor available through the Tab Delimited import in the Audio File Importer. The function returns an array of all objects created, replaced or re-used. Use the options to specify how the objects are returned. For more information, refer to \ref waapi_import. */
				static const char* import = "ak.wwise.core.audio.import";
				/*! Scripted object creation and audio file import from a tab-delimited file. */
				static const char* importTabDelimited = "ak.wwise.core.audio.importTabDelimited";
				/*! Sent at the end of an import operation. */
				static const char* imported = "ak.wwise.core.audio.imported";
			}
			namespace switchContainer
			{
				/*! Removes an assignment between a Switch Container's child and a State. */
				static const char* removeAssignment = "ak.wwise.core.switchContainer.removeAssignment";
				/*! Returns the list of assignments between a Switch Container's children and states. */
				static const char* getAssignments = "ak.wwise.core.switchContainer.getAssignments";
				/*! Sent when an assignment is removed from a Switch Container. */
				static const char* assignmentRemoved = "ak.wwise.core.switchContainer.assignmentRemoved";
				/*! Assigns a Switch Container's child to a Switch. This is the equivalent of doing a drag&drop of the child to a state in the Assigned Objects view. The child is always added at the end for each state. */
				static const char* addAssignment = "ak.wwise.core.switchContainer.addAssignment";
				/*! Sent when an assignment is added to a Switch Container. */
				static const char* assignmentAdded = "ak.wwise.core.switchContainer.assignmentAdded";
			}
			namespace plugin
			{
				/*! Retrieves the list of all object types registered in Wwise's object model. This function returns the equivalent of \ref wobjects_index . */
				static const char* getList = "ak.wwise.core.plugin.getList";
				/*! Retrieves information about an object property. */
				static const char* getProperty = "ak.wwise.core.plugin.getProperty";
				/*! Retrieves the list of property and reference names for an object. */
				static const char* getProperties = "ak.wwise.core.plugin.getProperties";
			}
		}
		namespace ui
		{
			/*! Bring Wwise main window to foreground. Refer to SetForegroundWindow and AllowSetForegroundWindow on MSDN for more information on the restrictions. Refer to ak.wwise.core.getInfo to obtain the Wwise process ID for AllowSetForegroundWindow. */
			static const char* bringToForeground = "ak.wwise.ui.bringToForeground";
			namespace commands
			{
				/*! Unregisters an array of add-on UI commands. */
				static const char* unregister = "ak.wwise.ui.commands.unregister";
				/*! Sent when a command is executed. The objects for which the command is executed are sent in the publication. */
				static const char* executed = "ak.wwise.ui.commands.executed";
				/*! Executes a command. Some commands can take a list of objects as parameters. Refer to \ref globalcommandsids for the available commands. */
				static const char* execute = "ak.wwise.ui.commands.execute";
				/*! Registers an array of add-on commands. Registered commands remain until the Wwise process is terminated. Refer to \ref defining_custom_commands for more information about registering commands. Also refer to \ref ak_wwise_ui_commands_executed. */
				static const char* register_ = "ak.wwise.ui.commands.register";
				/*! Gets the list of commands. */
				static const char* getCommands = "ak.wwise.ui.commands.getCommands";
			}
			/*! Retrieves the list of objects currently selected by the user in the active view. */
			static const char* getSelectedObjects = "ak.wwise.ui.getSelectedObjects";
			/*! Sent when the selection changes in the project. */
			static const char* selectionChanged = "ak.wwise.ui.selectionChanged";
			namespace project
			{
				/*! Closes the current project. */
				static const char* close = "ak.wwise.ui.project.close";
				/*! Opens a project, specified by path. Please refer to \ref ak_wwise_core_project_loaded for further explanations on how to be notified when the operation has completed. */
				static const char* open = "ak.wwise.ui.project.open";
			}
			/*! Captures a part of the Wwise UI relative to a view. */
			static const char* captureScreen = "ak.wwise.ui.captureScreen";
		}
		namespace cli
		{
			/*! Creates a blank new project. The project must not already exist. If the folder does not exist, it is created. */
			static const char* createNewProject = "ak.wwise.cli.createNewProject";
			/*! Loads the project and updates the contents of <project-name>.mediaid, if it exists. */
			static const char* updateMediaIdsInSingleFile = "ak.wwise.cli.updateMediaIdsInSingleFile";
			/*! Migrate and save the project. */
			static const char* migrate = "ak.wwise.cli.migrate";
			/*! Moves the project's media IDs from a single xml file <project-name>.mediaid to its work units (.wwu).  This command will force a save of all the project's work units. */
			static const char* moveMediaIdsToWorkUnits = "ak.wwise.cli.moveMediaIdsToWorkUnits";
			/*! Dump the objects model of a project as a JSON file. */
			static const char* dumpObjects = "ak.wwise.cli.dumpObjects";
			/*! SoundBank generation. SoundBank generation is performed according to the settings stored in the project. Custom user settings are ignored when SoundBank generation is launched from the command line. However, some of these settings can be overridden from the command line. */
			static const char* generateSoundbank = "ak.wwise.cli.generateSoundbank";
			/*! Imports a tab-delimited file to create and modify different object hierarchies. The project is automatically migrated (if required). It is also automatically saved following the import. */
			static const char* tabDelimitedImport = "ak.wwise.cli.tabDelimitedImport";
			/*! Moves the project's media IDs from its work units (.wwu) to a single file, <project-name>.mediaid.  This command will force a save of all the project's work units. */
			static const char* moveMediaIdsToSingleFile = "ak.wwise.cli.moveMediaIdsToSingleFile";
			/*! Starts a command-line Wwise Authoring API server, to which client applications, using the Wwise Authoring API, can connect. */
			static const char* waapiServer = "ak.wwise.cli.waapiServer";
			/*! Adds a new platform to a project. The platform must not already exist. */
			static const char* addNewPlatform = "ak.wwise.cli.addNewPlatform";
			/*! External Source conversion. While External Source conversion is also triggered by SoundBank generation, this operation can be used to process sources not contained in the Wwise Project. */
			static const char* convertExternalSource = "ak.wwise.cli.convertExternalSource";
		}
		namespace waapi
		{
			/*! Retrieves the list of topics to which a client can subscribe. */
			static const char* getTopics = "ak.wwise.waapi.getTopics";
			/*! Retrieves the list of functions. */
			static const char* getFunctions = "ak.wwise.waapi.getFunctions";
			/*! Retrieves the JSON schema of a Waapi URI. */
			static const char* getSchema = "ak.wwise.waapi.getSchema";
		}
	}
}
