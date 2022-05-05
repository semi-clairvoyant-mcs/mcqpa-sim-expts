#ifndef TASKS_H
#define TASKS_H

// =================
// MACRO DEFINITIONS
// =================

// Number of tasks and tasksets
#define NUM_TASKSETS 200
#define NUM_TASKS 20

// Utilization and wcet arrays indices
#define LO 0
#define HI 1

// =============================
// ABSTRACT DATATYPE DEFINITIONS
// =============================

// Task structure
typedef struct { 
    int task_no;                         // To identify a given task structure
    int period;                          // Minimum inter-arrival time between any two successive jobs of this task
    int criticality;                     // Criticality level defined for the task
    int deadline;                        // Relative deadline 
    double wcet[2];                      // Worst-case execution times of the task at each criticality level [dual-criticality system]
    double utilization[2];               // Task utilizations at each criticality level [dual-criticality system]
    int num_jobs;                        // Number of jobs in floor_upper_bound_t interval
} Tasks;

#endif
