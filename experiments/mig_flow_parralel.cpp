#include <dirent.h>
#include <fmt/format.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#define WRITE_IN_CSV true
#define WRITE_IN_JSON_VERBOSE false
#define WRITE_IN_CSV_VERBOSE true

int main( int argc, char* argv[] ){
    printf("\n");
    
    char tmp[256];
    getcwd(tmp, 256);
    std::string conf = (argc>1) ? argv[1]:"config/config.json"; 
    std::ifstream i( conf );
    nlohmann::json json_flow;
    i >> json_flow;

    std::string global_name = json_flow.at("name").get<std::string>();

    std::string csv_path = fmt::format( "{}", fmt::format("resultcsv/{}.csv",global_name));
    std::string json_v_path = fmt::format(  "{}", fmt::format("result/{}.json",global_name) );
    std::string csv_v_path = fmt::format(  "{}", fmt::format("result/{}.csv",global_name) );

    std::ofstream writer;
    if(WRITE_IN_CSV){
        writer.open(csv_path);
        writer << "benchmark;flow;size;size_mig;depth;depth_mig;runtime" <<std::endl;
        writer.close();
    }

    if(WRITE_IN_CSV_VERBOSE){
        writer.open(csv_v_path);
        writer << "benchmark;flow;op_num;op_type;size_mig;depth_mig;runtime;size;depth" <<std::endl;
        writer.close();
    }

    if(WRITE_IN_JSON_VERBOSE){
        writer.open(json_v_path);
        writer.close();
    }

    for ( auto const& benchmark : json_flow.at( "do" ).items() ){
        system( fmt::format("mig_flow_execution_parralel {} {} {} &",benchmark.value().get<std::string>(),conf,tmp).c_str());
    }
    printf("all launched");
    return 0;
}