#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <fmt/format.h>


int main(){
    DIR *dir = opendir("config");
    struct dirent *next;
    next = readdir(dir);
    while (next)
    {
        if (next->d_name[0] != '.') {
            std::cout << next->d_name << std::endl;
            char tmp[256];
            getcwd(tmp, 256);
            std::string conf = fmt::format("config/{}",next->d_name);
            std::string res = fmt::format("result/result_{}",next->d_name);
            system( fmt::format("mig_flow_execution {} {} {} &",conf,res,tmp).c_str());
        }
        
        next = readdir(dir);
    }
}