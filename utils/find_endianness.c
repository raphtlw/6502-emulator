#include <stdio.h>

int main()
{
    int n = 1;
    // little endian if true
    if (*(char *)&n == 1)
    {
        printf("Processor is little endian");
    }
    return 0;
}
