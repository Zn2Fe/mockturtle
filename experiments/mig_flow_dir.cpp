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
        
        std::string path = fmt::format("{}/{}",config,next->d_name);
        std::cout << path << std::endl;
        system( fmt::format("mig_flow_parralel {}",path).c_str());
        }
        next = readdir(dir);
    }
    return 0;
}