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

#include <mockturtle/algorithms/mig_flow_execution.hpp>
#include <experiments.hpp>

int main()
{
  using namespace experiments;
  using namespace mockturtle;

  experiment<std::string, uint32_t, uint32_t, double, uint32_t, uint32_t, double, float, float, bool, bool> exp(
      "mapper", "benchmark", "size", "size_mig", "area_after", "depth", "depth_mig", "delay_after", "runtime1", "runtime2", "equivalent1", "equivalent2" );

  fmt::print( "[i] processing technology library\n" );

  /* library to map to MIGs */
  mig_npn_resynthesis resyn{ true };
  exact_library_params eps;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );
  
  for ( auto const& benchmark : epfl_benchmarks() )
  {
    fmt::print( "[i] processing {}\n", benchmark );
    mig_network mig;
    
  }

  exp.save();
  exp.table();

  return 0;
}
