#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>

bool IsPhysicalAdapter(const std::string & adapter)
{
    std::string path = "/sys/class/net/" + adapter + "/device/";
    DIR* dir = opendir(path.c_str());
    bool ret = dir != NULL;
    if(dir) closedir(dir);
    return ret;
}

int main()
{
    int fd;
    struct ifreq ifr;
    unsigned char *mac;
    
    DIR *dir;
    dirent *entry;
  
    dir = opendir("/sys/class/net/");
    if(dir == NULL){
        std::cerr << "Error opening /sys/class/net/" << std::endl;
        return -1;
    }

    while((entry = readdir(dir)) != NULL){
        if(entry->d_name[0] != '.'){
            if(IsPhysicalAdapter(entry->d_name)){
                std::cout<< "Physical Adapter: " << entry->d_name << std::endl;
                
                fd = socket(AF_INET, SOCK_DGRAM, 0);
                if(fd < 0)
                {
                    perror("socket");
                    exit(1);
                }

                strcpy(ifr.ifr_name, entry->d_name);
                if(ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
                {
                    perror("socket");
                    exit(1);
                }
                mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
                printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
               
                close(fd);
            }
        }
    }
    closedir(dir);
   
    return 0;
}
