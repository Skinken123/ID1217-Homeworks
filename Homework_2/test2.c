#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    omp_set_num_threads(4);
    #pragma omp parallel //num_threads(4)
    {
        int id = omp_get_thread_num();
        printf("Hi im thread number %d\n", id);
    }
}