#include <stdio.h>
#include <sys/types.h> //key_t
#include <sys/ipc.h>   //ftok

#define DEBUG
#include "../comm.h"

// $ ls
// $ ... a ...
#define FILENAME "a"

int main()
{
    key_t key;

    key = ftok(FILENAME, 1);

    LOGI(key=%d,key);

    if(key == -1)
    {
        LOGE(ftok failed);
        return 1;
    }

    return 0; 
}
