#include <stdio.h>
#include <math.h>
#include <string.h>

int add(int x, int y);
void printSum(int x, int y);

int main()
{
    printSum(5, 6);

    int a = 42;
    printf("Age: %d", a);

    int res = add(10, 15);

    printf("\n%d", res);

    char v[] = "12345";
    char y[] = "12345";

    int ret = strcmp(v, y);

    return a - a;
}

int add(int x, int y)
{
    return x + y;
}

void printSum(int x, int y)
{
    int sum = x + y;
    printf("%d", sum);
}