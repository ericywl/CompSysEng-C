/* Lab 3 Banker's Algorithm
 * Author: Yap Wei Lok
 * ID: 1002394
 * Date: 16/02/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/boolean.h>

int numberOfCustomers; // the number of customers
int numberOfResources; // the number of resources

int *available;       // the available amount of each resource
int **maximum;        // the maximum demand of each customer
int **allocation;     // the amount currently allocated
int **need;           // the remaining needs of each customer


// Utility function to print a 2D array.
void printArray(int** array) {
    int i, j;

    for (i = 0; i < numberOfCustomers; i++) {
        for (j = 0; j < numberOfResources; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
}

// Utility function to allocate an int vector.
int *callocIntVector(int size) {
    int *a = calloc(sizeof(int) * size, sizeof(int));
    return a;
}

// Utility function to free an int vector.
void freeIntVector(int *a) {
    free(a);
}

// Utility function to allocate an int matrix.
int **callocIntMatrix(int numRows, int numColumns) {
    int i, **a = calloc(sizeof(int*) * (numRows+1), sizeof(int));
    for (i = 0; i < numRows; i++) {
        a[i] = calloc(sizeof(int) * numColumns, sizeof(int));
    }

    return a;
}

// Utility function to free an int matrix.
void freeIntMatrix(int **a) {
    int i = 0;
    while (a[i] != 0)
        free(a[i++]);

    free(a);
}

/**
 * Initializes the state of the bank.
 * @param resources  An array of the available count for each resource.
 * @param m          The number of resources.
 * @param n          The number of customers.
 */
void initBank(int *resources, int m, int n) {
    // Allocate memory for the vector and matrices
    available = callocIntVector(m);
    need = callocIntMatrix(n, m);
    allocation = callocIntMatrix(n, m);
    maximum = callocIntMatrix(n, m);

    numberOfCustomers = n;
    numberOfResources = m;
    available = resources;
}

/**
 * Frees the memory used to store the state of the bank.
 */
void freeBank() {
    freeIntVector(available);
    freeIntMatrix(need);
    freeIntMatrix(allocation);
    freeIntMatrix(maximum);
}

/**
 * Prints the state of the bank.
 */
void printState() {
    printf("\n\nAvailable:\n");
    for (int i = 0; i < numberOfResources; i++)
        printf("%d ", available[i]);

    printf("\n");
    printf("\nMaximum:\n");
    printArray(maximum);

    printf("\nAllocation:\n");
    printArray(allocation);

    printf("\nNeed:\n");
    printArray(need);
    printf("\n");
}

/**
 * Sets the maximum number of demand of each resource for a customer.
 * @param customerIndex  The customer's index (0-indexed).
 * @param maximumDemand  An array of the maximum demanded count for each resource.
 */
void setMaximumDemand(int customerIndex, const int *maximumDemand) {
    for (int i = 0; i < numberOfResources; i++) {
        maximum[customerIndex][i] = maximumDemand[i];
        need[customerIndex][i] = maximumDemand[i];
    }
}

/**
 * Check if all elements in array1 is greater than the respective element in array2.
 * @return 1 if the above is true and 0 if false.
 *
 */
int leqArray(const int *array1, const int *array2, int numOfElements) {
    for (int i = 0; i < numOfElements; i++) {
        if (array1[i] > array2[i])
            return 0;
    }

    return 1;
}

/**
 * Checks if the request will leave the bank in a safe state.
 * @param customerIndex  The customer's index (0-indexed).
 * @return 1 if the requested resources will leave the bank in a
 *         safe state, else 0
 */
int checkSafe() {
    int i, j;
    // Using integer as boolean, 0 for false, 1 for true
    int* finish = callocIntVector(numberOfCustomers);
    // Allocate temporary memory to copy the bank state.
    int *work = callocIntVector(numberOfResources);
    for (i = 0; i < numberOfResources; i++)
        work[i] = available[i];

    for (i = 0; i < numberOfCustomers; i++) {
        if (finish[i] == 0 && leqArray(need[i], work, numberOfResources) == 1) {
            for (j = 0; j < numberOfResources; j++)
                work[j] += allocation[i][j];

            finish[i] = 1;
            i = -1;
        }
    }

    for (j = 0; j < numberOfCustomers; j++)
        if (finish[j] == 0) return 0;

    return 1;
}

/**
 * Requests resources for a customer loan.
 * If the request leave the bank in a safe state, it is carried out.
 * @param customerIndex  The customer's index (0-indexed).
 * @param request        An array of the requested count for each resource.
 * @return 1 if the requested resources can be loaned, else 0.
 */
