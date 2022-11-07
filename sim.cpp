#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sim.h"
#include <vector>

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    params.bp_name  = argv[1];

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }


    uint32_t index;
    uint32_t mask;
    uint32_t  len = 2;

    if (strcmp(params.bp_name, "bimodal") == 0) {
        for (int i = 0; i < params.M2 - 1; i++) {
            len *= 2;
        }
    }



    std::vector<int> arr;
    if (strcmp(params.bp_name, "bimodal") == 0) {
        for (int i = 0; i < len; i++) {
            arr.push_back(2);
        }
    }

    int predictions = 0;
    int mispredictions = 0;

    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF) {
        if (strcmp(params.bp_name, "bimodal") == 0) {
            // get mask of length M2 and isolate index
            mask = (1 << params.M2) - 1;
            index = (addr >> (2)) & mask;


            outcome = str[0];
            /*
            if (outcome == 't')
                printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
            else if (outcome == 'n') printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
            */

            predictions++;

            if (*str == 't') {
                if (arr[index] < 2) mispredictions++;
                if (arr[index] != 3) arr[index]++;

            } else if (*str == 'n') {
                if (arr[index] >= 2) mispredictions++;
                if (arr[index] != 0) arr[index]--;
            }


        }
        else predictions = 1;
    }

    printf("OUTPUT\n");
    printf("number of predictions:    %u\n", predictions);
    printf("number of mispredictions: %u\n", mispredictions);
    printf("misprediction rate:       %.2f%\n", ((double)mispredictions/(double)predictions)*100);
    printf("FINAL GSHARE CONTENTS\n");

    if (strcmp(params.bp_name, "bimodal") == 0) {
        for (int i = 0; i < arr.size(); i++) {
            printf("%u   %u\n", i, arr[i]);
        }
    }







    return 0;
}
