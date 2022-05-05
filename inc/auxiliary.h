#ifndef AUXILIARY_H
#define AUXILIARY_H

#include "tasks.h"

// =====================
// FUNCTION DECLARATIONS
// =====================

// Get the number of jobs arriving and having deadlines within the given interval
int get_num_jobs (long long int interval, int deadline, int period);

// Quick sort comparator function
int sort_comparator (const void * a, const void * b);

// Determine S(t) for t = floor(B), store all the s points in s_arr array. Return array size
int get_s_points (Tasks *tasks, int num_tasks, long long int *s_points, long long int floor_t_upper_bound);

// Utility function to remove duplicate array elements and return new array size
int sort_deduplicate_s_points(long long int *s_points, int num_s_points);

// Utility function to calculate total taskset utilization
double get_taskset_utilization (Tasks *tasks, int num_tasks);

// Utility function to find the minimum of two numbers
int min (int a, int b);

// Utility function to find the maximum of two numbers
double max (double a, double b);

#endif
