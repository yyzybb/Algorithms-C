#include <stdio.h>
#include <stdlib.h>

extern void sort_fn(int *data, int count);

int main()
{
    int count;
    scanf("%d", &count);
    int *data = (int*)malloc(count * sizeof(int));
    int i;
    for (i = 0; i < count; i++)
        scanf("%d", &data[i]);

    sort_fn(data, count);

    printf("result:\n");
    for (i = 0; i < count; i++)
        printf("%d ", data[i]);
    printf("\n");
    return 0;
}

