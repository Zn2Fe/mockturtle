#include <string>
#include <vector>

#include <fmt/format.h>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <experiments.hpp>
#include <mockturtle/algorithms/mig_flow_execution.hpp>

#include <nlohmann/json.hpp>

#define WRITE_IN_CSV true
#define WRITE_IN_JSON_VERBOSE false
#define WRITE_IN_CSV_VERBOSE true


int main( int argc, char* argv[] )
{
  using namespace experiments;
  using namespace mockturtle;
  
  std::string path = ( argc > 3 ) ? fmt::format( "{}/{{}}", argv[3] ) : "{}";
  std::string conf = fmt::format( path, ( ( argc > 2 ) ? argv[2] : "config.json" ) );

  std::ifstream i( conf );
  json json_flow;
  i >> json_flow;
  i.close();
  
  std::string global_name = json_flow.at("name").get<std::string>();

  std::string csv_path = fmt::format( path, fmt::format("../resultcsv/{}.csv",global_name));
  std::string json_v_path = fmt::format( path, fmt::format("../result/{}.json",global_name) );
  std::string csv_v_path = fmt::format( path, fmt::format("../resultcsv_v/{}.csv",global_name) );

  std::ofstream writer;
  std::string benchmark = (argc >1) ? argv[1] : "adder";

  mig_network mig;
  if ( lorina::read_aiger( benchmark_path( benchmark ), aiger_reader( mig ) ) != lorina::return_code::success )
  {
    return 1;
  }

  u_int32_t size_before = mig.num_gates();
  u_int32_t depth_before = depth_view( mig ).depth();

  mig_flow_param ps;
  ps.progress = false;
  mig_flow_stats st;
  std::list<mig_flow_result*> result = mig_flow_execution( mig, json_flow, ps, &st );

  for ( mig_flow_result* res_op : result )
  {
      if(WRITE_IN_CSV){
      writer.open(csv_path,std::ios::app);
      if(!writer.is_open()){
        fmt::print("Waiting for file\n");
        sleep(1);
        writer.open(csv_path,std::ios::app);
      }
      writer <<  fmt::format(
                  "{};{};{};{};{};{};{}",
                  benchmark,
                  res_op->name(),
                  size_before,
                  res_op->data().size,
                  depth_before,
                  res_op->data().depth,
                  res_op->get_flow_runtime() )
            << std::endl;
      writer.close();
    }
    
    if(WRITE_IN_JSON_VERBOSE){
      json json_res;
      json_res["flow"] = res_op->save_data_to_json();
      json_res["benchmark"] = benchmark;
      json_res["name"] = res_op->name();

      writer.open(json_v_path,std::ios::app);
      if(!writer.is_open()){
        fmt::print("Waiting for file\n");
        sleep(1);
        writer.open(json_v_path,std::ios::app);
      }
      writer << "," <<std::endl;
      writer << std::setw( 4 ) << json_res << std::endl;
      writer.close();
    }
    
    if(WRITE_IN_CSV_VERBOSE){
      int num = 0;
      writer.open(csv_v_path,std::ios::app);
      if(!writer.is_open()){
        fmt::print("Waiting for file\n");
        sleep(1);
      writer.open(csv_v_path,std::ios::app);
      }
      for(std::string res : res_op->save_data_to_csv())
      {
        writer <<  fmt::format(
                  "{};{};{};{};{};{}\n",
                  benchmark,
                  res_op->name(),
                  num,
                  res,
                  size_before,
                  depth_before
                    );
        num+=1; 
      }

      writer.close();
    }
  }
  return 0;
}
