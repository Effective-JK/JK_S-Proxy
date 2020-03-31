#include "../JKL_Proxy/Proxy_Header.h"

void JKL_ClientConnect(int clientNum, qboolean firstTime, qboolean isBot)
{
	if (firstTime && !isBot)
	{
		trap_SendServerCommand(clientNum, va("print \"^3Server is using %s v%s.%s (%s) by %s\n\"", PROXY_NAME, PROXY_MAJOR, PROXY_MINOR, PROXY_STAGE, PROXY_AUTHOR));
	}
}
