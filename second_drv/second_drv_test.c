#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    int fd;
    char key_vals[4];
    int count = 0;
    fd = open("/dev/buttons",O_RDWR);
    if(fd < 0)
        printf("can't open!\n");

    while(1) {
        read(fd, key_vals, 4);
        if(key_vals[0] != 1 || key_vals[1] != 1 || key_vals[2] != 1 || key_vals[3] != 1) {
            printf("%04d, %d, %d, %d, %d \n", count++, key_vals[0], key_vals[1] , key_vals[2] ,key_vals[3]);
        }
    }


    return 0;
}
