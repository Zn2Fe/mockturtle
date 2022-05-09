#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <fmt/format.h>


int main( int argc, char* argv[] ){
    printf("\n");
    std::string config =(argc>1) ? argv[1]:"config"; 
    DIR *dir = opendir( config.c_str());
    struct dirent *next;
    next = readdir(dir);
    while (next)
    {  
        if (next->d_name[0] != '.') {
        std::cout << next->d_name << std::endl;
        char tmp[256];
        getcwd(tmp, 256);
        std::string path = fmt::format("{}/{}",path,config);
        system( fmt::format("mig_flow_execution {} {} &",next->d_name,tmp).c_str());
        }
        
        next = readdir(dir);
    }
    printf("all launched");
    return 0;
}