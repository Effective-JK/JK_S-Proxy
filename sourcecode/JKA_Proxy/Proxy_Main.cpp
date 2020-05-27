// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_Main.c
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#include "../JKA_Proxy/Proxy_Header.hpp"

Proxy_t proxy = { 0 };
int response;

// ==================================================
// Proxy_Init
// --------------------------------------------------
// Initialize our proxy module.
// ==================================================
void Proxy_Init( void )
{
	Proxy_LoadOriginalGameLibrary();

	// Engine -> Load Proxy dllEntry (store the original systemCall function pointer in originalSystemCall) ->
	// Get Original dllEntry -> send our own Proxy systemCall function pointer to the Original dllEntry
	// At the end of our Proxy systemCall function there's a call the Original systemCall function
	proxy.originalDllEntry = (dllEntryFuncPtr_t)Proxy_GetFunctionAddress(proxy.jampgameHandle, "dllEntry");
				
	// Engine -> Proxy vmMain -> Original vmMain
	proxy.originalVmMain = (vmMainFuncPtr_t)Proxy_GetFunctionAddress(proxy.jampgameHandle, "vmMain");
				
	// Send our own Proxy systemCall function pointer to the Original dllEntry
	proxy.originalDllEntry( Proxy_systemCall );

}

// ==================================================
// vmMain
// --------------------------------------------------
// The engine is making a call to the module, and we
// just happen to be between those two. We have the
// freedom to alter any message going through here.
// ==================================================
Q_CABI Q_EXPORT intptr_t vmMain(intptr_t command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4,
	intptr_t arg5, intptr_t arg6, intptr_t arg7, intptr_t arg8, intptr_t arg9, intptr_t arg10, intptr_t arg11)
{
	switch (command)
	{
		// ===================================================
		case GAME_INIT: // (int levelTime, int randomSeed, int restart)
		// ===================================================
		// This is called immediatly after entering vmMain for
		// the first time, and therefore is the best place to
		// initialize the child module. 
		// ===================================================
		{
			proxy.trap->Print("--> Proxy: Loading original game library %s\n", PROXY_LIBRARY);

			Proxy_Init();
				
			char version[MAX_STRING_CHARS];

			proxy.trap->Cvar_VariableStringBuffer("version", version, sizeof(version));

			if (!Q_stricmpn(version, ORIGINAL_ENGINE_VERSION, sizeof(ORIGINAL_ENGINE_VERSION) - 1))
			{
				proxy.isOriginalEngine = qtrue;
			}

			if (proxy.isOriginalEngine)
			{
				proxy.trap->Print("--> Proxy: Engine version \"" ORIGINAL_ENGINE_VERSION "\"\n");

				#if 0
				// Check JKA_Proxy/Proxy_Engine.cpp file	
				proxy.trap->Print("--> Proxy: Patching engine..\n");

				// Attach Deathspike's and BobaFett's engine fixes.
				Proxy_EngineAttach();
				
				proxy.trap->Print("--> Proxy: Engine properly patched..\n");
				#endif
			}
			else
			{
				proxy.trap->Print("--> Proxy: Engine version \"%s\"\n", version);
			}

			proxy.trap->Print("--> Proxy: Successfully loaded..\n");

			break;
		}
		// ===================================================
		case GAME_SHUTDOWN: // (int restart)
		// ===================================================
		// The game is shutting down, so we must be sure to
		// unplug our child module as well. Otherwise we'd leave
		// a half-intact one in memory and that could cause
		// problems.
		// ===================================================
		{
			if ( proxy.jampgameHandle )
			{
				// Send the shutdown signal to the original game module and store the response
				proxy.originalVmMainResponse = proxy.originalVmMain(command, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);

				#if 0
				// Check JKA_Proxy/Proxy_Engine.cpp file	
				if ( proxy.isOriginalEngine )
				{
					Proxy_EngineDetach();

					proxy.trap->Print("--> Proxy: Engine properly unpatched..\n");
					
				}
				#endif

				// We can close the proxy library
				Proxy_CloseLibrary( proxy.jampgameHandle );

				// Return the response of the original game module after the shutdown
				return proxy.originalVmMainResponse;
			}

			break;
		}
		// ===================================================
		case GAME_CLIENT_CONNECT: //(int clientNum, qboolean firstTime, qboolean isBot)
		// ===================================================
		// A client is connecting, but we do not know how the
		// loaded modification wants to respond. We will let
		// it do its job first.
		// ===================================================
		{
			if (( response = Proxy_ClientConnect( arg0, arg1, qfalse )) != 0 )
			{
				return response;
			}

			break;
		}
		default:
			break;
	}

	return proxy.originalVmMain( command, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11 );
}

// ==================================================
// dllEntry
// --------------------------------------------------
// The engine sends the system call function pointer 
// to the game module through dllEntry.
// This function gets called to pass the pointer to
// the engine function to handle system calls.
// ==================================================
Q_CABI Q_EXPORT void dllEntry( systemCallFuncPtr_t systemCallFuncPtdr )
{
	proxy.originalSystemCall = systemCallFuncPtdr;

	// Create trap calls available directly within the proxy (proxy.trap->*)
	Translate_SystemCalls();
}

// ==================================================
// GetModuleAPI
// --------------------------------------------------
// The engine will search GetModuleAPI in the proxy,
// then, the proxy will search it on the original
// jampgame handle.
// ==================================================
#ifdef PROXY_USENEWAPI
Q_CABI Q_EXPORT gameExport_t *QDECL GetModuleAPI( int apiVersion, gameImport_t *import )
{
	assert(import);

	// Needed for trap_... calls inside of the proxy
	proxy.trap = import;

	Com_Printf = proxy.trap->Print;
	Com_Error = proxy.trap->Error;

	if (apiVersion != GAME_API_VERSION)
	{
		proxy.trap->Print("--> Proxy: Mismatched GAME_API_VERSION: expected %i, got %i\n", GAME_API_VERSION, apiVersion);
		return nullptr;
	}

	static gameImport_t copyNewAPIGameImportTable_ = { 0 };
	static gameExport_t copyNewAPIGameExportTable_ = { 0 };

	proxy.originalNewAPIGameImportTable = import;
	memcpy(&copyNewAPIGameImportTable_, import, sizeof(gameImport_t));
	proxy.copyNewAPIGameImportTable = &copyNewAPIGameImportTable_;

	Proxy_LoadOriginalGameLibrary();

	GetGameAPI_t jampgameGetModuleAPI = (GetGameAPI_t)Proxy_GetFunctionAddress(proxy.jampgameHandle, "GetModuleAPI");

	if (!jampgameGetModuleAPI)
	{
		proxy.trap->Print("--> Proxy: Failed to find GetModuleAPI function, exiting\n");
		return nullptr;
	}

	proxy.originalNewAPIGameExportTable = jampgameGetModuleAPI(apiVersion, &copyNewAPIGameImportTable_);
	memcpy(&copyNewAPIGameExportTable_, proxy.originalNewAPIGameExportTable, sizeof(gameExport_t));
	proxy.copyNewAPIGameExportTable = &copyNewAPIGameExportTable_;

	Proxy_API_InitLayerExportTable();
	Proxy_API_InitLayerImportTable();

	return proxy.copyNewAPIGameExportTable;
}
#endif
