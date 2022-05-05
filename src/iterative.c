#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/auxiliary.h"
#include "../inc/schedtest.h"

// Iterative: Exact EDF schedulability test for 3-parameter sporadic task systems [3] --> Listed as Algorithm 1 in the paper

Schedulability* schedulability_test (Tasks *tasks, int num_tasks, Schedulability *schedulability) {

    double lo_util_sum = 0.0;                    // Sum of LO criticality utilizations of all tasks U_LO
    double hi_util_sum = 0.0;                    // Sum of HI criticality utilizations of all tasks U_HI
    double max_util_sum = 0.0;                   // Max {U_HI, U_LO}
    double wcet_sum = 0.0;                       // Sum of wcets of all tasks at their own criticality level
    double t_upper_bound = 0.0;                  // [a.k.a. B] Upper bound for t that we need to consider when using DBFi(t, s) for a schedulability test
    long long int floor_t_upper_bound = 0;       // Floor of t upper bound
    int num_jobs_total = 0;                      // Total number of jobs arriving and having deadlines within floor_t_upper_bound
    long long int *s_points;                     // [a.k.a. S(t)] s values for which we need to calculate DBFi(t, s), s --> {t - kTi - Di} U {t}
    int num_s_points = 0;                        // The number of s values in S(t)
    int s_idx = 0;                               // Index to traverse through the s points array
    long long int t = 0;                         // t point at which DBF needs to be calculated
    double dbf_i = 0.0;                          // DBFi(t, s) of i-th task for given (t, s) values
    double processor_demand = 0.0;               // Processor demand of the task system --> defined as sum of DBFi(t, s) over all tasks for given (t, s) values

    // Initializing schedulability struct variables 
    schedulability->taskset_schedulability = DEFAULT;
    schedulability->t_s_points = 0;
    schedulability->t_points = 0;

    // Check the first EDF schedulability condition: Max{U_HI, U_LO} < 1.0 for the given taskset

    // Calculate U_HI, U_LO 
    for (int i = 0; i < num_tasks; i++) {
        lo_util_sum += tasks[i].utilization[LO];
        hi_util_sum += tasks[i].utilization[HI];
    }
    
    // Get max{U_HI, U_LO}
    max_util_sum = max(hi_util_sum, lo_util_sum);

    // If the first EDF schedulability condition is satisfied
    if (max_util_sum < 1.0) {

        // Calculate wcet sum of all tasks at their own criticality levels
        for (int i = 0; i < num_tasks; i++)
            wcet_sum += tasks[i].wcet[(tasks[i].criticality)];

        // Calculate B (Upper bound for t that we need to consider for demand bound analysis)
        t_upper_bound = wcet_sum / (1.0 - max_util_sum);

        // Calculate floor B (DBF analysis is performed at integer points)
        floor_t_upper_bound = (long long int)(floor(t_upper_bound));
        schedulability->interval_length = floor_t_upper_bound;

        // Calculate number of jobs in floor_t_upper_bound for each task
        for (int i = 0; i < num_tasks; i++) {
            tasks[i].num_jobs = get_num_jobs(floor_t_upper_bound, tasks[i].deadline, tasks[i].period);
            num_jobs_total += tasks[i].num_jobs;
        }

        // Allocate memory for s points array (max s points = num_jobs_total + 1)
        s_points = malloc((num_jobs_total + 1) * sizeof(long long int));

        // DBF analysis as proposed in [3] 

        // For all t values
        for (t = 1; t <= floor_t_upper_bound; t++) {

            // Initialize all array elements
            for (int i = 0; i < (num_jobs_total + 1); i++)
                s_points[i] = 0;

            // Get the s points (i.e. S(t)) that we need to consider for DBFi(t, s) analysis
            num_s_points = get_s_points(tasks, num_tasks, s_points, t);

            // Initialize s_points index to 0 for each t
            s_idx = 0;

            // Traverse through the S(t) array (s_points) while s <= t
            while (s_idx < num_s_points) {

                // For a given (t, s) value initialize  processor_demand
                 processor_demand = 0.0;

                // For all tasks
                for (int i = 0; i < num_tasks; i++) {

                    // For a given task initialize dbf_i
                    dbf_i = 0.0;

                    // If the given task is a LO criticality task
                    if (tasks[i].criticality == LO) {

                        // Calculate DBFi as per eqn 13b [3]
                        dbf_i = get_num_jobs(t, tasks[i].deadline, tasks[i].period) * tasks[i].wcet[HI] + min((int)(floor(s_points[s_idx] / (1.0 * tasks[i].period))) + 1, get_num_jobs(t, tasks[i].deadline, tasks[i].period)) * (tasks[i].wcet[LO] - tasks[i].wcet[HI]);
                    }

                    // If the given task is a HI criticality task
                    else {

                        // Calculate DBFi as per eqn 13a [3]
                        dbf_i = get_num_jobs(t, tasks[i].deadline, tasks[i].period) * tasks[i].wcet[LO] + get_num_jobs(t - s_points[s_idx], tasks[i].deadline, tasks[i].period) * (tasks[i].wcet[HI] - tasks[i].wcet[LO]);
                    }

                    // Update the processor demand
                     processor_demand =  processor_demand + dbf_i;
                }

                schedulability->t_s_points++;

                // If the DBF summation for any t and s value for all tasks exceeds t 
                // Taskset is NOT_SCHEDULABLE --> in violation of condition 2
                if ( processor_demand > t) {
                    schedulability->failed_case1 = 0;            
                    schedulability->failed_case2 = 0;
                    if (t <= (0.5 * floor_t_upper_bound))
                        schedulability->failed_case1 = 1;
                    if (t <= (0.25 * floor_t_upper_bound))
                        schedulability->failed_case2 = 1;
                    schedulability->taskset_schedulability = NOT_SCHEDULABLE;
                    break;
                }

                // Move to the next s value only if s < t, else break out of loop
                if (s_points[s_idx] == t)
                    break;

                s_idx++;
            }
            
            schedulability->t_points++;

            // If the DEFAULT retval was changed to NOT_SCHEDULABLE by some (t, s) -->  processor_demand > t --> max_ processor_demand > t
            // Break out of the loop, no condition check required for the rest of the t values
            if (schedulability->taskset_schedulability == NOT_SCHEDULABLE)
                break;
        }

        // If the condition was satisfied at all t, s and retval still holds DEFAULT value 
        // The taskset is SCHEDULABLE
        if (schedulability->taskset_schedulability == DEFAULT)
            schedulability->taskset_schedulability = SCHEDULABLE;

        // Free all the dynamically allocated memory
        free(s_points);

        // Return schedulability
        return schedulability;
    }

    // If the first EDF schedulability condition is NOT satisfied -- should not happen for filtered tasksets
    else {
        schedulability->interval_length = 0.0;
        schedulability->failed_case1 = 1;
        schedulability->failed_case2 = 1;
        schedulability->t_s_points = 0;
        schedulability->t_points = 0;
        schedulability->taskset_schedulability = NOT_SCHEDULABLE;
        return schedulability;
    }
}


