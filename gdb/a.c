#include <stdio.h>

int getsum(int *p)
{
    int i;
    int s;
    s = 0;
    for(i = 0; i < 10; ++i)
    {
        s += i;
    }

    if(p) *p = s;

    return 999;
}


int main()
{
    int i;
    int s;
    char arr[] = {'a','b','c','d','o'};
    //(gdb) p arr
    //$ = "abcdo"
    //(gdb) ptype arr
    //type = char [5]
    //(gdb) x/6bc arr
    //0x7fffffffde90:	97 'a'	98 'b'	99 'c'	100 'd'	111 'o'	127 '\177'
    //(gdb) p arr[0]='r'
    //$10 = 114 'r'
    //(gdb) p arr
    //$11 = "rbcdo"
    //(gdb) p arr@3
    //$16 = {"rbcdo", "\177\000\000", <incomplete sequence \321>, "S\360\233d\220"}
    //(gdb) p *arr@3
    //$17 = "rbc"

    s = 0;

    getsum(&s);
    printf("getsum s=%d\n",s);

    for(i = 0; i < 5; ++i)
    {
        s += i;
        *(arr + i) = i + i;
    }

    printf("s=%d\n",s);

    return 0;
}
