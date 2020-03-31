// ==================================================
// Assembly Shell System by Deathspike & BobaFett
// --------------------------------------------------
// A definition-based method to easily create "naked"
// functions with assembly code for both Windows
// and Linux.
// --------------------------------------------------
// GCC must compile with the -masm=intel switch!
// --------------------------------------------------
// A compile has the tendancy to add an prologue and
// epilogue to the inline-assembly in a function. Upon
// calling the function, we return the actual address
// for the naked code - effectively skipping the compiler
// generated assembly.
//
// NOTE: Do *NOT* let your own code run into the shell
// code. It should return or make a jump no matter what
// the circumstances are. If it reaches the shell code
// the stack will become corrupted and is likely to crash.
//
// Look below the definitions for a simple example!
// ==================================================

#ifdef _WIN32

	#define __asm1__( a )			__asm a
	#define __asm2__( a, b )		__asm a, b
	#define __asmL__( a )			__asm a##:
	#define __sh_Get( a, b )		void *( *b )( void ) = a();
	#define __sh_GetPointer( a )	a();
	#define __sh_GetRun( a, b )		void *( *b )( void ) = a(); b();
	#define __sh_Prologue			__asm lea eax, [__hookStart] \
									__asm jmp __hookEnd \
									__asm __hookStart:
	#define __sh_Epilogue			__asm __hookEnd:

#else 

	#define __asm1__( a )			__asm__( #a "\n" );
	#define __asm2__( a, b )		__asm__( #a ", " #b "\n" );
	#define __asmL__( a )			__asm__( ".att_syntax\n" ); \
									__asm__( #a ":\n" ); \
									__asm__( ".intel_syntax noprefix\n" );
	#define __sh_Get( a, b )		void *( *b )( void ) = a();
	#define __sh_GetPointer( a )	a();
	#define __sh_GetRun( a, b )		void *( *b )( void ) = a(); b();
	#define __sh_Prologue			__asm__("lea eax, [__hookStart]\n"); \
									__asm__("jmp __hookEnd\n"); \
									__asm__(".att_syntax\n"); \
									__asm__("__hookStart:\n"); \
									__asm__(".intel_syntax noprefix\n");
	#define __sh_Epilogue			__asm__(".att_syntax\n"); \
									__asm__("__hookEnd:\n"); \
									__asm__(".intel_syntax noprefix\n");

#endif

// ==================================================
//
// char *zMessage = "This a simple test message!\n";
//
// void *ShellExample( void )
// {
// 	__sh_Prologue;
// 	{
// 		__asm1__( push	zMessage	)
// 		__asm1__( call	printf		)
// 		__asm2__( add	esp, 4		)
// 		__asm1__( ret			)
// 	}
// 	__sh_Epilogue;
// }
// 
// int main( int argc, char **argv )
// {
// 	__sh_GetRun( ShellExample, pShellExample );
// 	return ( EXIT_SUCCESS );
// }
// 
// ==================================================