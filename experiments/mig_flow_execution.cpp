/* mockturtle: C++ logic network library
 * Copyright (C) 2018-2021  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string>
#include <vector>

#include <fmt/format.h>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/mapper.hpp>
//#include <mockturtle/algorithms/mig_flow_execution.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <nlohmann/json.hpp>
#include <thread>

#include <experiments.hpp>

using namespace experiments;
using namespace mockturtle;
using namespace nlohmann;

namespace mockturtle
{
struct exp_container
{
  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool>* exp;
};

struct mig_flow_data
{
  std::string benchmark;
  const u_int32_t size_before;
  const u_int32_t depth_before;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    map_params,
    required_time,
    skip_delay_round,
    area_flow_rounds,
    ela_rounds,
    eswp_rounds,
    switching_activity_patterns, enable_logic_sharing,
    logic_sharing_cut_limit,
    verbose )
} // namespace mockturtle

mig_network execute_map( mig_flow_data data_param, exp_container exp_con, json config, mig_network mig, float* runtime )
{

  map_params map_ps;
  map_stats map_st;
  // TODO : param
  map_ps.skip_delay_round = true;
  map_ps.required_time = std::numeric_limits<double>::max(),
  map_ps.ela_rounds = 3;
  map_ps.enable_logic_sharing = true;
  map_ps.logic_sharing_cut_limit = 1;

  mig_network res = map( mig, data_param.exact_lib, map_ps, &map_st );
  *runtime += to_seconds( map_st.time_total );
  return res;
}

/*
type :
        - compute :
                101 : branch
                102 : end
                103 : loop
        - algorithm
                201 : map
*/

/*
//TODO multithreading
void branch_execute_flow( mig_flow_data data_param, exp_container exp_con, json config,mig_network mig, float runtime){
        float current_runtime = runtime;
        std::thread new_thread(execute_flow,data_param,exp_con,config,mig,&current_runtime);

}
*/

mig_network execute_flow( mig_flow_data data_param, exp_container exp_con, json config, mig_network mig, float* runtime )
{
  mig_network res = mig;
  for ( auto& item : config["flow"].items() )
  {
    
    float individual_runtime = 0;
    const auto cec = data_param.benchmark == "hyp" ? true : abc_cec( res, data_param.benchmark );

	//if (item.value()["type"].get<int>() == 101){
	float current_runtime = *runtime;
	//	std::thread (execute_flow, data_param, exp_con, item.value(), cleanup_dangling( res ), &current_runtime );
	//}
    switch ( item.value()["type"].get<int>() )
    {
    case 101: // branching
      execute_flow( data_param, exp_con, item.value(), cleanup_dangling( res ), &current_runtime );
      break;
    case 102: // ending
      fmt::print( "End of flow : {} \n", item.value()["name"].get<std::string>() );
      ( *exp_con.exp )(
          data_param.benchmark,
          item.value()["name"].get<std::string>(),
          data_param.size_before,
          res.num_gates(),
          data_param.depth_before,
          depth_view( res ).depth(),
          *runtime,
          cec );

      break;
    case 103: // looping
      while ( true )
      {
        const u_int32_t res_size = res.num_gates();
        mig_network mig_calculated = execute_flow( data_param, exp_con, item.value(), cleanup_dangling( res ), &individual_runtime );
        if ( mig_calculated.num_gates() >= res_size )
        {
          break;
        }
        res = mig_calculated;
      }
      break;

    case 201:
      res = execute_map( data_param, exp_con, item.value(), res, &individual_runtime );
      break;
	  // TODO save in JSON

    default:
      fmt::print( "! - Error " );
      break;
    }
    if(item.value()["type"].get<int>()%100  == 2){
      json stats = {
        {"size",res.num_gates()},
        {"depth",depth_view(res).depth()},
        {"runtime",individual_runtime}
      };
      item.value()["stats"] = stats;
    }
    *runtime += individual_runtime;
    
  }
  return res;
}

int main()
{

  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool> exp(
      "mapper", "benchmark", "algorithm", "size", "size_mig", "depth", "depth_mig", "runtime", "equivalent" );

  /* library to map to MIGs */
  mig_npn_resynthesis resyn{ true };
  exact_library_params eps;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );


  for ( auto const& benchmark : epfl_benchmarks() )
  {
    fmt::print( "[i] processing {}\n", benchmark );

    mig_network mig;

    if ( lorina::read_aiger( benchmark_path( benchmark ), aiger_reader( mig ) ) != lorina::return_code::success )
    {
      continue;
    }

    mig_flow_data data_param = {
        benchmark,
        mig.num_gates(),
        depth_view( mig ).depth(),
        exact_lib };

    exp_container exp_con = {
        &exp };

    std::ifstream i( fmt::format( "{}/config/config.json", EXPERIMENTS_PATH ) );
    json config;
    i >> config;

    float runtime = 0;
    //std::cout << config.dump(4) << "\n\n" << std::endl;
    execute_flow( data_param, exp_con, config, cleanup_dangling( mig ), &runtime );
    //std::cout << config.dump(4) << "\n\n" << std::endl;
    
  }

  exp.save();
  exp.table();
  return 0;
}
