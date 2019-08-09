#ifndef KRandomGenLFSR_H
#define KRandomGenLFSR_H

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

using namespace std;

uint32_t randLFSR(uint32_t &lfsr)
{
  uint32_t bit;
  /* taps: 32 30 26 25; feedback polynomial: x^32 + x^30 + x^26 + x^25 + 1 */
  bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 6) ^ (lfsr >> 7) ) & 1;
  return lfsr =  (lfsr >> 1) | (bit << 31);
}

#endif
