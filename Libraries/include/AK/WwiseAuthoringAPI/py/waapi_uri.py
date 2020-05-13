'''

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

  Version: <VERSION>  Build: <BUILDNUMBER>
  Copyright (c) <COPYRIGHTYEAR> Audiokinetic Inc.

'''
class WAAPI_URI:
    # Retrieve global Wwise information.
    ak_wwise_core_getinfo = u"ak.wwise.core.getInfo"
    # Bring Wwise main window to foreground. Refer to SetForegroundWindow and AllowSetForegroundWindow on MSDN for more information on the restrictions. Refer to ak.wwise.core.getInfo to obtain the Wwise process ID for AllowSetForegroundWindow.
    ak_wwise_ui_bringtoforeground = u"ak.wwise.ui.bringToForeground"
    # Asynchronously post an Event to the sound engine (by event ID). See <tt>AK::SoundEngine::PostEvent</tt>.
    ak_soundengine_postevent = u"ak.soundengine.postEvent"
    # Executes an action on all nodes that are referenced in the specified event in a Play action. See <tt>AK::SoundEngine::ExecuteActionOnEvent</tt>.
    ak_soundengine_executeactiononevent = u"ak.soundengine.executeActionOnEvent"
    # Register a game object. Registering a game object twice does nothing. Unregistering it once unregisters it no matter how many times it has been registered. See <tt>AK::SoundEngine::RegisterGameObj</tt>.
    ak_soundengine_registergameobj = u"ak.soundengine.registerGameObj"
    # Stops the current content, associated to the specified playing ID, from playing. See <tt>AK::SoundEngine::StopPlayingID</tt>.
    ak_soundengine_stopplayingid = u"ak.soundengine.stopPlayingID"
    # Stop playing the current content associated to the specified game object ID. If no game object is specified, all sounds are stopped. See <tt>AK::SoundEngine::StopAll</tt>.
    ak_soundengine_stopall = u"ak.soundengine.stopAll"
    # Display a message in the Profiler's Capture Log view.
    ak_soundengine_postmsgmonitor = u"ak.soundengine.postMsgMonitor"
    # Set a game object's obstruction and occlusion levels. This function is used to affect how an object should be heard by a specific listener. See <tt>AK::SoundEngine::SetObjectObstructionAndOcclusion</tt>.
    ak_soundengine_setobjectobstructionandocclusion = u"ak.soundengine.setObjectObstructionAndOcclusion"
    # Set the output bus volume (direct) to be used for the specified game object. See <tt>AK::SoundEngine::SetGameObjectOutputBusVolume</tt>.
    ak_soundengine_setgameobjectoutputbusvolume = u"ak.soundengine.setGameObjectOutputBusVolume"
    # Sets the Auxiliary Busses to route the specified game object. See <tt>AK::SoundEngine::SetGameObjectAuxSendValues</tt>.
    ak_soundengine_setgameobjectauxsendvalues = u"ak.soundengine.setGameObjectAuxSendValues"
    # Posts the specified Trigger. See <tt>AK::SoundEngine::PostTrigger</tt>.
    ak_soundengine_posttrigger = u"ak.soundengine.postTrigger"
    # Sets the State of a Switch Group. See <tt>AK::SoundEngine::SetSwitch</tt>.
    ak_soundengine_setswitch = u"ak.soundengine.setSwitch"
    # Sets the State of a State Group. See <tt>AK::SoundEngine::SetState</tt>.
    ak_soundengine_setstate = u"ak.soundengine.setState"
    # Resets the value of a real-time parameter control to its default value, as specified in the Wwise project. See <tt>AK::SoundEngine::ResetRTPCValue</tt>.
    ak_soundengine_resetrtpcvalue = u"ak.soundengine.resetRTPCValue"
    # Sets the value of a real-time parameter control. See <tt>AK::SoundEngine::SetRTPCValue</tt>.
    ak_soundengine_setrtpcvalue = u"ak.soundengine.setRTPCValue"
    # Sets a listener's spatialization parameters. This lets you define listener-specific volume offsets for each audio channel. See <tt>AK::SoundEngine::SetListenerSpatialization</tt>.
    ak_soundengine_setlistenerspatialization = u"ak.soundengine.setListenerSpatialization"
    # Sets multiple positions for a single game object. Setting multiple positions for a single game object is a way to simulate multiple emission sources while using the resources of only one voice. This can be used to simulate wall openings, area sounds, or multiple objects emitting the same sound in the same area. See <tt>AK::SoundEngine::SetMultiplePositions</tt>.
    ak_soundengine_setmultiplepositions = u"ak.soundengine.setMultiplePositions"
    # Sets the position of a game object. See <tt>AK::SoundEngine::SetPosition</tt>.
    ak_soundengine_setposition = u"ak.soundengine.setPosition"
    # Sets the scaling factor of a game object. You can modify the attenuation computations on this game object to simulate sounds with a larger or smaller affected areas. See <tt>AK::SoundEngine::SetScalingFactor</tt>.
    ak_soundengine_setscalingfactor = u"ak.soundengine.setScalingFactor"
    # Sets the default active listeners for all subsequent game objects that are registered. See <tt>AK::SoundEngine::SetDefaultListeners</tt>.
    ak_soundengine_setdefaultlisteners = u"ak.soundengine.setDefaultListeners"
    # Sets a single game object's active listeners. By default, all new game objects have no listeners active, but this behavior can be overridden with <tt>SetDefaultListeners()</tt>. Inactive listeners are not computed. See <tt>AK::SoundEngine::SetListeners</tt>.
    ak_soundengine_setlisteners = u"ak.soundengine.setListeners"
    # Seeks inside all playing objects that are referenced in Play Actions of the specified Event. See <tt>AK::SoundEngine::SeekOnEvent</tt>.
    ak_soundengine_seekonevent = u"ak.soundengine.seekOnEvent"
    # Unregisters a game object. Registering a game object twice does nothing. Unregistering it once unregisters it no matter how many times it has been registered. Unregistering a game object while it is in use is allowed, but the control over the parameters of this game object is lost. For example, say a sound associated with this game object is a 3D moving sound. It stops moving when the game object is unregistered, and there is no way to regain control over the game object. See <tt>AK::SoundEngine::UnregisterGameObj</tt>.
    ak_soundengine_unregistergameobj = u"ak.soundengine.unregisterGameObj"
    # Retrieves the list of topics to which a client can subscribe.
    ak_wwise_waapi_gettopics = u"ak.wwise.waapi.getTopics"
    # Retrieves the list of functions.
    ak_wwise_waapi_getfunctions = u"ak.wwise.waapi.getFunctions"
    # Retrieves the JSON schema of a Waapi URI.
    ak_wwise_waapi_getschema = u"ak.wwise.waapi.getSchema"
    # Opens a project, specified by path. Please refer to \ref ak_wwise_core_project_loaded for further explanations on how to be notified when the operation has completed.
    ak_wwise_ui_project_open = u"ak.wwise.ui.project.open"
    # Closes the current project.
    ak_wwise_ui_project_close = u"ak.wwise.ui.project.close"
    # Saves the current project.
    ak_wwise_core_project_save = u"ak.wwise.core.project.save"
    # Renames an object.
    ak_wwise_core_object_setname = u"ak.wwise.core.object.setName"
    # Sets an object's reference value. Refer to \ref wobjects_index for more information on the references available on each object type.
    ak_wwise_core_object_setreference = u"ak.wwise.core.object.setReference"
    # Sets a property value of an object for a specific platform. Refer to \ref wobjects_index for more information on the properties available on each object type. Refer to \ref ak_wwise_core_object_setreference to set a reference to an object.
    ak_wwise_core_object_setproperty = u"ak.wwise.core.object.setProperty"
    # Sets the randomizer values of a property of an object for a specific platform. Refer to \ref wobjects_index for more information on the properties available on each object type.
    ak_wwise_core_object_setrandomizer = u"ak.wwise.core.object.setRandomizer"
    # Sets the object's notes.
    ak_wwise_core_object_setnotes = u"ak.wwise.core.object.setNotes"
    # Executes a command. Some commands can take a list of objects as parameters. Refer to \ref globalcommandsids for the available commands.
    ak_wwise_ui_commands_execute = u"ak.wwise.ui.commands.execute"
    # Gets the list of commands.
    ak_wwise_ui_commands_getcommands = u"ak.wwise.ui.commands.getCommands"
    # Retrieves the list of objects currently selected by the user in the active view.
    ak_wwise_ui_getselectedobjects = u"ak.wwise.ui.getSelectedObjects"
    # Gets the specified attenuation curve for a given attenuation object.
    ak_wwise_core_object_getattenuationcurve = u"ak.wwise.core.object.getAttenuationCurve"
    # Sets the specified attenuation curve for a given attenuation object.
    ak_wwise_core_object_setattenuationcurve = u"ak.wwise.core.object.setAttenuationCurve"
    # Assigns a Switch Container's child to a Switch. This is the equivalent of doing a drag&drop of the child to a state in the Assigned Objects view. The child is always added at the end for each state.
    ak_wwise_core_switchcontainer_addassignment = u"ak.wwise.core.switchContainer.addAssignment"
    # Removes an assignment between a Switch Container's child and a State.
    ak_wwise_core_switchcontainer_removeassignment = u"ak.wwise.core.switchContainer.removeAssignment"
    # Returns the list of assignments between a Switch Container's children and states.
    ak_wwise_core_switchcontainer_getassignments = u"ak.wwise.core.switchContainer.getAssignments"
    # Creates an object of type 'type', as a child of 'parent'. Refer to \ref waapi_import for more information about creating objects. Also refer to \ref ak_wwise_core_audio_import to import audio files to Wwise.
    ak_wwise_core_object_create = u"ak.wwise.core.object.create"
    # Moves an object to the given parent. Returns the moved object.
    ak_wwise_core_object_move = u"ak.wwise.core.object.move"
    # Copies an object to the given parent.
    ak_wwise_core_object_copy = u"ak.wwise.core.object.copy"
    # Deletes the specified object.
    ak_wwise_core_object_delete = u"ak.wwise.core.object.delete"
    # Performs a query and returns specified data for each object in query result. Refer to \ref waapi_query for more information.
    ak_wwise_core_object_get = u"ak.wwise.core.object.get"
    # Creates Wwise objects and imports audio files. This function uses the same importation processor available through the Tab Delimited import in the Audio File Importer. The function returns an array of all objects created, replaced or re-used. Use the options to specify how the objects are returned. For more information, refer to \ref waapi_import.
    ak_wwise_core_audio_import = u"ak.wwise.core.audio.import"
    # Scripted object creation and audio file import from a tab-delimited file.
    ak_wwise_core_audio_importtabdelimited = u"ak.wwise.core.audio.importTabDelimited"
    # Connects the Wwise Authoring application to a Wwise Sound Engine running executable. The host must be running code with communication enabled. If only "host" is provided, Wwise connects to the first Sound Engine instance found. To distinguish between different instances, you can also provide the name of the application to connect to.
    ak_wwise_core_remote_connect = u"ak.wwise.core.remote.connect"
    # Disconnects the Wwise Authoring application from a connected Wwise Sound Engine running executable.
    ak_wwise_core_remote_disconnect = u"ak.wwise.core.remote.disconnect"
    # Retrieves all consoles available for connecting Wwise Authoring to a Sound Engine instance.
    ak_wwise_core_remote_getavailableconsoles = u"ak.wwise.core.remote.getAvailableConsoles"
    # Retrieves the connection status.
    ak_wwise_core_remote_getconnectionstatus = u"ak.wwise.core.remote.getConnectionStatus"
    # Begins an undo group. Make sure to call \ref ak_wwise_core_undo_endgroup exactly once for every ak.wwise.core.beginUndoGroup call you make. Calls to ak.wwise.core.undo.beginGroup can be nested.
    ak_wwise_core_undo_begingroup = u"ak.wwise.core.undo.beginGroup"
    # Cancels the last undo group. Please note that this does not revert the operations made since the last \ref ak_wwise_core_undo_begingroup call.
    ak_wwise_core_undo_cancelgroup = u"ak.wwise.core.undo.cancelGroup"
    # Ends the last undo group.
    ak_wwise_core_undo_endgroup = u"ak.wwise.core.undo.endGroup"
    # Retrieves the list of all object types registered in Wwise's object model. This function returns the equivalent of \ref wobjects_index .
    # \deprecated in favor of ak.wwise.core.object.getTypes
    ak_wwise_core_plugin_getlist = u"ak.wwise.core.plugin.getList"
    # Retrieves the list of all object types registered in Wwise's object model. This function returns the equivalent of \ref wobjects_index .
    ak_wwise_core_object_gettypes = u"ak.wwise.core.object.getTypes"
    # Retrieves information about an object property.
    # \deprecated in favor of ak.wwise.core.object.getPropertyInfo
    ak_wwise_core_plugin_getproperty = u"ak.wwise.core.plugin.getProperty"
    # Retrieves information about an object property.
    ak_wwise_core_object_getpropertyinfo = u"ak.wwise.core.object.getPropertyInfo"
    # Retrieves the list of property and reference names for an object.
    # \deprecated in favor of ak.wwise.core.object.getPropertyAndReferenceNames
    ak_wwise_core_plugin_getproperties = u"ak.wwise.core.plugin.getProperties"
    # Retrieves the list of property and reference names for an object.
    # \deprecated in favor of ak.wwise.core.object.getPropertyAndReferenceNames
    ak_wwise_core_object_getpropertynames = u"ak.wwise.core.object.getPropertyNames"
    # Retrieves the list of property and reference names for an object.
    ak_wwise_core_object_getpropertyandreferencenames = u"ak.wwise.core.object.getPropertyAndReferenceNames"
    # Returns true if a property is enabled based on the values of the properties it depends on.
    ak_wwise_core_object_ispropertyenabled = u"ak.wwise.core.object.isPropertyEnabled"
    # Enables debug assertions. Every call to enableAsserts with 'false' increments the ref count. Calling with true decrements the ref count. This is only available with Debug builds.
    ak_wwise_debug_enableasserts = u"ak.wwise.debug.enableAsserts"
    # Private use only.
    ak_wwise_debug_testassert = u"ak.wwise.debug.testAssert"
    # Private use only.
    ak_wwise_debug_testcrash = u"ak.wwise.debug.testCrash"
    # Enables or disables the automation mode for Wwise. This reduces the potential interruptions caused by message boxes and dialogs. For instance, enabling the automation mode silently accepts: project migration, project load log, EULA acceptance, project licence display and generic message boxes.
    ak_wwise_debug_enableautomationmode = u"ak.wwise.debug.enableAutomationMode"
    # Captures a part of the Wwise UI relative to a view.
    ak_wwise_ui_capturescreen = u"ak.wwise.ui.captureScreen"
    # Retrieves a SoundBank's inclusion list.
    ak_wwise_core_soundbank_getinclusions = u"ak.wwise.core.soundbank.getInclusions"
    # Modifies a SoundBank's inclusion list. The 'operation' argument determines how the 'inclusions' argument modifies the SoundBank's inclusion list; 'inclusions' may be added to / removed from / replace the SoundBank's inclusion list.
    ak_wwise_core_soundbank_setinclusions = u"ak.wwise.core.soundbank.setInclusions"
    # Generate a list of SoundBank in memory without writing them to disk, subscribe to ak.wwise.core.soundbank.generated to get SoundBank structure info and the bank data as base64.
    ak_wwise_core_soundbank_generate = u"ak.wwise.core.soundbank.generate"
    # Migrate and save the project.
    ak_wwise_cli_migrate = u"ak.wwise.cli.migrate"
    # Imports a tab-delimited file to create and modify different object hierarchies. The project is automatically migrated (if required). It is also automatically saved following the import.
    ak_wwise_cli_tabdelimitedimport = u"ak.wwise.cli.tabDelimitedImport"
    # Starts a command-line Wwise Authoring API server, to which client applications, using the Wwise Authoring API, can connect.
    ak_wwise_cli_waapiserver = u"ak.wwise.cli.waapiServer"
    # Creates a blank new project. The project must not already exist. If the folder does not exist, it is created.
    ak_wwise_cli_createnewproject = u"ak.wwise.cli.createNewProject"
    # Dump the objects model of a project as a JSON file.
    ak_wwise_cli_dumpobjects = u"ak.wwise.cli.dumpObjects"
    # Adds a new platform to a project. The platform must not already exist.
    ak_wwise_cli_addnewplatform = u"ak.wwise.cli.addNewPlatform"
    # External Source conversion. While External Source conversion is also triggered by SoundBank generation, this operation can be used to process sources not contained in the Wwise Project.
    ak_wwise_cli_convertexternalsource = u"ak.wwise.cli.convertExternalSource"
    # SoundBank generation. SoundBank generation is performed according to the settings stored in the project. Custom user settings are ignored when SoundBank generation is launched from the command line. However, some of these settings can be overridden from the command line.
    ak_wwise_cli_generatesoundbank = u"ak.wwise.cli.generateSoundbank"
    # Moves the project's media IDs from its work units (.wwu) to a single file, <project-name>.mediaid.  This command will force a save of all the project's work units.
    ak_wwise_cli_movemediaidstosinglefile = u"ak.wwise.cli.moveMediaIdsToSingleFile"
    # Moves the project's media IDs from a single xml file <project-name>.mediaid to its work units (.wwu).  This command will force a save of all the project's work units.
    ak_wwise_cli_movemediaidstoworkunits = u"ak.wwise.cli.moveMediaIdsToWorkUnits"
    # Loads the project and updates the contents of <project-name>.mediaid, if it exists.
    ak_wwise_cli_updatemediaidsinsinglefile = u"ak.wwise.cli.updateMediaIdsInSingleFile"
    # Retrieves the latest log for a specific channel. Refer to \ref ak_wwise_core_log_itemadded to be notified when a item is added to the log.
    ak_wwise_core_log_get = u"ak.wwise.core.log.get"
    # Creates a transport object for the given Wwise object. The return transport object can be used to play, stop, pause and resume the Wwise object via the other transport functions.
    ak_wwise_core_transport_create = u"ak.wwise.core.transport.create"
    # Destroys the given transport object.
    ak_wwise_core_transport_destroy = u"ak.wwise.core.transport.destroy"
    # Gets the state of the given transport object.
    ak_wwise_core_transport_getstate = u"ak.wwise.core.transport.getState"
    # Returns the list of transport objects.
    ak_wwise_core_transport_getlist = u"ak.wwise.core.transport.getList"
    # Executes an action on the given transport object, or all transport objects if none is specified.
    ak_wwise_core_transport_executeaction = u"ak.wwise.core.transport.executeAction"
    # Gets the min/max peak pairs, in the given region of an audio source, as a collection of binary strings (one per channel). The strings are base-64 encoded, 16-bit signed int arrays, with min and max values being interleaved. If getCrossChannelPeaks is true, only one binary string represents the peaks across all channels globally.
    ak_wwise_core_audiosourcepeaks_getminmaxpeaksinregion = u"ak.wwise.core.audioSourcePeaks.getMinMaxPeaksInRegion"
    # Gets the min/max peak pairs in the entire trimmed region of an audio source, for each channel, as an array of binary strings (one per channel). The strings are base-64 encoded, 16-bit signed int arrays, with min and max values being interleaved. If getCrossChannelPeaks is true, there is only one binary string representing peaks across all channels globally.
    ak_wwise_core_audiosourcepeaks_getminmaxpeaksintrimmedregion = u"ak.wwise.core.audioSourcePeaks.getMinMaxPeaksInTrimmedRegion"
    # Registers an array of add-on commands. Registered commands remain until the Wwise process is terminated. Refer to \ref defining_custom_commands for more information about registering commands. Also refer to \ref ak_wwise_ui_commands_executed.
    ak_wwise_ui_commands_register = u"ak.wwise.ui.commands.register"
    # Unregisters an array of add-on UI commands.
    ak_wwise_ui_commands_unregister = u"ak.wwise.ui.commands.unregister"
    # Retrieves the voices at a specific profiler capture time.
    ak_wwise_core_profiler_getvoices = u"ak.wwise.core.profiler.getVoices"
    # Retrieves active RTPCs at a specific profiler capture time.
    ak_wwise_core_profiler_getrtpcs = u"ak.wwise.core.profiler.getRTPCs"
    # Retrieves the busses at a specific profiler capture time.
    ak_wwise_core_profiler_getbusses = u"ak.wwise.core.profiler.getBusses"
    # Retrieves all parameters affecting voice volume, highpass and lowpass for a voice path, resolved from pipeline IDs.
    ak_wwise_core_profiler_getvoicecontributions = u"ak.wwise.core.profiler.getVoiceContributions"
    # Returns the current time of the specified profiler cursor, in milliseconds.
    ak_wwise_core_profiler_getcursortime = u"ak.wwise.core.profiler.getCursorTime"
    # Starts the profiler capture and returns the time at the beginning of the capture, in milliseconds.
    ak_wwise_core_profiler_startcapture = u"ak.wwise.core.profiler.startCapture"
    # Stops the profiler capture and returns the time at the end of the capture, in milliseconds.
    ak_wwise_core_profiler_stopcapture = u"ak.wwise.core.profiler.stopCapture"
    # Sent at the end of an import operation.
    ak_wwise_core_audio_imported = u"ak.wwise.core.audio.imported"
    # Sent when an object reference is changed.
    ak_wwise_core_object_referencechanged = u"ak.wwise.core.object.referenceChanged"
    # Sent when an assignment is added to a Switch Container.
    ak_wwise_core_switchcontainer_assignmentadded = u"ak.wwise.core.switchContainer.assignmentAdded"
    # Sent when an assignment is removed from a Switch Container.
    ak_wwise_core_switchcontainer_assignmentremoved = u"ak.wwise.core.switchContainer.assignmentRemoved"
    # Sent when an object is renamed. Publishes the renamed object.
    ak_wwise_core_object_namechanged = u"ak.wwise.core.object.nameChanged"
    # Sent when the object's notes are changed.
    ak_wwise_core_object_noteschanged = u"ak.wwise.core.object.notesChanged"
    # Sent when an object is created.
    ak_wwise_core_object_created = u"ak.wwise.core.object.created"
    # Sent prior to an object's deletion.
    ak_wwise_core_object_predeleted = u"ak.wwise.core.object.preDeleted"
    # Sent following an object's deletion.
    ak_wwise_core_object_postdeleted = u"ak.wwise.core.object.postDeleted"
    # Sent when an object is added as a child to another object.
    ak_wwise_core_object_childadded = u"ak.wwise.core.object.childAdded"
    # Sent when an object is removed from the children of another object.
    ak_wwise_core_object_childremoved = u"ak.wwise.core.object.childRemoved"
    # Sent when one or many curves are changed.
    ak_wwise_core_object_curvechanged = u"ak.wwise.core.object.curveChanged"
    # Sent when an attenuation curve is changed.
    ak_wwise_core_object_attenuationcurvechanged = u"ak.wwise.core.object.attenuationCurveChanged"
    # Sent when an attenuation curve's link/unlink is changed.
    ak_wwise_core_object_attenuationcurvelinkchanged = u"ak.wwise.core.object.attenuationCurveLinkChanged"
    # Sent when the watched property of an object changes.
    ak_wwise_core_object_propertychanged = u"ak.wwise.core.object.propertyChanged"
    # Sent when a single SoundBank is generated. This could be sent multiple times during SoundBank generation, for every SoundBank generated and for every platform. To generate SoundBanks, refer to \ref ak_wwise_ui_commands_execute with one of the SoundBank generation commands. Refer to \ref globalcommandsids for the list of commands.
    ak_wwise_core_soundbank_generated = u"ak.wwise.core.soundbank.generated"
    # Sent when all soundbanks are generated.
    ak_wwise_core_soundbank_generationdone = u"ak.wwise.core.soundbank.generationDone"
    # Sent when an item is added to the log. This could be used to retrieve items added to the SoundBank generation log. To retrieve the complete log, refer to \ref ak_wwise_core_log_get.
    ak_wwise_core_log_itemadded = u"ak.wwise.core.log.itemAdded"
    # Sent when the selection changes in the project.
    ak_wwise_ui_selectionchanged = u"ak.wwise.ui.selectionChanged"
    # Sent when the project has been successfully loaded.
    ak_wwise_core_project_loaded = u"ak.wwise.core.project.loaded"
    # Sent when the project begins closing.
    ak_wwise_core_project_preclosed = u"ak.wwise.core.project.preClosed"
    # Sent when the after the project is completely closed.
    ak_wwise_core_project_postclosed = u"ak.wwise.core.project.postClosed"
    # Sent when the project has been saved.
    ak_wwise_core_project_saved = u"ak.wwise.core.project.saved"
    # Sent when the transport's state has changed.
    ak_wwise_core_transport_statechanged = u"ak.wwise.core.transport.stateChanged"
    # Sent when an assert has failed. This is only available in Debug builds.
    ak_wwise_debug_assertfailed = u"ak.wwise.debug.assertFailed"
    # Sent when a command is executed. The objects for which the command is executed are sent in the publication.
    ak_wwise_ui_commands_executed = u"ak.wwise.ui.commands.executed"
