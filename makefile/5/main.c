#include "add.h"
#include "sub.h"

#include <stdio.h>

int main()
{
    int a = 10;
    int b = 5;

    printf("sub:%d\n",sub(a, b));
    printf("add:%d\n",add(a, b));
    return 0;
}
