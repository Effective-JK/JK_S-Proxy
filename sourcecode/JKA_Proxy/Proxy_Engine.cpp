// ==================================================
// File from JMPProxy by Deathspike
// --------------------------------------------------
// A project aimed at controlling the extensions used
// on both Windows/Linux, adding new capabilities,
// commands or fixes.
// --------------------------------------------------
// This file holds all the calls made into the engine
// which are intercepted there as well. Detouring the
// actual engine is done here.
// https://github.com/Deathspike/JMPProxy
// ==================================================

// >>>>>>>>>>>>>>>>>>>>>>>>>><><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// IMPORTANT: This file is just an example. It is not used as
// it's inteded to run using OpenJKded.
// Proxy_Shell, and DetourPatcher have been modified.
// NEEDS TO BE FIXED before being used.
// >>>>>>>>>>>>>>>>>>>>>>>>>><><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#if 0
#include "../JKA_Proxy/Proxy_Header.hpp"
#include "../JKA_Proxy/Proxy_Shell.hpp"

// ==================================================
// D E T O U R E D   F U N C T I O N S
// ==================================================

#define SVC_RemoteCommand		(( bWindows ) ? 0x443CE0 : 0x8056B14 )
#define SV_BeginDownload_f		(( bWindows ) ? 0x43B3C0 : 0x804EA44 )
#define SV_MessageBoom			(( bWindows ) ? 0x443F7F : 0x8056E23 )
#define SV_MessageBoomLuigi		(( bWindows ) ? 0x418B2C : 0x807803D )

// ==================================================
// I M P O R T E D   F U N C T I O N S
// ==================================================

#define NET_AdrToString			(( bWindows ) ? 0x419F10 : 0x807B314 )
#define Cmd_Argv				(( bWindows ) ? 0x40F490 : 0x812C264 )

// ==================================================
// I M P O R T E D   V A R I A B L E S
// ==================================================

#define SV_RconPassword			(( bWindows ) ? 0x606210 : 0x83121D4 )

// ==================================================
// These are the imported functions in a C-syntax
// style so we can use them without worries about the
// operating system.
// ==================================================

char *(*pCmd_Argv)(int);
char *(*pNET_AdrToString)(netadr_t);

// ==================================================
// These are the trampoline locations to run the
// original functions which have been intercepted.
// ==================================================

void (*pSVC_RemoteRcon)(netadr_t from, /*msg_t*/ void *msg);
void (*pSV_BeginDownload_f)( /*client_t*/ void *cl);
unsigned char *pMessageBoom;

// ==================================================
// These are child functions and aren't correct at
// times, basically when a modification decides to
// ignore the traditional naming. Works only for Linux!
// ==================================================

int (*pG_FilterPacket)(char*);

// ==================================================
// Proxy_EngineAttach
// --------------------------------------------------
// Attach all the engine functions, shouldn't be very
// long without these detours. This allows me to detour
// calls into engine functions so I can add additional
// options, features or fixes.
// ==================================================

void Proxy_EngineAttach(void)
{
	// Detoured Functions	
	pSVC_RemoteRcon = (void (*)(netadr_t, void*))	Attach((unsigned char*)SVC_RemoteCommand, (unsigned char*)&Proxy_EngineRcon);
	pSV_BeginDownload_f = (void (*)(void*))			Attach((unsigned char*)SV_BeginDownload_f, (unsigned char*)&Proxy_EngineDownload);
	pMessageBoom = Attach((unsigned char*)SV_MessageBoom, (unsigned char*)Proxy_EngineMessageBoom());

	// Imported Functions
	pCmd_Argv = (char *(*)(int)) Cmd_Argv;
	pNET_AdrToString = (char* (*)(netadr_t)) NET_AdrToString;

	// Child Functions
	pG_FilterPacket = (int (*)(char*)) Proxy_GetFunctionAddress(proxy.jampgameHandle, "G_FilterPacket");

	// Alter the message boom attachment from a jump to a call. Remove the path made by Aluigi, since it can cause problems.
	Patch((unsigned char*)SV_MessageBoom, (unsigned char*)0x15);
	Patch((unsigned char*)SV_MessageBoomLuigi, (unsigned char*)0x3FF);
}

// ==================================================
// Proxy_EngineDetach
// --------------------------------------------------
// We are closing and we should remove the detour,
// otherwise the jump might be done to an undefined
// function and we can't restore the original bytes
// anymore.
// ==================================================

void Proxy_EngineDetach( void )
{
	Detach((unsigned char*)SVC_RemoteCommand, (unsigned char*)pSVC_RemoteRcon);
	Detach((unsigned char*)SV_BeginDownload_f, (unsigned char*)pSV_BeginDownload_f);
	Detach((unsigned char*)SV_MessageBoom, (unsigned char*)pMessageBoom);
}

