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
#define WRITE_IN_CSV_VERBOSE false


int main( int argc, char* argv[] )
{
  using namespace experiments;
  using namespace mockturtle;

  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float /*, bool*/> exp(
      "mapper", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime1" /*, "equivalent"*/ );

  std::string path = ( argc > 2 ) ? fmt::format( "{}/{{}}", argv[2] ) : "{}";
  std::string conf = fmt::format( path, ( ( argc > 1 ) ? argv[1] : "config/config.json" ) );

  std::ifstream i( conf );
  json json_flow;
  i >> json_flow;
  i.close();
  
  std::string global_name = json_flow.at("name").get<std::string>();

  std::string csv_path = fmt::format( path, fmt::format("resultcsv/{}.csv",global_name));
  std::string json_v_path = fmt::format( path, fmt::format("result/{}.json",global_name) );
  std::string csv_v_path = fmt::format( path, fmt::format("result/{}.csv",global_name) );

  std::ofstream csv_write;
  if(WRITE_IN_CSV){
    csv_write.open(csv_path);
    csv_write << "benchmark;flow;size;size_mig;depth;depth_mig;runtime" <<std::endl;
    csv_write.close();
  }

  std::ofstream csv_v_write;
  if(WRITE_IN_CSV_VERBOSE){

  }

  std::ofstream json_v_write;
  if(WRITE_IN_JSON_VERBOSE){
    json_v_write.open(json_v_path);
    json_v_write.close();
  }

  stopwatch<>::duration time{ 0 };
  {

    stopwatch t( time );
    for ( auto const& benchmark : json_flow.at( "do" ).items() )
    {
      mig_network mig;
      if ( lorina::read_aiger( benchmark_path( benchmark.value().get<std::string>() ), aiger_reader( mig ) ) != lorina::return_code::success )
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

        if(WRITE_IN_CSV){
          csv_write.open(csv_path,std::ios::app);
          csv_write <<  fmt::format(
                      "{};{};{};{};{};{};{}",
                      benchmark.value().get<std::string>(),
                      res_op->name(),
                      size_before,
                      res_op->data().size,
                      depth_before,
                      res_op->data().depth,
                      res_op->get_flow_runtime() )
               << std::endl;
          csv_write.close();
        }
        
        if(WRITE_IN_JSON_VERBOSE){
          json json_res;
          json_res["flow"] = res_op->save_data_to_json();
          json_res["benchmark"] = benchmark.value().get<std::string>();
          json_res["name"] = res_op->name();

          json_v_write.open(json_v_path,std::ios::app);
          json_v_write << "," <<std::endl;
          json_v_write << std::setw( 4 ) << json_res << std::endl;
          json_v_write.close();
        }
      }
    }
  }
  exp.save();
  exp.table();
  std::cout << fmt::format( "{:5.2f} seconds passed\n", to_seconds( time ) );
  return 0;
}
