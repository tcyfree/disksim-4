/*
 * DiskSim Storage Subsystem Simulation Environment (Version 4.0)
 * Revision Authors: John Bucy, Greg Ganger
 * Contributors: John Griffin, Jiri Schindler, Steve Schlosser
 *
 * Copyright (c) of Carnegie Mellon University, 2001-2008.
 *
 * This software is being provided by the copyright holders under the
 * following license. By obtaining, using and/or copying this software,
 * you agree that you have read, understood, and will comply with the
 * following terms and conditions:
 *
 * Permission to reproduce, use, and prepare derivative works of this
 * software is granted provided the copyright and "No Warranty" statements
 * are included with all reproductions and derivative works and associated
 * documentation. This software may also be redistributed without charge
 * provided that the copyright and "No Warranty" statements are included
 * in all redistributions.
 *
 * NO WARRANTY. THIS SOFTWARE IS FURNISHED ON AN "AS IS" BASIS.
 * CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED AS TO THE MATTER INCLUDING, BUT NOT LIMITED
 * TO: WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY
 * OF RESULTS OR RESULTS OBTAINED FROM USE OF THIS SOFTWARE. CARNEGIE
 * MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT
 * TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * COPYRIGHT HOLDERS WILL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE
 * OR DOCUMENTATION.
 *
 */

/*
 * A sample skeleton for a system simulator that calls DiskSim as
 * a slave.
 *
 * Contributed by Eran Gabber of Lucent Technologies - Bell Laboratories
 *
 * Usage:
 *	syssim <parameters file> <output file> <max. block number>
 * Example:
 *	syssim parv.seagate out 2676846
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "syssim_driver.h"
#include "disksim_interface.h"
#include "disksim_rand48.h"


#define	BLOCK	4096
#define	SECTOR	512
#define	BLOCK2SECTOR	(BLOCK/SECTOR)

typedef	struct	{
  int n;
  double sum;
  double sqr;
} Stat;


static SysTime now = 0;		/* current time */
static SysTime next_event = -1;	/* next event */
static int completed = 0;	/* last request was completed */
static Stat st;


void
panic(const char *s)
{
  perror(s);
  exit(1);
}


void
add_statistics(Stat *s, double x)
{
  s->n++;
  // printf("s->sum=%f x=%f\n", s->sum, x);
  s->sum += x;
  s->sqr += x*x;
}


void
print_statistics(Stat *s, const char *title)
{
  double avg, std;

  avg = s->sum/s->n;
  std = sqrt((s->sqr - 2*avg*s->sum + s->n*avg*avg) / s->n);
  // printf("sum=%f n=%d\n", s->sum, s->n);
  printf("%s: n=%d average=%f std. deviation=%f\n", title, s->n, avg, std);
}

void
avg_statistics_pro(Stat *s, const char *title)
{
  double avg, std;
  avg = s->sum/s->n;
  std = sqrt((s->sqr - 2*avg*s->sum + s->n*avg*avg) / s->n);
  //改成纳秒，和ssdsim同步
  printf("%d\n", (int)(avg * 1000000));
}

void
avg_statistics(Stat *s, const char *title)
{
  double avg, std;

  avg = s->sum/s->n;
  std = sqrt((s->sqr - 2*avg*s->sum + s->n*avg*avg) / s->n);
  //改成纳秒，和ssdsim同步
  printf("%d\n", (int)(avg * 1000000));
}

/*
 * Schedule next callback at time t.
 * Note that there is only *one* outstanding callback at any given time.
 * The callback is for the earliest event.
 */
void
syssim_schedule_callback(disksim_interface_callback_t fn, 
			 SysTime t, 
			 void *ctx)
{
  next_event = t;
}


/*
 * de-scehdule a callback.
 */
void
syssim_deschedule_callback(double t, void *ctx)
{
  next_event = -1;
}


void
syssim_report_completion(SysTime t, struct disksim_request *r, void *ctx)
{
  completed = 1;
  now = t;
  add_statistics(&st, t - r->start);
}

int main(int argc, char *argv[])
{
	int i;
	int nsectors;
	struct stat buf;
	struct disksim_interface *disksim;

	if (argc != 6 ) {
		fprintf(stderr, "usage: %s param_file tracefile time lpn size isread\n",argv[0]);
		exit(1);
	}

	if (stat(argv[1], &buf) < 0)
		panic(argv[1]);

	disksim = disksim_interface_initialize(argv[1], "system.out",
			syssim_report_completion,
			syssim_schedule_callback,
			syssim_deschedule_callback,0,0,0);

	double time;
	int devno = 0;
	unsigned long logical_block_number;
	int size;
	int isread;
  // printf("%lf %d %ld %d %d\n", time, devno, logical_block_number,size, isread);
  sscanf(argv[2], "%lf", &time);
  sscanf(argv[3], "%d", &logical_block_number);
  sscanf(argv[4], "%d", &size);
  sscanf(argv[5], "%d", &isread);
  time = time / 1000000000;                                           // 将ns改成s
  struct disksim_request *r = malloc(sizeof(struct disksim_request)); // 这里需要特别注意，每个request都需要不同的内存
  r->start = time;
  r->flags = isread;
  r->devno = devno;
  r->blkno = logical_block_number;
  r->bytecount = size * 512 * 16; // 必须是一个块(512B)的整数倍
  disksim_interface_request_arrive(disksim, time, r);

  while(next_event >= 0) {    //处理剩下的事件
		now = next_event;
		next_event = -1;
		disksim_interface_internal_event(disksim, now, 0);
	}

	disksim_interface_shutdown(disksim, now);

	avg_statistics_pro(&st, "response time");
	print_statistics(&st, "response time");

	exit(0);
}
