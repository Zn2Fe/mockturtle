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

int main( int argc, char* argv[] )
{
  using namespace experiments;
  using namespace mockturtle;

  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float /*, bool*/> exp(
      "mapper", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime1" /*, "equivalent"*/ );

  std::string path = argv[3];
  std::string conf = argv[2] ;
  std::string benchmark_name = argv[1];


  std::ifstream i( conf );
  json json_flow;
  i >> json_flow;


  mig_network mig;

  if ( lorina::read_aiger( benchmark_path( benchmark_name ), aiger_reader( mig ) ) != lorina::return_code::success )
  {
    continue;
  }
    u_int32_t size_before = mig.num_gates();
    u_int32_t depth_before = depth_view( mig ).depth();
    mig_flow_param ps;
    mig_flow_stats st;
    
    std::list<mig_flow_result*> result = mig_flow_execution( mig, json_flow, ps, &st );

      for ( mig_flow_result* res_op : result )
      {
        exp( benchmark.value().get<std::string>(), res_op->name(), size_before, res_op->data().size, depth_before, res_op->data().depth, res_op->get_flow_runtime() /*, cec*/ );

        if(true){ //writing to csv file 

        }
        std::ofstream file_write;
        file_write.open(result_path,std::ios::app);
        if(!file_write.is_open()){        
          fmt::print("waiting for result file");
          sleep(1);
          file_write.open(result_path,std::ios::app);
        }
        if(true){ //writing to detailed json file

        }
        if(false){ //writing to detailed csv file

        }

        
        json json_res;
        json_res["flow"] = res_op->save_data_to_json();
        json_res["benchmark"] = benchmark.value().get<std::string>();
        // json_res["eqv"] = cec;
        json_res["name"] = res_op->name();
        //json_result.push_back( json_res );
        file_write << "," <<std::endl;
        file_write << std::setw( 4 ) << json_res << std::endl;
        file_write.close();

        std::ofstream global;
        global.open( csv_path, std::ios::app );
        if ( !global.is_open() )
        {
          fmt::print("waiting for csv file");
          sleep( 1 );
          global.open( csv_path, std::ios::app );
        }
        global << fmt::format(
                      "{};{};{};{};{};{};{}",
                      benchmark.value().get<std::string>(),
                      res_op->name(),
                      size_before,
                      res_op->data().size,
                      depth_before,
                      res_op->data().depth,
                      res_op->get_flow_runtime() )
               << std::endl;

        std::ofstream resultcsv;
        resultcsv.open( result_csv_path, std::ios::app );
        if ( !resultcsv.is_open() )
        {
          fmt::print("waiting for csv file");
          sleep( 1 );
          resultcsv.open( result_csv_path, std::ios::app );
        }
        int j =0;
        for(const auto& i : res_op->save_data_to_csv()){
          resultcsv << fmt::format(
            "{};{};{};{}\n",
            benchmark.value().get<std::string>(),
            res_op->name(),
            j,
            i
            );
          j++;
        }
      }
  exp.save();
  exp.table();
  std::cout << fmt::format( "{:5.2f} seconds passed\n", to_seconds( time ) );
  return 0;
}
