// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_API.c
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#ifdef PROXY_USENEWAPI
#include "../JKA_Proxy/Proxy_Header.hpp"

void Proxy_API_InitLayerExportTable( void )
{
	proxy.copyNewAPIGameExportTable->ClientConnect = Proxy_API_ClientConnect;
	proxy.copyNewAPIGameExportTable->ShutdownGame = Proxy_API_ShutdownGame;
}

void Proxy_API_InitLayerImportTable( void )
{
	// TODO
}

// ==================================================
// IMPORT TABLE
// ==================================================

// ==================================================
// EXPORT TABLE
// ==================================================

char *Proxy_API_ClientConnect( int clientNum, qboolean firstTime, qboolean isBot )
{
	Proxy_ClientConnect( clientNum, firstTime, isBot );

	return proxy.originalNewAPIGameExportTable->ClientConnect( clientNum, firstTime, isBot );
}

void Proxy_API_ShutdownGame( int restart )
{
	if (proxy.jampgameHandle)
	{	
		proxy.originalNewAPIGameExportTable->ShutdownGame( restart );

		// We can close our proxy library
		Proxy_CloseLibrary( proxy.jampgameHandle );
	}
} 
#endif
