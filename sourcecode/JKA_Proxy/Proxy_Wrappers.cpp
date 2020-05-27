// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_Wrappers.c
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#include "../JKA_Proxy/Proxy_Header.hpp"

intptr_t QDECL Proxy_systemCall( intptr_t command, ... )
{
	intptr_t args[15];
	
	va_list ap;

	va_start(ap, command);

	for (size_t i = 0; i < sizeof(args) / sizeof(args[i]); ++i)
	{
		args[i] = va_arg(ap, intptr_t);
	}

	va_end(ap);

	switch (command)
	{
		default:
			break;
	}

	return proxy.originalSystemCall(command, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10], args[11], args[12], args[13], args[14]);
}
