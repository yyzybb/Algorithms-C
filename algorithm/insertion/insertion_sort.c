#include <stdio.h>
#include <stdlib.h>

void sort_insertion(int* data, int count, int (*cmp_fn)(int, int))
{
    if (count <= 1) return ;
    int i, j;
    for (i = 1; i < count; i++)
    {
        int value = data[i];
        for (j = 0; j < i; j++)
            if (cmp_fn(value, data[j]) <= 0)
                break;

        // insert to [j]
        for (; j <= i; j++)
        {
            int tmp = value;
            value = data[j];
            data[j] = tmp;
        }

        /*
        int t;
        for (t = 0; t < count; t++)
            printf("%d ", data[t]);
        printf("\n");
        */
    }
}

int cmp(int l, int r)
{
    if (l < r) return -1;
    else if (l == r) return 0;
    else return 1;
}

int sort_fn(int* data, int count)
{
    sort_insertion(data, count, &cmp);
}
