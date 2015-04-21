#include <stdlib.h>

void merge(int *data, int mid, int count)
{
    int i;
    int *tmp = (int*)malloc(count * sizeof(int));
    int *lhs = data, *rhs = data + mid;
    for (i = 0; i < count; ++i)
    {
        if (lhs - data == mid) {
            tmp[i] = *rhs++;
        } else if (rhs - data == count) {
            tmp[i] = *lhs++;
        } else {
            if (*lhs <= *rhs) {
                tmp[i] = *lhs++;
            } else {
                tmp[i] = *rhs++;
            }
        }
    }

    for (i = 0; i < count; ++i)
    {
        data[i] = tmp[i];
    }

    free(tmp);
}

void sort_fn(int *data, int count)
{
    if (count <= 1) return ;

    int mid = count / 2;
    sort_fn(data, mid);
    sort_fn(data + mid, count - mid);
    merge(data, mid, count);
}
