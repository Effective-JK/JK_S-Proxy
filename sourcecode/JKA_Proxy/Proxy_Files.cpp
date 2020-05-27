// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_Files.c
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#include "../JKA_Proxy/Proxy_Header.hpp"

// ==================================================
// Proxy_LoadOriginalGameLibrary
// --------------------------------------------------
// Proxy will attempt to load the original jampgame
// library file. If this file does not exist, 
// immediatly exit the process.
// ==================================================
void Proxy_LoadOriginalGameLibrary( void ) 
{  
	char fs_gameBuffer[MAX_OSPATH];
	size_t pathLength = 0;
	size_t tmpLength = 0;

	fs_gameBuffer[0] = 0;

	proxy.trap->Cvar_VariableStringBuffer( GAME_CVAR, fs_gameBuffer, sizeof(fs_gameBuffer) );

	pathLength = strlen( fs_gameBuffer );

	// length 0 means that fs_game is set to the default value,
	// which is the default "base" game folder name.
	if (pathLength == 0)
	{
		pathLength += strlen( GAME_DEFAULT );
		memcpy( fs_gameBuffer, GAME_DEFAULT, pathLength );
	}

	tmpLength = strlen( PROXY_LIBRARY );
	memcpy( fs_gameBuffer + pathLength, PROXY_LIBRARY, tmpLength );
	pathLength += tmpLength;

	fs_gameBuffer[pathLength] = '\0';

	proxy.jampgameHandle = Proxy_OpenLibrary( fs_gameBuffer );

	if (proxy.jampgameHandle == NULL)
	{
		exit( EXIT_FAILURE );
	}
}
