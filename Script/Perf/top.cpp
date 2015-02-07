#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>

#define SMLBUFSIZ 256

typedef unsigned long long ul_t;

typedef struct cpu_t {
	ul_t u, n, s, i, w, x, y, z; // as represented in /proc/stat
	ul_t u_sav, s_sav, n_sav, i_sav, w_sav, x_sav, y_sav, z_sav; // in the order of our display
} cpu_t;

cpu_t *cpus_refresh(cpu_t *cpus)
{
	static FILE *fp = NULL;
	int i;
	int num;
	char buf[SMLBUFSIZ];

	if (!fp) {
		if (!(fp = fopen("/proc/stat", "r")))
			fprintf(stderr, "Failed /proc/stat open: %s", strerror(errno));
	}
	rewind(fp);
	fflush(fp);

	// first value the last slot with the cpu summary line
	if (!fgets(buf, sizeof(buf), fp)) fprintf(stderr, "failed /proc/stat read");

	cpus->x = 0;
	cpus->y = 0;
	cpus->z = 0;
	num = sscanf(buf, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
		&cpus->u,
		&cpus->n,
		&cpus->s,
		&cpus->i,
		&cpus->w,
		&cpus->x,
		&cpus->y,
		&cpus->z
		);
	if (num < 4)
		fprintf(stderr, "failed /proc/stat read");

	return cpus;
}

static void format_output(cpu_t *cpu, const char *pfx)
{
#define TRIMz(x)  ((tz = (long long)(x)) < 0 ? 0 : tz)
	long long u_frme, s_frme, n_frme, i_frme, w_frme, x_frme, y_frme, z_frme, tot_frme, tz;
	float scale;

	u_frme = cpu->u - cpu->u_sav;
	s_frme = cpu->s - cpu->s_sav;
	n_frme = cpu->n - cpu->n_sav;
	i_frme = TRIMz(cpu->i - cpu->i_sav);
	w_frme = cpu->w - cpu->w_sav;
	x_frme = cpu->x - cpu->x_sav;
	y_frme = cpu->y - cpu->y_sav;
	z_frme = cpu->z - cpu->z_sav;
	tot_frme = u_frme + s_frme + n_frme + i_frme + w_frme + x_frme + y_frme + z_frme;
	if (tot_frme < 1) tot_frme = 1;
	scale = 100.0 / (float)tot_frme;

	fprintf(stderr, "%s: %.2f%%us,   %.2f%%sy,   %.2f%%ni,   %.2f%%id,   %.2f%%wa,   %.2f%%hi,   %.2f%%si,   %.2f%%st\n",
		pfx,
		(float)u_frme * scale,
		(float)s_frme * scale,
		(float)n_frme * scale,
		(float)i_frme * scale,
		(float)w_frme * scale,
		(float)x_frme * scale,
		(float)y_frme * scale,
		(float)z_frme * scale);

	cpu->u_sav = cpu->u;
	cpu->s_sav = cpu->s;
	cpu->n_sav = cpu->n;
	cpu->i_sav = cpu->i;
	cpu->w_sav = cpu->w;
	cpu->x_sav = cpu->x;
	cpu->y_sav = cpu->y;
	cpu->z_sav = cpu->z;

#undef TRIMz
}

int main(){
	cpu_t * cpu;
	cpu = (cpu_t *)malloc(sizeof(cpu_t));
	memset(cpu, 0, sizeof(cpu_t));


	while (1){
		cpus_refresh(cpu);
		format_output(cpu, "Cpu usage:  ");
		sleep(1);
	}
}
