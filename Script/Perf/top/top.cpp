#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/times.h>

#include "Perf.h"

#define ONE_MB (1024 * 1024)

#define CPU_FREQUENCY    2930000000UL

using namespace std;

int main(int argc, char *argv[])
{
	printf("The number of processors configured is :%ld\n",
		sysconf(_SC_NPROCESSORS_CONF));
	printf("The number of processors currently online (available) is :%ld\n",
		sysconf(_SC_NPROCESSORS_ONLN));
	printf("The pagesize: %ld\n", sysconf(_SC_PAGESIZE));
	printf("The number of pages: %ld\n", sysconf(_SC_PHYS_PAGES));
	printf("The number of available pages: %ld\n", sysconf(_SC_AVPHYS_PAGES));
	printf("The memory size: %lld MB\n",
		(long long)sysconf(_SC_PAGESIZE) * (long long)sysconf(_SC_PHYS_PAGES) / ONE_MB);
	printf("The number of files max opened:: %ld\n", sysconf(_SC_OPEN_MAX));
	printf("The number of ticks per second: %ld\n", sysconf(_SC_CLK_TCK));
	printf("The max length of host name: %ld\n", sysconf(_SC_HOST_NAME_MAX));
	printf("The max length of login name: %ld\n", sysconf(_SC_LOGIN_NAME_MAX));

	long num_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	printf("CPU number:%ld\n", num_cpu);

	Perf *pf = Perf::get_instance();
	while (1)
	{
		pf->keep_update();
		std::cout << pf->get_sys_info().cpu_rate << endl;
		sleep(3);
	}

	return 0;
}