int requestResources(int customerIndex, int *request) {
    int i;

    printf("\nCustomer %d requesting:\n", customerIndex);
    for (i = 0; i < numberOfResources; i++) {
        printf("%d ", request[i]);
    }

    for (i = 0; i <numberOfResources; i++) {
        if (request[i] > need[customerIndex][i])
            return 0;

        if (request[i] > available[i])
            return 0;
    }

    for (i = 0; i < numberOfResources; i++) {
        available[i] -= request[i];
        allocation[customerIndex][i] += request[i];
        need[customerIndex][i] -= request[i];
    }

    int safe = checkSafe();
    if (safe == 0) {
        for (i = 0; i < numberOfResources; i++) {
            available[i] += request[i];
            allocation[customerIndex][i] -= request[i];
            need[customerIndex][i] += request[i];
        }

        return 0;
    }

    return 1;
}

/**
 * Releases resources borrowed by a customer. Assume release is valid for simplicity.
 * @param customerIndex  The customer's index (0-indexed).
 * @param release        An array of the release count for each resource.
 */
void releaseResources(int customerIndex, int *release) {
    int i;

    printf("\nCustomer %d releasing:\n", customerIndex);
    for (i = 0; i < numberOfResources; i++) {
        printf("%d ", release[i]);
        available[i] += release[i];
        allocation[customerIndex][i] -= release[i];
        need[customerIndex][i] += release[i];
    }
}

/**
 * Parses and runs the file simulating a series of resource request and releases.
 * Provided for your convenience.
 * @param filename  The name of the file.
 */
void runFile(const char * filename) {
    FILE *fp = fopen(filename, "r");
    int c = 0, i = 0, j = 0, m = 0, n = 0, bankInited = 0,
            lineLen = 0, prevLineEnd = 0, maxLineLen = 0;
    do {
        if (c == '\n' || c == EOF) {
            lineLen = i - prevLineEnd;
            prevLineEnd = i;
            if (lineLen > maxLineLen) maxLineLen = lineLen;
        }
        i++;
    } while ((c = fgetc(fp)) != EOF);

    rewind(fp);
    lineLen++;
    char *line = malloc(lineLen), *token;
    i = 0;
    while (fgets(line, lineLen, fp) != NULL) {
        for (j = 0; j < lineLen-1; j++)
            if (line[j] == '\n') line[j] = '\0';
        if (i == 0) {
            token = strtok(line, ",");
            token = strtok(NULL, ",");
            n = atoi(token);
        } else if (i == 1) {
            token = strtok(line, ",");
            token = strtok(NULL, ",");
            m = atoi(token);
        } else if (i == 2) {
            token = strtok(line, ",");
            token = strtok(NULL, ",");
            int *resources = malloc(sizeof(int) * m);
            for (j = 0; j < m; j++) {
                resources[j] = atoi(strtok(j == 0 ? token : NULL, " "));
            }
            initBank(resources, m, n);
            bankInited = 1;
            free(resources);
        } else {
            int *resources = malloc(sizeof(int) * m);
            token = strtok(line, ",");
            if (strcmp(token, "c") == 0) {
                int customerIndex = atoi(strtok(NULL, ","));
                int *resources = malloc(sizeof(int) * m);
                token = strtok(NULL, ",");
                for (j = 0; j < m; j++) {
                    resources[j] = atoi(strtok(j == 0 ? token : NULL, " "));
                }
                setMaximumDemand(customerIndex, resources);
                free(resources);
            } else if (strcmp(token, "r") == 0) {
                int customerIndex = atoi(strtok(NULL, ","));
                int *resources = malloc(sizeof(int) * m);
                token = strtok(NULL, ",");
                for (j = 0; j < m; j++) {
                    resources[j] = atoi(strtok(j == 0 ? token : NULL, " "));
                }
                requestResources(customerIndex, resources);
                free(resources);
            } else if (strcmp(token, "f") == 0) {
                int customerIndex = atoi(strtok(NULL, ","));
                int *resources = malloc(sizeof(int) * m);
                token = strtok(NULL, ",");
                for (j = 0; j < m; j++) {
                    resources[j] = atoi(strtok(j == 0 ? token : NULL, " "));
                }
                releaseResources(customerIndex, resources);
                free(resources);
            } else if (strcmp(token, "p") == 0) {
                printState();
            }
        }
        i++;
    }
    if (bankInited)
        freeBank();

    free(line);
    fclose(fp);
}

/**
 * Main function
 * @param args  The command line arguments
 */
int main (int argc, const char ** argv)
{
    if (argc > 1)
        runFile(argv[1]);

    return 0;
}



