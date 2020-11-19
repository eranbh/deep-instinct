/*
* a simple test for the lkm_sort kernel module
* this will:
* 1. open the device and obtain an fd to it
* 2. write a buffer of unsorted chars to it.
* 3. read from it (this could be done in chunks)
*/
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string_view>
#include <iostream>

const std::string_view DEV_NAME = "/dev/lkm_sort";


int main(int argc, char* argv[])
{
    int fd = open(DEV_NAME.data(), O_RDWR);
    if (0 > fd) {
        std::cout << "failed open. errno: " << errno << std::endl;
        return 1;
    }
    char buff[] = {0x0a, 'D', 'e', 'e', 'p', ' ', 'I', 'n', 's', 't', 'i', 'n', 'c', 't', 0x0a};
    std::cout << "unsorted: " << buff << std::endl;
    int len = sizeof(buff);

    int sts = write(fd, buff, len);
    if(0 > sts){
        std::cout << "failed write. errno: " << errno << std::endl;
        return 2;
    }

    sts = read(fd, buff, len);
    if(0 > sts){
        std::cout << "failed read. errno: " << errno << std::endl;
        return 2;
    }

    std::cout << "sorted: " << buff << std::endl;

    return 0;
}