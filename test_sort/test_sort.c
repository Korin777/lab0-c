#include <stdio.h>
#include <string.h>
#include "list_sort.h"
#include "queue.h"
long long compare_count = 0;

int main(int argc, char *argv[])
{
    FILE *fp;
    fp = fopen(argv[1], "r");
    struct list_head *queue = q_new();
    if (!queue) {
        fprintf(stderr, "Can't allocate memory.\n");
        fclose(fp);
        return -1;
    }
    char *str = NULL;
    size_t len = 0;
    while (getline(&str, &len, fp) != -1) {
        char *nl = strchr(str, '\n');
        if (nl)
            *nl = '\0';
        if (!q_insert_head(queue, str)) {
            fprintf(stderr, "Can't allocate memory.\n");
            q_free(queue);
            free(str);
            fclose(fp);
            return -2;
        }
    }
    free(str);
#ifdef KSORT
    list_sort(NULL, queue, cmp);
#elif defined QSORT
    QuickSort(queue);
#else
    q_sort(queue);
#endif
    // FILE * fp2;
    // fp2 = fopen ("result.txt", "w+");
    // element_t *itr;
    // list_for_each_entry(itr,queue,list) {
    //     fprintf(fp2,"%s\n",itr->value);
    // }
    printf("%lld\n", compare_count);
#ifdef KSORT
    puts("ksort");
#elif defined QSORT
    puts("my quicksort");
#else
    puts("my merge sort");
#endif
    q_free(queue);
    fclose(fp);
    return 0;
}