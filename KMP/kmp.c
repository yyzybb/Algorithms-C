#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* getNext(const char* pattern)
{
    int m = strlen(pattern);
    int *next = (int*)malloc(sizeof(int) * m);
    int k = 0;
    next[0] = 0;

    int i;
    for (i = 1; i < m; i++) {
        while (k > 0 && pattern[i] != pattern[k])
            k = next[k - 1];

        if (pattern[i] == pattern[k])
            k++;

        next[i] = k;
    }

    return next;
}

int g_cmp_count = 0;
int find(const char *source, const char* pattern, const int* next)
{
    int s_len = strlen(source), m = strlen(pattern), i = 0, j = 0;

    while (i < s_len && j < m)
    {
        g_cmp_count ++;
        if (source[i] == pattern[j]) {
            i++, j++;
        } else if (j == 0) {
            i++;
        } else {
            j = next[j];
        }
    }

    if (j == m) return i - m;

    return -1;
}

int main()
{
    const char* pattern = "abaab";
    const char* source = "ababababababaabc";
    int *next = getNext(pattern);
    int i;
    for (i = 0; i < strlen(pattern); ++i)
        printf("%d ", next[i]);
    printf("\n");

    int pos = find(source, pattern, next);
    printf("pos=%d, substr=%s\n", pos, &source[pos]);
    printf("cmp count=%d", g_cmp_count);
    return 0;
}