// ==================================================
// Proxy_EngineMessageBoomCheck && Proxy_EngineMessageBoom
// --------------------------------------------------
// Check for potential dangerous command coming in
// through a connectionless packet, filter if
// required to dos. Exploit scanner is known as
// q3infoboom, fix utilizes the Shell System.
// --------------------------------------------------
// http://aluigi.altervista.org/adv/q3infoboom-adv.txt
// --------------------------------------------------
// Credits: BobaFett
// ==================================================

void Proxy_EngineMessageBoomCheck( const char *zCmd )
{
	int		bNotConnect = qtrue;
	char	*s;

	if (Q_stricmp(zCmd, "getstatus") == 0 || Q_stricmp(zCmd, "getinfo") == 0 || (bNotConnect = Q_stricmp(zCmd, "connect")) == 0)
	{
		s = (char*)pCmd_Argv(1);

		if (strlen(s) > ((bNotConnect) ? 32 : 980))
		{
			s[((bNotConnect) ? 32 : 980)] = 0;
		}
	}
}

void *Proxy_EngineMessageBoom( void )
{
	__sh_Prologue;
	{
		__asm1__(pushad)
			__asm1__(push	ebx)
			__asm1__(call	Proxy_EngineMessageBoomCheck)
			__asm2__(add	esp, 4)
			__asm1__(popad)
			__asm1__(jmp	Q_stricmp)
	}
	__sh_Epilogue;
}

// ==================================================
// Proxy_EngineRcon
// --------------------------------------------------
// An rcon command is being issued, but usually the
// time-limit check is done before password inspection.
// --------------------------------------------------
// http://aluigi.altervista.org/papers/multircon.zip
// ==================================================

void Proxy_EngineRcon( netadr_t from, /*msg_t*/ void *msg )
{
	cvar_t 		*pSV_RconPassword = *(cvar_t**)SV_RconPassword;
	char *zIP =  pNET_AdrToString(from);
	static int	 iAttempt = 0;
	static int	 iStoredIP[4];
	int			 iIP[4];

	// If this is a linux machine, run his IP to check for banned state.
	if (pG_FilterPacket && !pG_FilterPacket(zIP))
	{
		return;
	}

	// The length of the rconPassword is missing, none set?
	if (!strlen(pSV_RconPassword->string))
	{
		return;
	}

	// RconPassword is OK, continue with this guy!
	if (Q_stricmp(pCmd_Argv(1), pSV_RconPassword->string) == 0)
	{
		pSVC_RemoteRcon(from, msg);
		return;
	}

	// No point in checking for banned states without being able to do so.
	if (!pG_FilterPacket)
	{
		return;
	}

	// Scan his IP :)
	sscanf(zIP, "%i.%i.%i.%i", &iIP[0], &iIP[1], &iIP[2], &iIP[3]);

	// ==================================================
	// Check if the previously stored IP matches this one,
	// therefore this is a stupid thing on multiple
	// attackers. Lets not hope for that just yet.
	// ==================================================

	if (iAttempt >= 65000)
	{
		return;
	}

	if (iIP[0] == iStoredIP[0] && iIP[1] == iStoredIP[1] && iIP[2] == iStoredIP[2] && iIP[3] == iStoredIP[3])
	{
		if ((iAttempt = (iAttempt + 1)) == 3)
		{
			trap_SendConsoleCommand(EXEC_APPEND, va("addip %i.%i.%i.%i", iIP[0], iIP[1], iIP[2], iIP[3]));
		}
	}
	else
	{
		iAttempt = 0;
	}

	// ==================================================
	// Store this guy his IP into our tracking static, which be mind you - gets
	// cleared on each round restart. For most RPG-like-servers the map rarely
	// gets changed, not a problem.
	// ==================================================

	iStoredIP[0] = iIP[0];
	iStoredIP[1] = iIP[1];
	iStoredIP[2] = iIP[2];
	iStoredIP[3] = iIP[3];
}

// ==================================================
// Proxy_EngineDownload
// --------------------------------------------------
// A download is requested but is not being checked
// for validity. We shall do this before allowing it.
// --------------------------------------------------
// http://aluigi.altervista.org/poc/q3dirtrav.zip
// ==================================================

void Proxy_EngineDownload( /*client_t*/ void *cl )
{
	char *pFile = pCmd_Argv(1);

	// We avoid the client downloading files with any other extension than .pk3
	if (pFile == NULL || strlen(pFile) < 5 || Q_stricmpn(pFile + strlen(pFile) - 5, ".pk3", 4))
	{
		return;
	}

	pSV_BeginDownload_f(cl);
}
#endif
