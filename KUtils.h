#ifndef KUtils_H
#define KUtils_H

//Perf record related
#pragma once

#include <sstream>
#include <iostream>
#include <functional>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
//KUtils related
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/perf_event.h>
#include <time.h>
#include <asm/unistd.h>
#include <chrono>

//#define CLOCKS_PER_SECOND 1000000

//#define DEBUG_BUILD
#ifdef  DEBUG_BUILD
#define DEBUGC(fmt, ...) do{ fprintf( stdout, fmt, __VA_ARGS__ ); } while( false )
#define DEBUG(x) do { std::cout << x; } while (0)
#else
#define DEBUGC(fmt, ...) do {} while (0)
#define DEBUG(x) do {} while (0)
#endif

using namespace std;
using namespace chrono;
using ns = chrono::nanoseconds;
using ms = chrono::milliseconds;
using get_time = chrono::steady_clock;

#define HAVE_LIBANGEL
#if defined (ANGEL) && defined (HAVE_LIBANGEL)
#include "./disk/angel-utils/libangel/include/angel.h"

// pass the <control> as "angel-control <control>" here as an argument
static void make_angel_control_call(const char *control)
{
  uint32_t service = 0;
  uint32_t arg = 0;
  if (angel_find_ctrl_by_name(control, &service, &arg) != 0)
  {
    fprintf(stderr, "ERROR: Unable to locate control '%s'\n", control);
    exit(-1);
  }

  intptr_t _arg = (intptr_t)arg;

  if (angel_hypercall(service, (void*)_arg))
  {
    fprintf(stderr, "ERROR: angel_hypercall failed:'%s'\n", control);
  }
  return;
}
#endif /* ANGEL && HAVE_LIBANGEL */

//#define PERF_INSTRUMENT
#ifdef PERF_INSTRUMENT
#include <sys/ioctl.h>

enum enable_perf_events : uint8_t
{
 ENABLE_HW_INSTRS_PER = 1,
 ENABLE_HW_INSTRS_TOT = 2,
 ENABLE_HW_CYCLES_PER = 4,
 ENABLE_HW_CYCLES_TOT = 8,
 ENABLE_ALL_EVENTS = 255
};

static int fdI =  -1;
static int fdC =  -1;
static int fdTI = -1;
static int fdTC = -1;

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
               int cpu, int group_fd, unsigned long flags)
{
   int ret;

   ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
   return ret;
}

static void
perf_event_init(enable_perf_events flag)
{

  if( (fdC == -1) && (fdI == -1) &&
       (fdTC == -1) && (fdTI == -1) )
  {
    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    if(flag & ENABLE_HW_INSTRS_PER)
    {
      pe.config = PERF_COUNT_HW_INSTRUCTIONS;
      fdI = perf_event_open(&pe, 0, -1, -1, 0);
      if (fdI == -1)
      {
        fprintf(stderr, "Error opening PERF_COUNT_HW_INSTRUCTIONS %llx\n", pe.config);
        //exit(EXIT_FAILURE);
      }
      else
      {
        printf("Successfully opened the PERF_COUNT_HW_INSTRUCTIONS \n");
      }
    }

    if(flag & ENABLE_HW_CYCLES_PER)
    {
      pe.config = PERF_COUNT_HW_CPU_CYCLES;
      fdC = perf_event_open(&pe, 0, -1, -1, 0);
      if (fdC == -1)
      {
         fprintf(stderr, "Error opening PERF_COUNT_HW_CPU_CYCLES %llx\n", pe.config);
         //exit(EXIT_FAILURE);
      }
      else
      {
        printf("Successfully opened the PERF_COUNT_HW_CPU_CYCLES \n");
      }
    }

    if(flag & ENABLE_HW_INSTRS_TOT)
    {
      pe.config = PERF_COUNT_HW_INSTRUCTIONS;
      fdTI = perf_event_open(&pe, 0, -1, -1, 0);
      if (fdTI == -1)
      {
        fprintf(stderr, "Error opening PERF_COUNT_HW_INSTRUCTIONS total %llx\n", pe.config);
        //exit(EXIT_FAILURE);
      }
      else
      {
        printf("Successfully opened the PERF_COUNT_HW_INSTRUCTIONS total \n");
      }
    }

    if(flag & ENABLE_HW_CYCLES_TOT)
    {
      pe.config = PERF_COUNT_HW_CPU_CYCLES;
      fdTC = perf_event_open(&pe, 0, -1, -1, 0);
      if (fdTC == -1)
      {
         fprintf(stderr, "Error opening PERF_COUNT_HW_CPU_CYCLES total %llx\n", pe.config);
         //exit(EXIT_FAILURE);
      }
      else
      {
        printf("Successfully opened the PERF_COUNT_HW_CPU_CYCLES total \n");
      }
    }

  }
}

