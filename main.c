// Online C compiler to run C program online
#include <stdio.h>

// Function prototype for add
int add(int x, int y);

int main()
{
    int a = 42;
    printf("Age: %d", a);

    int res = add(10, 15);

    printf("\n%d", res);

    return a - a;
}

int add(int x, int y)
{
    return x + y;
}