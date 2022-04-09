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

  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool> exp(
      "mapper", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime1", "equivalent" );

  fmt::print( "[i] processing technology library\n" );

  json json_result = json::array();
  stopwatch<>::duration time{ 0 };
  {
    std::string path =  ( argc > 3 ) ? argv[3] : "../../../mig_flow_result/{}";
    std::string conf = fmt::format( path, ( argc > 1 ) ? argv[1] : "config.json" );
    std::cout << "From : " << conf << std::endl;
        
    stopwatch t( time );
    for ( auto const& benchmark : epfl_benchmarks() )
    {

      fmt::print( "[i] processing {}\n", benchmark );
      mig_network mig;

      if ( lorina::read_aiger( benchmark_path( benchmark ), aiger_reader( mig ) ) != lorina::return_code::success )
      {
        continue;
      }
      std::ifstream i( conf );
      json json_flow;
      i >> json_flow;

      u_int32_t size_before = mig.num_gates();
      u_int32_t depth_before = depth_view( mig ).depth();

      mig_flow_param ps;
      mig_flow_stats st;
      std::list<mig_flow_result*> result = mig_flow_execution( mig, json_flow, ps, &st );

      for ( mig_flow_result* res_op : result )
      {
        const auto cec = benchmark == "hyp" ? true : abc_cec( res_op->mig(), benchmark );
        exp( benchmark, res_op->name(), size_before, res_op->data().size, depth_before, res_op->data().depth, res_op->get_flow_runtime(), cec );

        json json_res;
        json_res["flow"] = res_op->save_data_to_json();
        json_res["benchmark"] = benchmark;
        json_res["eqv"] = cec;
        json_res["name"] = res_op->name();
        json_result.push_back( json_res );
      }
    }
    std::string result = fmt::format( path, "result.json" );;
    if( argc > 1 ){
      result= fmt::format( path, ( argc > 2 ) ? argv[2] : fmt::format("result/result_{}",argv[1]) );
    }
    std::cout << "To : " << result << std::endl;
    std::ofstream o( result );
    o << std::setw( 4 ) << json_result << std::endl;
  }
  exp.save();
  exp.table();
  std::cout << fmt::format( "{:5.2f} seconds passed\n", to_seconds( time ) );
  return 0;
}