static void
perf_event_enable(enable_perf_events flag)
{
   //printf("Called perf_event_enable \n");
   if(flag & ENABLE_HW_INSTRS_PER)
   {
     ioctl(fdI, PERF_EVENT_IOC_RESET, 0);
     ioctl(fdI, PERF_EVENT_IOC_ENABLE, 0);
   }

   if(flag & ENABLE_HW_CYCLES_PER)
   {
     ioctl(fdC, PERF_EVENT_IOC_RESET, 0);
     ioctl(fdC, PERF_EVENT_IOC_ENABLE, 0);
   }

   if(flag & ENABLE_HW_INSTRS_TOT)
   {
     ioctl(fdTI, PERF_EVENT_IOC_RESET, 0);
     ioctl(fdTI, PERF_EVENT_IOC_ENABLE, 0);
   }

   if(flag & ENABLE_HW_CYCLES_TOT)
   {
     ioctl(fdTC, PERF_EVENT_IOC_RESET, 0);
     ioctl(fdTC, PERF_EVENT_IOC_ENABLE, 0);
   }
}

inline static uint64_t
perf_per_cycle_event_read()
{
   //printf("Called perf_event_read \n");
   uint64_t count;

   //ioctl(fdC, PERF_EVENT_IOC_DISABLE, 0);

   int result = read(fdC, &count, sizeof(uint64_t));

   //printf("PERF_COUNT_HW_CPU_CYCLES cycles: %lld cycles\n", count);
   //ioctl(fdC, PERF_EVENT_IOC_RESET, 0);

   return (count);
}

inline static uint64_t
perf_per_instr_event_read()
{
   //printf("Called perf_event_read \n");
   uint64_t count;

   //ioctl(fdI, PERF_EVENT_IOC_DISABLE, 0);

   int result = read(fdI, &count, sizeof(uint64_t));

   //printf("PERF_COUNT_HW_CPU_INSTRS instrs: %lld cycles\n", count);
   //ioctl(fdI, PERF_EVENT_IOC_RESET, 0);

   return (count);
}

inline static uint64_t
perf_tot_cycle_event_read()
{
   //printf("Called perf_event_read \n");
   uint64_t count;

   //ioctl(fdTC, PERF_EVENT_IOC_DISABLE, 0);

   int result = read(fdTC, &count, sizeof(uint64_t));

   //printf("PERF_COUNT_HW_CPU_CYCLES cycles: %lld cycles\n", count);
   //ioctl(fdTC, PERF_EVENT_IOC_RESET, 0);

   return (count);
}

inline static uint64_t
perf_tot_instr_event_read()
{
   //printf("Called perf_event_read \n");
   uint64_t count;

   //ioctl(fdTI, PERF_EVENT_IOC_DISABLE, 0);

   int result = read(fdTI, &count, sizeof(uint64_t));

   //printf("PERF_COUNT_HW_CPU_INSTRS instrs: %lld cycles\n", count);
   //ioctl(fdTI, PERF_EVENT_IOC_RESET, 0);

   return (count);
}
#endif //PERF_INSTRUMENT

#endif //KUtils_H
