#ifndef GENERATOR_H
#define GENERATOR_H

#include "tasks.h"

// =================
// MACRO DEFINITIONS
// =================

// Task period bounds
#define MIN_PERIOD 10000
#define MAX_PERIOD 1000000
#define GRANULARITY 1

// Task deadline = DEADLINE_FACTOR * task period
#define MIN_DEADLINE_FACTOR 0.25
#define MAX_DEADLINE_FACTOR 4.0

// Task wcet(HI) = CRITICALITY_FACTOR * Task wcet(LO)
// For hi-criticality tasks CRITICALITY_FACTOR_HI >= 1.0
// For lo-criticality tasks CRITICALITY_FACTOR_LO <= 1.0
#define MIN_CRITICALITY_FACTOR_LO 0.25
#define MAX_CRITICALITY_FACTOR_LO 1.00
#define MIN_CRITICALITY_FACTOR_HI 1.00
#define MAX_CRITICALITY_FACTOR_HI 4.00

// =====================
// FUNCTION DECLARATIONS
// =====================

// Assign task criticalities HI for all tasks with a given probability (criticality proportion)
void assign_task_criticalities (Tasks *tasks, int num_tasks, double criticality_proportion);

// Task utilizations (Ui = Ci / Ti) are generated using UUnifast [1] providing an unbiased distribution
void generate_task_utilizations (Tasks *tasks, int num_tasks, double u_bar, int criticality);

// Task periods Ti were generated according to a log-uniform distribution [2]
void generate_task_periods (Tasks *tasks, int num_tasks);

// Task deadlines are generated according to a log-uniform distribution [2] in the range [0.25, 4.0]Ti
void generate_task_deadlines (Tasks *tasks, int num_tasks);

// The execution time of each task is given by Ci(X) = Ui(X) · Ti
void calculate_task_wcets (Tasks *tasks, int num_tasks) ;

// Task parameter generator driver function
Tasks* task_parameter_generator (Tasks *tasks, int num_tasks, double total_util, double criticality_proportion);

// Fetch task parameters as input from file and stores it in the task structure array
void fetch_task_parameters ( FILE* input_file, Tasks *tasks, int num_tasks);

#endif
