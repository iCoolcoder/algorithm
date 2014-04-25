#include "minicrt.h"

#ifdef WIN32
#include <Windows.h>
#endif

extern int main(int argc, char* argv[]);
void exit(int);

static void crt_fatal_error(const char* msg)
{
	// printf("fatal error: %s", msg);
	exit(1);
}

void mini_crt_entry(void)
{
	int ret;

#ifdef WIN32
	int flag = 0;
	int argc = 0;
	char* argv[16]; // limit to 16 parameters
	char* cl = GetCommandLineA();

	// Parse command line
	argv[0] = cl;
	argc++;
	while(*cl)
	{
		if(*cl == '\"')
			if(flag == 0) flag = 1;
			else flag = 0;
		else if(*cl == ' ' && flag == 0)
		{
			if(*(cl + 1))
			{
				argv[argc] = cl + 1;
				argc++;
			}
			*cl = '\0';
		}
		cl++;
	}
#else
	// TODO: linux part
#endif

	if(!mini_crt_heap_init())
		crt_fatal_error("heap initialize failed");
	
	if(!mini_crt_io_init())
		crt_fatal_error("IO initialize failed");

	ret = main(argc, argv);
	exit(ret);
}

void exit(int exitCode)
{
	// mini_crt_call_exit_routine();

#ifdef WIN32
	ExitProcess(exitCode);
#else
	// TODO: linux part
#endif
}