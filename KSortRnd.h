#ifndef KSortRn_H
#define KSortRn_H

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
#include <inttypes.h>
#include "version.h"
#include "KUtils.h"

using namespace std;

#if 0
//A comparator function used by qsort supported by standard C
uint32_t compare (const void * a, const void * b)
{
    if( (*(uint32_t64_t*)a - *(uint32_t64_t*)b) < 0 )
        return -1;
    if( (*(uint32_t64_t*)a - *(uint32_t64_t*)b) > 0 )
        return 1;
    if( (*(uint32_t64_t*)a - *(uint32_t64_t*)b) == 0 )
        return 0;
}
#endif

/**
 * Generate a number between start and end
 */
uint32_t randomNumber(int64_t start, int64_t end)
{
    // Seed the randomiser
    srand( time(NULL) );

    return rand() % end + start;
}

/**
 * Perform KuickSort
 * Data provided will be modified
 */
template <typename Iterator> void quickSortRnd(Iterator start, Iterator end)
{
    //printf("Start=%p End=%p \n", dynamic_cast<void *>(start), dynamic_cast<void *>(end));
    int64_t size = (end - start);
    if (size <= 0)
    {
      return;// Base case
    }

    // Partition - in place partitioning that involves only swapping
    // https://class.coursera.org/algo/lecture/preview/22 - O(n) time with no extra memory++
    /*
        Assume pivot is first element of array
        Otherwise swap pivot with first element

        Idea: Array A is in this form
            pivot | < p | > p | unpartitioned/unseen

        Let i = index of the first item > p
        Let j = index of the first item unpartitioned

        Let i = 1
        Let j = 1
        Let p = pivot value

        for j < size
            if A[i] < p
                swap A[j] with A[i]
                i++
        swap A[0] with A[i-1]
    */
    // Get pivot element
    uint32_t pivotIndex = randomNumber(0, size);
    //cout<<"size: "<<size<<" pivotIndex: "<<pivotIndex<<endl;
    typename std::iterator_traits<Iterator>::value_type pivot = *(start + pivotIndex);

    if (pivotIndex != 0)
    {
        std::swap(*(start + pivotIndex), *start);
    }

    uint32_t i = 1;
    for (uint32_t j = 1; j <= size; j++)
    {
        if ( *(start + j) < pivot )
        {
            std::swap( *(start+j), *(start+i));
            i++;
        }
    }

    //printf("Swap with i=%u\n", i);
    std::swap(*start, *(start + i - 1));

    //printf("start=%p start_i_1=%p\n", start, (start+i-1));
    quickSortRn(start, start+i-1);
    //printf("start=%p start_i=%p end=%p\n", start, (start+i), end);
    quickSortRn(start+i, end);
}

template<class Iter>
void mergeSort(Iter first, Iter last)
{
    if (last - first > 1)
    {
        Iter middle = first + (last - first) / 2;
        mergeSort(first, middle); // [first, middle)
        mergeSort(middle, last);  // [middle, last)
        std::inplace_merge(first, middle, last);
    }
}
#endif
