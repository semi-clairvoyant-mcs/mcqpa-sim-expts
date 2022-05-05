#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "../inc/generator.h"

// Assign task criticalities HI for all tasks with a given probability (criticality proportion)

void assign_task_criticalities (Tasks *tasks, int num_tasks, double criticality_proportion) {

    double random_number = 0.0;    // Random number between 0.0 and 1.0 (inclusive)

    // Precondition check
    assert(tasks != NULL && num_tasks > 0 && criticality_proportion >= 0.00 && criticality_proportion <= 1.00);

    // For all tasks
    for (int i = 0; i < num_tasks; i++) {

        // If criticality proportion = 1.0, all tasks must be HI criticality
        if (criticality_proportion == 1.00)
            tasks[i].criticality = HI;

        // Else if criticality proportion = 0.0, all tasks must be LO criticality
        else if (criticality_proportion == 0.00)
            tasks[i].criticality = LO;

        // Else if 0.0 < criticality proportion < 1.0
        else {

           // Generate a uniformly distributed random number between 0 and 1 (inclusive)
           random_number = rand() / (RAND_MAX * 1.0);

            // If the random number generated is less thank or equal to the given criticality proportion,
            // assign HI criticality
            if (random_number <= criticality_proportion)
                tasks[i].criticality = HI;

            // Else, assign LO criticality
            else 
                tasks[i].criticality = LO;
        }
    }
}

// Generate task utilizations (Ui = Ci / Ti) using the UUnifast algorithm [1] -- provides an unbiased utilization distribution

void generate_task_utilizations (Tasks *tasks, int num_tasks, double u_bar, int criticality) {

    double util_sum = u_bar;       // Desired sum of all tasks in the taskset
    double next_util_sum = 0.0;    // Sum of remaining tasks in the taskset

    // Precondition check
    assert(tasks != NULL && num_tasks > 0 && u_bar > 0.00 && u_bar < 1.00);

    // For the first (n - 1) tasks
    for (int i = 0; i < (num_tasks - 1); i++) {

        // Generate a random value for sum of utilizations of the remaining (n - i) tasks
        next_util_sum = util_sum * pow((rand() / (RAND_MAX * 1.0)), (1.0 / (num_tasks - i)));

        // Determine the utilization of i-th task --> sum of (n - i) task utilizations - sum of (n - i - 1) utilizations
        tasks[i].utilization[criticality] = util_sum - next_util_sum;

        // Set the sum of utilizations for remaining (n - i - 1) tasks
        util_sum = next_util_sum;

        // Postcondition check
        assert(next_util_sum >= 0.0 && next_util_sum <= u_bar && util_sum >= 0.0 && util_sum <= u_bar);
    }

    // For the n-th task, utilization = remaining sum i.e. the sum of utilization(s) of the last remaining task
    tasks[num_tasks - 1].utilization[criticality] = util_sum;

    // Postcondition check
    assert(next_util_sum >= 0.0 && next_util_sum <= u_bar && util_sum >= 0.0 && util_sum <= u_bar);
}

// Generate task periods Ti according as per log-uniform distribution [2]

void generate_task_periods (Tasks *tasks, int num_tasks) {

    double random_number = 0.0;    // Random number ~ U (log(Tmin), log(Tmax + Tg))

    // Precondition check
    assert(tasks != NULL && num_tasks > 0);

    // For all tasks
    for (int i = 0; i < num_tasks; i++) {

        // Generate a random number such that the generated value ~ U (log(Tmin), log(Tmax + Tg))
        random_number = fmod(rand() / (RAND_MAX * 1.0), log(MAX_PERIOD + GRANULARITY) - log(MIN_PERIOD)) + log(MIN_PERIOD);

        // Set task period to floor(exp(random number) / GRANULARITY) * GRANULARITY [2]
        tasks[i].period = (int)(floor(exp(random_number) / (1.0 * GRANULARITY)) * GRANULARITY);

        // Postcondition check
        assert((tasks[i].period >= MIN_PERIOD) && (tasks[i].period <= (MAX_PERIOD + GRANULARITY)));
    }
}

// Determine the execution times of each task --> Ci(LO) = Ui(LO) * Ti, Ci(HI) = Criticality factor * Ci(LO)

