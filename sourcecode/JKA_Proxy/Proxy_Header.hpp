// ==========================
// Jedi Academy Server Proxy
// --------------------------
// File: Proxy_Header.h
// 
// Special thanks to Yberion,
// Zylden, DeathSpike and 
// BobaFett.
// ==========================

#ifdef _MSC_VER
	#include <windows.h>

	#define PROXY_LIBRARY "/proxy_jampgamex86.dll"

	#define Proxy_OpenLibrary( a ) (void *)LoadLibrary( a )
	#define Proxy_CloseLibrary( a ) FreeLibrary( (HMODULE)a )
	#define Proxy_GetFunctionAddress( a, b ) GetProcAddress( (HMODULE)a, b )

	#define ORIGINAL_ENGINE_VERSION "(internal)JAmp: v1.0.1.0 win-x86 Oct 30 2003"
#else
	#include <dlfcn.h>

	#define PROXY_LIBRARY "/proxy_jampgamei386.so"

	#define Proxy_OpenLibrary( a ) dlopen( a, RTLD_NOW )
	#define Proxy_CloseLibrary( a ) dlclose( a )
	#define Proxy_GetFunctionAddress( a, b ) dlsym( a, b )

	#define ORIGINAL_ENGINE_VERSION "JAmp: v1.0.1.1 linux-i386 Nov 10 2003"
#endif

#ifdef PROXY_DETOUR
#include "../DetourPatcher/DetourPatcher.hpp"
#endif
#include "../JKA_SDK/g_local.hpp"

#define PROXY_NAME 		"Jedi Knight: Jedi Academy Server Proxy"
#define PROXY_MAJOR 	"0"
#define PROXY_MINOR 	"1"
#define PROXY_STAGE 	"beta"
#define PROXY_AUTHOR 	"qK"

#define GAME_CVAR 		"fs_game"
#define GAME_DEFAULT 	"base"

typedef intptr_t	( QDECL *systemCallFuncPtr_t)(intptr_t command, ... );
typedef intptr_t	( *vmMainFuncPtr_t)(intptr_t command, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t );
typedef void		(*dllEntryFuncPtr_t)(systemCallFuncPtr_t);

// Custom structs
typedef struct Proxy_s 
{
	void 					*jampgameHandle;

	vmMainFuncPtr_t 		originalVmMain;
	dllEntryFuncPtr_t 		originalDllEntry;
	systemCallFuncPtr_t 	originalSystemCall;

	intptr_t				originalVmMainResponse;

	gameImport_t			*trap;

	gameImport_t			*originalNewAPIGameImportTable;
	gameExport_t			*originalNewAPIGameExportTable;

	gameImport_t			*copyNewAPIGameImportTable;
	gameExport_t			*copyNewAPIGameExportTable;

	qboolean				isOriginalEngine;
} Proxy_t ;

// Net addresses
typedef struct
{
	int						type;

	unsigned char			ip[4];
	unsigned char			ipx[10];

	unsigned short			port;
} netadr_t;

// Global defines
extern Proxy_t proxy;

// ====================
// 		Functions
// ====================

#ifdef PROXY_USENEWAPI
// Proxy_API.c
void Proxy_API_InitLayerExportTable( void );
void Proxy_API_InitLayerImportTable( void );

// -- Import table

// -- Export table
char *Proxy_API_ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );
void Proxy_API_ShutdownGame( int restart );
#endif

// Proxy_Client.c
int Proxy_ClientConnect( int clientNum, qboolean firstTime, qboolean isBot );

// Proxy_Engine.c
#if 0
void Proxy_EngineAttach( void );
void Proxy_EngineDetach( void );
void Proxy_EngineMessageBoomCheck( const char *zCmd );
void *Proxy_EngineMessageBoom( void );
void Proxy_EngineRcon( netadr_t from, /*msg_t*/ void *msg );
void Proxy_EngineDownload( /*client_t*/ void *cl );
#endif

// Proxy_Files.c
void Proxy_LoadOriginalGameLibrary( void );

// Proxy_Imports.c
char *QDECL va( const char *format, ... );
#if defined (_MSC_VER)
	// vsnprintf is ISO/IEC 9899:1999
	// abstracting this to make it portable
	int Q_vsnprintf( char *str, size_t size, const char *format, va_list ap );
#else // not using MSVC
	#define Q_vsnprintf vsnprintf
#endif

// Proxy_Main.c
void Proxy_Init( void );

// Proxy_Wrappers.c
// VM_DllSyscall can handle up to 1 (command) + 15 args
intptr_t QDECL Proxy_systemCall( intptr_t command, ... );

// Proxy_SystemCalls.c
void Translate_SystemCalls( void );
