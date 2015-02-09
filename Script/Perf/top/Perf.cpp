#include "Perf.h"
#include <string.h>

#define ONE_MB (1024 * 1024)
#define SMLBUFSIZ 256

Perf * Perf::_instance = NULL;

Perf * Perf::get_instance()
{
	if (NULL == _instance)
		_instance = new Perf();
	return _instance;
}

void Perf::destory()
{
	delete _instance;
}

Perf::Perf()
{
	_cpu = (cpu_t *)malloc(sizeof(cpu_t));
	_sys_info.online_processors = sysconf(_SC_NPROCESSORS_ONLN);
}

Perf::~Perf()
{
	free(_cpu);
}

cpu_t *Perf::cpus_refresh()
{
	static FILE *fp = NULL;
	int i;
	int num;
	char buf[SMLBUFSIZ];

	if (!fp) {
		if (!(fp = fopen("/proc/stat", "r")))
			printf("failed /proc/stat open: %s", strerror(errno));
	}
	rewind(fp);
	fflush(fp);

	// first value the last slot with the cpu summary line
	if (!fgets(buf, sizeof(buf), fp)) printf("failed /proc/stat read.");

	_cpu->x = 0;
	_cpu->y = 0;
	_cpu->z = 0;
	num = sscanf(buf, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
		&_cpu->u,
		&_cpu->n,
		&_cpu->s,
		&_cpu->i,
		&_cpu->w,
		&_cpu->x,
		&_cpu->y,
		&_cpu->z
		);
	if (num < 4)
		printf("failed /proc/stat read. parameters less than 4");

	return _cpu;
}

void Perf::update_cpu_rate()
{
#define TRIMz(x)  ((tz = (long long)(x)) < 0 ? 0 : tz)
	long long u_frme, s_frme, n_frme, i_frme, w_frme, x_frme, y_frme, z_frme, tot_frme, tz;
	float scale;

	u_frme = _cpu->u - _cpu->u_sav;
	s_frme = _cpu->s - _cpu->s_sav;
	n_frme = _cpu->n - _cpu->n_sav;
	i_frme = TRIMz(_cpu->i - _cpu->i_sav);
	w_frme = _cpu->w - _cpu->w_sav;
	x_frme = _cpu->x - _cpu->x_sav;
	y_frme = _cpu->y - _cpu->y_sav;
	z_frme = _cpu->z - _cpu->z_sav;
	tot_frme = u_frme + s_frme + n_frme + i_frme + w_frme + x_frme + y_frme + z_frme;
	if (tot_frme < 1) tot_frme = 1;
	scale = 100.0 / (float)tot_frme;

	_cpu->u_sav = _cpu->u;
	_cpu->s_sav = _cpu->s;
	_cpu->n_sav = _cpu->n;
	_cpu->i_sav = _cpu->i;
	_cpu->w_sav = _cpu->w;
	_cpu->x_sav = _cpu->x;
	_cpu->y_sav = _cpu->y;
	_cpu->z_sav = _cpu->z;

	_sys_info.cpu_rate = (tot_frme - i_frme) * scale;

#undef TRIMz
}

void Perf::update_mem_state()
{
	long page_size = sysconf(_SC_PAGESIZE);
	long num_pages = sysconf(_SC_PHYS_PAGES);
	long free_pages = sysconf(_SC_AVPHYS_PAGES);
	ul_t mem = (ul_t)((ul_t)num_pages * (ul_t)page_size);
	_sys_info.mem_ocy = (mem /= ONE_MB);
	ul_t free_mem = (ul_t)((ul_t)free_pages * (ul_t)page_size);
	_sys_info.free_mem_ocy = (free_mem /= ONE_MB);
}

void Perf::keep_update()
{
	update_mem_state();
	cpus_refresh();
	update_cpu_rate();
}

sys_info_t Perf::get_sys_info()
{
	return _sys_info;
}

