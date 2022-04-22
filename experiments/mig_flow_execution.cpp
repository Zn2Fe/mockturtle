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

  std::string path = ( argc > 3 ) ? fmt::format( "{}/{{}}", argv[3] ) : "{}";
  std::string conf = fmt::format( path, ( ( argc > 1 ) ? argv[1] : "config/config.json" ) );
  std::string result_path = fmt::format( path, ( ( argc > 2 ) ? argv[2] : "result/result.json" ) );
  std::string csv_path = fmt::format( path, "global.csv" );

  std::ofstream o( result_path );
  o << std::setw( 4 ) << json::array() << std::endl;
  o.close();

  std::ifstream i( conf );
  json json_flow;
  i >> json_flow;
  if ( argc > 4 )
  {
    std::cout << "From : " << conf << std::endl;
    std::cout << "To : " << result_path << std::endl;
    fmt::print( "[i] processing technology library\n" );
  }

  stopwatch<>::duration time{ 0 };
  {

    stopwatch t( time );
    for ( auto const& benchmark : json_flow.at( "do" ).items() )
    {
      if ( argc > 4 )
      {
        fmt::print( "[i] processing {}\n", benchmark.value().get<std::string>() );
      }

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
        std::ifstream file_result( result_path );
        json json_result;
        file_result >> json_result;
        file_result.close();

        // const auto cec = benchmark == "hyp" ? true : abc_cec( res_op->mig(), benchmark );
        exp( benchmark.value().get<std::string>(), res_op->name(), size_before, res_op->data().size, depth_before, res_op->data().depth, res_op->get_flow_runtime() /*, cec*/ );

        json json_res;
        json_res["flow"] = res_op->save_data_to_json();
        json_res["benchmark"] = benchmark.value().get<std::string>();
        // json_res["eqv"] = cec;
        json_res["name"] = res_op->name();
        json_result.push_back( json_res );

        std::ofstream result_out( result_path );
        result_out << std::setw( 4 ) << json_result << std::endl;
        result_out.close();

        std::ofstream global;
        global.open( csv_path, std::ios::app );
        if ( !global.is_open() )
        {
          sleep( 1 );
          global.open( csv_path, std::ios::app );
        }
        global << fmt::format(
                      "{};{};{};{};{};{};{}",
                      benchmark.value().get<std::string>(),
                      res_op->name(),
                      size_before,
                      res_op->data().size,
                      depth_before, res_op->data().depth,
                      res_op->get_flow_runtime() )
               << std::endl;
      }
    }
  }
  exp.save();
  exp.table();
  std::cout << fmt::format( "{:5.2f} seconds passed\n", to_seconds( time ) );
  return 0;
}
