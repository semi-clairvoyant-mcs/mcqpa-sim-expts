#ifndef SCHEDTEST_H
#define SCHEDTEST_H

#include "tasks.h"

// =================
// MACRO DEFINITIONS
// =================

// Return values of schedulability test function
#define DEFAULT -1
#define SCHEDULABLE 1
#define NOT_SCHEDULABLE 0

// =============================
// ABSTRACT DATATYPE DEFINITIONS
// =============================

// Schedulability information structure
typedef struct {
    int taskset_schedulability;          // Boolean variable set to SCHEDULABLE (i.e. 1) if taskset is schedulable, else NOT_SCHEDULABLE (i.e. 0)
    long long int t_points;              // Total number of t points for which the demand bound function is calculated
    long long int t_s_points;            // Total number of (t, s) points for which the demand bound function is calculated
    long double interval_length;         // Total interval length that is considered for demand bound function analysis of the given taskset
    int failed_case1;                    // Boolean variable set to 1 if the schedulability test failure happens at t < 0.5B, else 0
    int failed_case2;                    // Boolean variable set to 1 if the schedulability test failure happens at t < 0.25B, else 0
} Schedulability;

// =====================
// FUNCTION DECLARATIONS
// =====================

// Iterative: Exact EDF schedulability test for 3-parameter sporadic task systems [3] --> Listed as Algorithm 1 in the paper
Schedulability* schedulability_test (Tasks *tasks, int num_tasks, Schedulability *schedulability);

// MC-QPA (based on QPA [4]): A more efficient exact EDF schedulability test for 3-parameter sporadic task systems --> Listed as Algorithm 2 in the paper
Schedulability* schedulability_test_mcqpa (Tasks *tasks, int num_tasks, Schedulability *schedulability);

#endif
