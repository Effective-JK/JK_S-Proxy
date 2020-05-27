// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_Client.c
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#include "../JKA_Proxy/Proxy_Header.hpp"

int Proxy_ClientConnect( int clientNum, qboolean firstTime, qboolean isBot )
{
	if (firstTime && !isBot)
	{
		proxy.trap->SendServerCommand(clientNum, va("print \"^3Server is using %s v%s.%s (%s) by %s\n\"", PROXY_NAME, PROXY_MAJOR, PROXY_MINOR, PROXY_STAGE, PROXY_AUTHOR));
	}

	return 0;
}
