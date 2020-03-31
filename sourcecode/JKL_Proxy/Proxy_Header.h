#ifdef _MSC_VER
#include <windows.h>

#define Q_EXPORT __declspec(dllexport)

#define PROXY_LIBRARY "/jkl_jampgamex86.dll"

#define Proxy_OpenLibrary(a) (void *)LoadLibrary(a)
#define Proxy_CloseLibrary(a) FreeLibrary((HMODULE)a)
#define Proxy_GetFunctionAddress(a, b) GetProcAddress((HMODULE)a, b)
#else
#include <dlfcn.h>

#define PROXY_LIBRARY "/jkl_jampgame386.so"

#define Proxy_OpenLibrary(a, b) dlopen(a, RTLD_NOW)
#define Proxy_CloseLibrary(a) dlclose(a)
#define Proxy_GetFunctionAddress(a, b) dlsym(a, b)
#endif

#include "../DetourPatcher/DetourPatcher.h"
#include "../JKA_SDK/g_local.h"

#define PROXY_NAME "Jedi Knight Legends Proxy"
#define PROXY_MAJOR "0"
#define PROXY_MINOR "1"
#define PROXY_STAGE "beta"
#define PROXY_AUTHOR "qK"

#define GAME_CVAR "fs_game"
#define GAME_DEFAULT "base"

typedef int		(QDECL* systemCallFuncPtr_t)(int command, ...);
typedef int		(*vmMainFuncPtr_t)(int command, int, int, int, int, int, int, int, int, int, int, int, int);
typedef void	(*dllEntryFuncPtr_t)(systemCallFuncPtr_t);

// Custom structs
typedef struct Proxy_s {
	void *jampgameHandle;

	vmMainFuncPtr_t originalVmMain;
	dllEntryFuncPtr_t originalDllEntry;
	systemCallFuncPtr_t originalSystemCall;

	int originalVmMainResponse;
} Proxy_t ;

typedef struct
{
	int				type;

	unsigned char	ip[4];
	unsigned char	ipx[10];

	unsigned short	port;

} netadr_t;

// Global defines
extern Proxy_t proxy;
// Proxy_Client.c
void JKL_ClientConnect(int clientNum, qboolean firstTime, qboolean isBot);

// Proxy_Engine.c
void JKL_EngineAttach(void);
void JKL_EngineDetach(void);
void JKL_EngineMessageBoomCheck(const char* zCmd);
void *JKL_EngineMessageBoom(void);
void JKL_EngineRcon(netadr_t from, /*msg_t*/ void* msg);
void JKL_EngineDownload( /*client_t*/ void* cl);

// Proxy_Imports.c
char *QDECL va(const char* format, ...);

#if defined (_MSC_VER)
// vsnprintf is ISO/IEC 9899:1999
// abstracting this to make it portable
int Q_vsnprintf(char* str, size_t size, const char* format, va_list args);
#else // not using MSVC
#define Q_vsnprintf vsnprintf
#endif

// Proxy_Wrappers.c

int QDECL Proxy_systemCall(int command, int* arg1, int* arg2, int* arg3, int* arg4, int* arg5, int* arg6, int* arg7, int* arg8, int* arg9, int* arg10);
