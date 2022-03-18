#include <string>
#include <vector>

#include <fmt/format.h>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/io/aiger_reader.hpp>

#include <mockturtle/algorithms/mig_flow_execution.hpp>
#include <experiments.hpp>

int main()
{
  using namespace experiments;
  using namespace mockturtle;

  experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool> exp(
      "mapper", "benchmark" ,"size", "size_mig", "depth", "depth_mig", "runtime1", "equivalent" );

  fmt::print( "[i] processing technology library\n" );

  /* library to map to MIGs */
  mig_npn_resynthesis resyn{ true };
  exact_library_params eps;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );

  mig_flow_algo::mig_flow_config config = {exact_lib};

  for ( auto const& benchmark : epfl_benchmarks() )
  {
    fmt::print( "[i] processing {}\n", benchmark );
    mig_network mig;
    if ( lorina::read_aiger( benchmark_path( benchmark ), aiger_reader( mig ) ) != lorina::return_code::success )
    {
      continue;
    }

    std::ifstream i( fmt::format( "{}/config/config.json", EXPERIMENTS_PATH ) );
    json json_flow;
    i >> json_flow;

    mig_flow flow( json_flow, &config, mig );

  for( operation* f_item : flow.flows){
      exp(benchmark,mig.num_gates(),f_item->data->size,depth_view(mig).depth(),f_item->data->depth,f_item->flow_runtime,f_item->data->cec);
    }
    
  }

  exp.save();
  exp.table();

  return 0;
}
