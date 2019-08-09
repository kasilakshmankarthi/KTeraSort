#ifndef KTeraSort_H
#define KTeraSort_H

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <vector>
#include <iterator>
#include <time.h>
#include <array>
#include <memory>
#include <type_traits>
#include <iomanip>
#include "KSortRnd.h"
#include "version.h"

//#define KTERASORT_VALIDATION
//#define KTERASORT_DEBUG

#ifdef KTERASORT_DEBUG
#define DEBUG_BUILD
#endif

#include "KUtils.h"

//Two level Trie is implemented and used for partitioning
//First 2 bytes of key is used for partioning
#define PREFIX_LENGTH (2)

using namespace std;
using byte = unsigned char;

template <class T, size_t ROW, size_t COL>
using Matrix = std::array<std::array<T, COL>, ROW>;

#endif

