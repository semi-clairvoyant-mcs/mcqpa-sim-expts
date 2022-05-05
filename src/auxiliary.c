#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/tasks.h"

// Get the number of jobs arriving and having deadlines within the given interval

int get_num_jobs (long long int interval, int deadline, int period) {

    int num_jobs = 0;    // psi_i(t) [3]: number of jobs arriving and having deadlines within the given interval

    // Calculate the number of jobs arriving and having deadlines within the given interval
    num_jobs = (int)(floor((interval - deadline) / (1.0 * period))) + 1;

    // If number of jobs < 0, set number of jobs equal to 0
    if (num_jobs < 0)
        num_jobs = 0;

    return num_jobs;
}

// Quick sort comparator

int sort_comparator (const void * a, const void * b) {
   return ( *(long long int *)a - *(long long int *)b );
}

// Sorting and deduplicating the s points array

int sort_deduplicate_s_points (long long int *s_points, int num_s_points) {

    int j = 0;    // To store index of next unique element

    // Quick sort s points in increasing order
    qsort(s_points, num_s_points, sizeof(long long int), sort_comparator);

    // If array is empty/ contains only 1 element
    if (num_s_points == 0 || num_s_points == 1)
        return num_s_points;

    // For all array elements
    for (int i = 0; i < num_s_points - 1; i++) {

        // If current element is not equal to the next element
        // Store next element
        if (s_points[i] != s_points[i + 1]) {
            s_points[j] = s_points[i];
            j++;
        }
    }
    s_points[j] = s_points[num_s_points - 1];
    j++;

    return j;
}

// Determine S(t) for t = floor(B), where S(t) = Arrival times of all hi-criticality jobs U {t} 
// Store all the s points in s_points array; return array size

int get_s_points (Tasks *tasks, int num_tasks, long long int *s_points, long long int t) {

    int s_idx = 0;           // Index used to traverse through the s_points array
    int num_s_points = 0;    // s points array size
    int num_jobs = 0;
    
    // For all tasks
    for (int i = 0; i < num_tasks; i++) {

        num_jobs = get_num_jobs(t, tasks[i].deadline, tasks[i].period);

        // If the given task is HI criticality
        if (tasks[i].criticality == HI) {

            // For all jobs
            for (int k = 0; k < num_jobs; k++) {

                // Find arrival times for all HI-criticality jobs for the worst-case scenario
                s_points[s_idx] = t - ((long long int)k * (long long int)tasks[i].period) - tasks[i].deadline;

                // If arrival time value < 0, break out of the loop [reqd for t < deadline]
                if (s_points[s_idx] < 0) {
                    break;
                }

                s_idx++;
            }
        }
    }

    // Finally, floor(B) is also included in S(t)
    s_points[s_idx] = t;

    // The s-points array is now sorted in increasing order and deduplicated
    num_s_points = sort_deduplicate_s_points (s_points, s_idx + 1);

    // Return the s_points array size
    return num_s_points;
}

// Utility function to calculate total taskset utilization

double get_taskset_utilization (Tasks *tasks, int num_tasks) {
    double taskset_utilization = 0.0;

    for (int i = 0; i < num_tasks; i++)
        taskset_utilization += tasks[i].utilization[(tasks[i].criticality)];

    return taskset_utilization;
}

// Utility function to find the minimum of 2 numbers

int min (int a, int b) {
    if (a <= b)
        return a;
    else
        return b;
}

// Utility function to find the maximum of 2 numbers

double max (double a, double b) {
    if (a >= b)
        return a;
    else
        return b;
}