void calculate_task_wcets (Tasks *tasks, int num_tasks) {

    double lo_criticality_factor = 0.0;    // Multiplying factor to obtain wcet(HI) of lo-criticality tasks from their corresponding wcet(LO) values
    double hi_criticality_factor = 0.0;    // Multiplying factor to obtain wcet(HI) of hi-criticality tasks from their corresponding wcet(LO) values

    // Precondition check
    assert(tasks != NULL && num_tasks > 0);

    // For all tasks
    for (int i = 0; i < num_tasks; i++) {

        // Ci(LO) = Ui(LO) * Ti
        tasks[i].wcet[LO] = tasks[i].utilization[LO] * tasks[i].period;
        assert(tasks[i].wcet[LO] >= 0.0);

        // If task criticality is LO
        if (tasks[i].criticality == LO) {

            // Generate a random LO criticality factor in the range [MIN_CRITICALITY_FACTOR_LO, MAX_CRITICALITY_FACTOR_LO]
            lo_criticality_factor = fmod(rand() / (RAND_MAX * 1.0), MAX_CRITICALITY_FACTOR_LO - MIN_CRITICALITY_FACTOR_LO) + MIN_CRITICALITY_FACTOR_LO;

            // Ci(HI) = Criticality factor LO * Ci(LO)
            tasks[i].wcet[HI] = lo_criticality_factor * tasks[i].wcet[LO];

            // Postcondition check
            assert(lo_criticality_factor >= MIN_CRITICALITY_FACTOR_LO && lo_criticality_factor <= MAX_CRITICALITY_FACTOR_LO && tasks[i].wcet[HI] >= 0.0 && tasks[i].wcet[HI] <= tasks[i].wcet[LO]);
        }

        // Else, if criticality factor is HI
        else {

            // Generate a random HI criticality factor in the range [MIN_CRITICALITY_FACTOR_HI, MAX_CRITICALITY_FACTOR_HI]
            hi_criticality_factor = fmod(rand() / (RAND_MAX * 1.0), MAX_CRITICALITY_FACTOR_HI - MIN_CRITICALITY_FACTOR_HI) + MIN_CRITICALITY_FACTOR_HI;

            // Ci(HI) = Criticality factor HI * Ci(LO)
            tasks[i].wcet[HI] = hi_criticality_factor * tasks[i].wcet[LO];

            // Postcondition check
            assert(hi_criticality_factor >= MIN_CRITICALITY_FACTOR_HI && hi_criticality_factor <= MAX_CRITICALITY_FACTOR_HI && tasks[i].wcet[HI] >= 0.0 && tasks[i].wcet[HI] >= tasks[i].wcet[LO]);
        }

        // Storing HI criticality task utilizations in the task structure array
        tasks[i].utilization[HI] = tasks[i].wcet[HI] / tasks[i].period;
    }
}

// Generates task deadlines Di according to log uniform distribution in the range [MIN_DEADLINE_FACTOR, MAX_DEADLINE_FACTOR]Ti

void generate_task_deadlines (Tasks *tasks, int num_tasks) {

    double random_number = 0.0;    // Multiplying factor to obtain task deadlines from their corresponding task period values

    // Precondition check
    assert(tasks != NULL && num_tasks > 0);

    // For all tasks
    for (int i = 0; i < num_tasks; i++) {

        // Generate a random deadline factor in the range [MIN_DEADLINE_FACTOR, MAX_DEADLINE_FACTOR]
        random_number = fmod(rand() / (RAND_MAX * 1.0), log((MAX_DEADLINE_FACTOR * tasks[i].period) + GRANULARITY) - log(MIN_DEADLINE_FACTOR * tasks[i].period)) + log(MIN_DEADLINE_FACTOR * tasks[i].period);

        // Set task deadline to floor(exp(random number) / GRANULARITY) * GRANULARITY [2]
        tasks[i].deadline = (int)(floor(exp(random_number) / (1.0 * GRANULARITY)) * GRANULARITY);

        // Postcondition check
        assert(tasks[i].deadline >= MIN_DEADLINE_FACTOR * tasks[i].period - 1 && tasks[i].deadline <= MAX_DEADLINE_FACTOR * tasks[i].period);
    }
}

// Task parameter generator

Tasks* task_parameter_generator (Tasks *tasks, int num_tasks, double total_utilization, double criticality_proportion) {

    // Precondition check
    assert(tasks != NULL && num_tasks > 0 && total_utilization >= 0.0 && total_utilization < 1.0 && criticality_proportion >= 0.0 && criticality_proportion < 1.010);

    // Assign a task number to each task structure in the array
    for (int i = 0; i < num_tasks; i++)
        tasks[i].task_no = i + 1;

    // Generate individual task parameters
    assign_task_criticalities(tasks, num_tasks, criticality_proportion);    // Assign task criticalities
    generate_task_utilizations(tasks, num_tasks, total_utilization, LO);    // Generate lo-criticality task utilizations
    generate_task_periods(tasks, num_tasks);                                // Generate task periods
    calculate_task_wcets(tasks, num_tasks);                                 // Determine task wcets
    generate_task_deadlines(tasks, num_tasks);                              // Generate task deadlines

    // Return pointer to tasks struct array
    return tasks;
}

// Fetch task parameters as input from file and stores it in the task structure array

void fetch_task_parameters (FILE* input_file, Tasks *tasks, int num_tasks) {

    // Precondition check
    assert(input_file != NULL && tasks != NULL && num_tasks > 0);

    // For all tasks
    for (int i = 0 ; i < num_tasks ; i++) {

        // Assign a task number
        tasks[i].task_no = i + 1;

        // Record the task's phase, period, relative deadline, criticality and wcet values from the file
        fscanf(input_file,"%d\t%d\t%d\t%lf\t%lf", &tasks[i].period, &tasks[i].deadline, &tasks[i].criticality, &tasks[i].wcet[LO], &tasks[i].wcet[HI]);

        // Calculate task utilizations at each criticality level
        tasks[i].utilization[LO] = tasks[i].wcet[LO] / tasks[i].period;
        tasks[i].utilization[HI] = tasks[i].wcet[HI] / tasks[i].period;
    }
}
