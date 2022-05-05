#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../inc/generator.h"
#include "../inc/auxiliary.h"
#include "../inc/schedtest.h"

int main(int argc, char* argv[]) {

    // Time variables
    clock_t t1, t2;                                // To store the number of clock ticks elapsed
    double time_taken_1 = 0.0;                     // Records the time taken by the EDF schedulability test with QPA optimization
    double time_taken_2 = 0.0;                     // Records the time taken by the EDF schedulability test without QPA optimization

    // Taskset generation variables
    Tasks *tasks;                                  // Pointer to task structure array
    int num_tasks = 0;                             // Number of tasks in the taskset (file input)
    int num_tasksets = 0;                          // Number of tasksets generated / read

    // Schedulability result structs
    Schedulability *schedulability;                // Schedulability struct for iterative test
    Schedulability *schedulability_mcqpa;          // Schedulability struct for MC-QPA

    // Output print variables
    long long int num_t_s, num_t_s_mcqpa = 0;      // Total number of (t, s) points verified for NUM_TASKSETS tasksets
    long long int num_t, num_t_mcqpa = 0;          // Total number of t points verified for NUM_TASKSETS tasksets
    long double av_t = 0;                          // Average interval length over which the schedulability is ensured for NUM_TASKSETS tasksets
    int num_failed_tasksets = 0;                   // Total number of tasksets that fail the schedulability test
    int failed_case1 = 0;                          // Number of tasksets that fail the schedulability test at t < (upper bound) / 2
    int failed_case2 = 0;                          // Number of tasksets that fail the schedulability test at t < (upper bound) / 4

    // Weighted schedulability variables
    double taskset_util = 0.0;                     // Taskset utilization
    double sum_taskset_utils = 0.0;                // Sum of all taskset utilizations
    double sum_schedulable_taskset_utils = 0.0;    // Sum of all SCHEDULABLE taskset utilizations
    double weighted_schedulability = 0.0;          // Weighted Schedulability = (Sum of all SCHEDULABLE taskset utilizations) / (Sum of all taskset utilizations)

    // Precondition check
    assert((argc == 2) && (*argv[1] == 'f' || *argv[1] == 'F' || *argv[1] == 't' || *argv[1] == 'T') && "Program expects the following two arguments: <executable name> <input method> (see README for further details)");

    // Open output file
    FILE *output_fptr;
    output_fptr = fopen("output.csv","w+");
    assert(output_fptr != NULL && "Error opening the output file \"output.csv\"");

    schedulability = malloc(sizeof(Schedulability));
    schedulability_mcqpa = malloc(sizeof(Schedulability));

    // Simulate tests for custom inputs via files
    if (*argv[1] == 'f' || *argv[1] == 'F') {

        // Open input file
        FILE *input_fptr;
        input_fptr = fopen("input.txt","r");
        assert(input_fptr != NULL && "Error opening the input file \"input.txt\"");

        // Read number of tasksets 
        fscanf(input_fptr,"%d\n", &num_tasksets);

        // Print output file headings
        fprintf(output_fptr, ",Schedulability,Iterative test time,MC-QPA test time,Iterative interval lengths,MC-QPA interval lengths,Iterative pair-points,MC-QPA pair-points\n");

        // For each taskset
        for (int i = 0; i < num_tasksets; i++) {

            // Fetch task parameters
            fscanf(input_fptr,"%d\n", &num_tasks);
            tasks = malloc(num_tasks * sizeof(Tasks));
            fetch_task_parameters(input_fptr, tasks, num_tasks);

            // Run Iterative test (Algorithm 1)
            t1 = clock();
            schedulability_test(tasks, num_tasks, schedulability);
            t1 = clock() - t1;
            time_taken_1 += ((double)t1) / CLOCKS_PER_SEC;

            // Run MC-QPA test (Algorithm 2)
            t2 = clock();
            schedulability_test_mcqpa(tasks, num_tasks, schedulability_mcqpa);
            t2 = clock() - t2;
            time_taken_2 += ((double)t2) / CLOCKS_PER_SEC;

            // Experimentally confirms the correctness of MC-QPA
            assert(schedulability->taskset_schedulability == schedulability_mcqpa->taskset_schedulability);

            // Print outputs
            printf(" For taskset %d\t Schedulability: %d\t Iterative test time: %lf\t MC-QPA test time: %lf\t Iterative interval lengths: %lld\t MC-QPA interval lengths: %lld\t Iterative pair-points:%lld\t MC-QPA pair-points: %lld\n", i + 1, schedulability->taskset_schedulability, time_taken_1, time_taken_2, schedulability->t_points, schedulability_mcqpa->t_points, schedulability->t_s_points, schedulability_mcqpa->t_s_points);
            fprintf(output_fptr, "Taskset %d,%d,%lf,%lf,%lld,%lld,%lld,%lld\n", i + 1, schedulability->taskset_schedulability, time_taken_1, time_taken_2, schedulability->t_points, schedulability_mcqpa->t_points, schedulability->t_s_points, schedulability_mcqpa->t_s_points);

            // Get the taskset utilization
            taskset_util = get_taskset_utilization(tasks, num_tasks);

            // Update sum of all taskset utilizations
            sum_taskset_utils += taskset_util;

            // Update sum of all SCHEDULABLE taskset utilizations
            if (schedulability_mcqpa->taskset_schedulability == SCHEDULABLE)
                sum_schedulable_taskset_utils += taskset_util;

            // Free allocated memory
            free(tasks);
        }

        // Calculate weighted schedulability
        if (sum_taskset_utils != 0.0)
            weighted_schedulability = sum_schedulable_taskset_utils / sum_taskset_utils;
        else
            weighted_schedulability = 0.0;

        // Print outputs
        fprintf(output_fptr,"\n Weighted Schedulability: %lf\n", weighted_schedulability);
        printf("\n Weighted schedulability: %lf\n", weighted_schedulability);

        // Close the input file
        fclose(input_fptr);
    }

    // Simulate tests on tasksets generated using the task parameter generator function
    else if (*argv[1] == 't' || *argv[1] == 'T') {

        // For criticality proportions 0.25 to 0.75 (step: 0.25)
        for (double criticality_proportion = 0.25; criticality_proportion <= 0.99; criticality_proportion += 0.25) {

            srand(time(0));

            // Print output file headings
            fprintf(output_fptr,"\nCriticality Proportion: %lf\n", criticality_proportion);
            fprintf(output_fptr, "Taskset Utilization,Weighted Schedulability,Iterative test time,MC-QPA test time,Av T upper bound,Iterative interval lengths,MC-QPA interval lengths,Iterative pair-points,MC-QPA pair-points,Number of tasksets failed,Failed before t < 0.5B,Failed before t < 0.25B\n");

            // Print heading (terminal)
            printf("\n Criticality Proportion: %lf\n", criticality_proportion);

            // For taskset utilizations 0.10 to 0.90 (step: 0.10)
            for (double taskset_utilization = 0.10; taskset_utilization <= 0.95; taskset_utilization += 0.10) {

                // Initializing weighted schedulability and runtime recording variables for each run
                sum_taskset_utils = 0.0;
                sum_schedulable_taskset_utils = 0.0;
                time_taken_1 = 0.0;
                time_taken_2 = 0.0;
                av_t = 0.0;
                num_t_s = 0;
                num_t = 0;
                num_t_s_mcqpa = 0;
                num_t_mcqpa = 0;
                num_failed_tasksets = 0;
                failed_case1 = 0;
                failed_case2 = 0;
                num_tasksets = 0;

                while (num_tasksets < NUM_TASKSETS) {

                    // Generate input tasksets
                    tasks = malloc(NUM_TASKS * sizeof(Tasks));
                    tasks = task_parameter_generator(tasks, NUM_TASKS, taskset_utilization, criticality_proportion);
                    num_tasksets++;

                    // Run Iterative test (Algorithm 1)
                    t1 = clock();
                    schedulability_test(tasks, NUM_TASKS, schedulability);
                    t1 = clock() - t1;
                    time_taken_1 += ((double)t1) / CLOCKS_PER_SEC;

                    // Run MC-QPA test (Algorithm 2)
                    t2 = clock();
                    schedulability_test_mcqpa(tasks, NUM_TASKS, schedulability_mcqpa);
                    t2 = clock() - t2;
                    time_taken_2 += ((double)t2) / CLOCKS_PER_SEC;

                    // Experimentally confirms the correctness of MC-QPA
                    assert(schedulability->taskset_schedulability == schedulability_mcqpa->taskset_schedulability);

                    // Get the taskset utilization
                    taskset_util = get_taskset_utilization(tasks, NUM_TASKS);

                    // Update sum of all taskset utilizations
                    sum_taskset_utils += taskset_util;

                    // If the taskset is SCHEDULABLE, update sum of all SCHEDULABLE taskset utilizations
                    if (schedulability_mcqpa->taskset_schedulability == SCHEDULABLE)
                        sum_schedulable_taskset_utils += taskset_util;
                        
                    // Else if, the taskset is NOT SCHEDULABLE, update failed tasksets info
                    else {
                        num_failed_tasksets++;
                        failed_case1 += schedulability_mcqpa->failed_case1;
                        failed_case2 += schedulability_mcqpa->failed_case2;
                    }

                    // Update schedulability results
                    num_t_s += schedulability->t_s_points;
                    num_t += schedulability->t_points;
                    num_t_s_mcqpa += schedulability_mcqpa->t_s_points;
                    num_t_mcqpa += schedulability_mcqpa->t_points;
                    av_t = (schedulability_mcqpa->interval_length + (av_t * (num_tasksets - 1))) / (long double)num_tasksets;
                
                    // Free allocated memory
                    free(tasks);
                }

                // Calculate weighted schedulability
                if (sum_taskset_utils != 0.0)
                    weighted_schedulability = sum_schedulable_taskset_utils / sum_taskset_utils;
                else
                    weighted_schedulability = 0.0;

                // Print weighted schedulability info
                printf(" Taskset Utilization: %lf\t Weighted schedulability: %lf\t Iterative test time: %lf\t MC-QPA test time: %lf\t Tasksets failed: %d (%d before 0.5B, %d before 0.25B)\n", taskset_utilization, weighted_schedulability, time_taken_1, time_taken_2, num_failed_tasksets, failed_case1, failed_case2);
                printf(" Av B: %Lf\t Iterative interval lengths: %lld\t MC-QPA interval lengths: %lld\t Iterative pair-points:%lld\t MC-QPA pair-points: %lld\n\n", av_t, num_t, num_t_mcqpa, num_t_s, num_t_s_mcqpa);
                fprintf(output_fptr, "%lf,%lf,%lf,%lf,%Lf,%lld,%lld,%lld,%lld,%d,%d,%d\n", taskset_utilization, weighted_schedulability, time_taken_1, time_taken_2, av_t, num_t, num_t_mcqpa, num_t_s, num_t_s_mcqpa, num_failed_tasksets, failed_case1, failed_case2);
            }
        }
    }

    // Free allocated memory
    free(schedulability);
    free(schedulability_mcqpa);

    // Close the output file
    fclose(output_fptr);

    return 0;
}
