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

/*!
  \file mig_flow_execution.hpp
  \brief mig_flow

  \author Nicolas DEVAUX
*/

#pragma once

#include <mockturtle/algorithms/cut_rewriting.hpp>
#include <mockturtle/algorithms/det_randomize.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/mig_algebraic_rewriting.hpp>
#include <mockturtle/algorithms/mig_resub.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/algorithms/rewriting.hpp>
#include <mockturtle/networks/mig.hpp>

#include <nlohmann/json.hpp>

namespace mockturtle
{
using namespace nlohmann;
void from_json( const json& j, map_params& param )
{
  try
  {
    j.at( "required_time" ).get_to( param.required_time );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "skip_delay_round" ).get_to( param.skip_delay_round );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "area_flow_rounds" ).get_to( param.area_flow_rounds );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "ela_rounds" ).get_to( param.ela_rounds );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "eswp_rounds" ).get_to( param.eswp_rounds );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "enable_logic_sharing" ).get_to( param.enable_logic_sharing );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "logic_sharing_cut_limit" ).get_to( param.logic_sharing_cut_limit );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "verbose" ).get_to( param.verbose );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "cut_enumeration_ps.cut_limit" ).get_to( param.cut_enumeration_ps.cut_limit );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "use_dont_cares" ).get_to( param.use_dont_cares );
  }
  catch ( const json::exception& )
  {
  }
}

void from_json( const json& j, resubstitution_params& param )
{
  try
  {
    j.at( "max_pis" ).get_to( param.max_pis );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "max_divisors" ).get_to( param.max_divisors );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "max_inserts" ).get_to( param.max_inserts );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "skip_fanout_limit_for_roots" ).get_to( param.skip_fanout_limit_for_roots );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "skip_fanout_limit_for_divisors" ).get_to( param.skip_fanout_limit_for_divisors );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "progress" ).get_to( param.progress );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "verbose" ).get_to( param.verbose );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "use_dont_cares" ).get_to( param.use_dont_cares );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "window_size" ).get_to( param.window_size );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "preserve_depth" ).get_to( param.preserve_depth );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "max_clauses" ).get_to( param.max_clauses );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "conflict_limit" ).get_to( param.conflict_limit );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "random_seed" ).get_to( param.random_seed );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "odc_levels" ).get_to( param.odc_levels );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "max_trials" ).get_to( param.max_trials );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "max_divisors_k" ).get_to( param.max_divisors_k );
  }
  catch ( const json::exception& )
  {
  }
}

void from_json( const json& j, cut_rewriting_params& param )
{
  try
  {
    j.at( "cut_enumeration_ps.cut_size" ).get_to( param.cut_enumeration_ps.cut_size );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "cut_enumeration_ps.cut_limit" ).get_to( param.cut_enumeration_ps.cut_limit );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "cut_enumeration_ps.minimize_truth_table" ).get_to( param.cut_enumeration_ps.minimize_truth_table );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "allow_zero_gain" ).get_to( param.allow_zero_gain );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "use_dont_cares" ).get_to( param.use_dont_cares );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "candidate_selection_strategy" ).get_to( param.candidate_selection_strategy );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "min_cand_cut_size" ).get_to( param.min_cand_cut_size );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "preserve_depth" ).get_to( param.preserve_depth );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "progress" ).get_to( param.progress );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "verbose" ).get_to( param.verbose );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "very_verbose" ).get_to( param.very_verbose );
  }
  catch ( const json::exception& )
  {
  }
}

void from_json( const json& j, mig_algebraic_depth_rewriting_params& param )
{
  try
  {
    j.at( "strategy" ).get_to( param.strategy );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "overhead" ).get_to( param.overhead );
  }
  catch ( const json::exception& )
  {
  }
  try
  {
    j.at( "allow_area_increase" ).get_to( param.allow_area_increase );
  }
  catch ( const json::exception& )
  {
  }
}

} // namespace mockturtle

namespace mockturtle
{
struct compute_data
{
  float runtime = -1;
  u_int32_t size = -1;
  u_int32_t depth = -1;
};

struct libraries
{
  mig_npn_resynthesis* resyn;
  exact_library<mig_network, mig_npn_resynthesis>* exact_lib;
};

namespace compute
{

void mig_flow_mapper( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{

  map_params map_ps = param.get<map_params>();
  map_stats flow_map_stat;

  network = map( network, *library.exact_lib, map_ps, &flow_map_stat );
  network = cleanup_dangling( network );

  op_data->runtime = to_seconds( flow_map_stat.time_total );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
}

void mig_flow_rewrite( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{

  rewriting_params rewrite_ps;
  rewrite_ps.use_mffc = true;
  rewrite_ps.use_dont_cares = false;
  rewrite_ps.allow_multiple_structures = true;
  rewriting_stats flow_rewrite_stats;
  fanout_view f_network{ network };
  rewrite( f_network, *library.exact_lib, rewrite_ps, &flow_rewrite_stats );
  network = cleanup_dangling( network );
  op_data->runtime = to_seconds( flow_rewrite_stats.time_total );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
  return;
}

void mig_flow_resubstitution( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{

  resubstitution_params ps = param.get<resubstitution_params>();
  depth_view depth_mig{ network };
  fanout_view fanout_mig{ depth_mig };
  resubstitution_stats flow_resubstitution_stats;

  mig_resubstitution( fanout_mig, ps, &flow_resubstitution_stats );
  network = cleanup_dangling( network );

  op_data->runtime = to_seconds( flow_resubstitution_stats.time_total );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
}

void mig_flow_cut_rewriting( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{
  cut_rewriting_params ps = param.get<cut_rewriting_params>();
  cut_rewriting_stats flow_cut_rewriting_stats;

  network = cut_rewriting( network, *library.resyn, ps, &flow_cut_rewriting_stats );
  network = cleanup_dangling( network );

  op_data->runtime = to_seconds( flow_cut_rewriting_stats.time_total );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
}

void mig_flow_algebraic_rewriting( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{
  mig_algebraic_depth_rewriting_params ps = param.get<mig_algebraic_depth_rewriting_params>();
  depth_view mig_depth{ network };
  fanout_view mig_fanout{ mig_depth };
  mig_algebraic_depth_rewriting_stats flow_mig_algebraic_depth_rewriting_stats;

  mig_algebraic_depth_rewriting( mig_fanout, ps, &flow_mig_algebraic_depth_rewriting_stats );
  network = cleanup_dangling( network );

  op_data->runtime = to_seconds( flow_mig_algebraic_depth_rewriting_stats.time_total );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
}

void mig_flow_det_randomize( mig_network& network, nlohmann::json param, compute_data* op_data, libraries& library )
{

  stopwatch<>::duration time{ 0 };
  {
    stopwatch t( time );
    network = det_randomize( network );
  }

  op_data->runtime = to_seconds( time );
  op_data->size = network.num_gates();
  op_data->depth = depth_view( network ).depth();
}
} // namespace compute

void compute_network( mig_network& network, nlohmann::json& config, compute_data* op_data, libraries& library )
{
  std::string operation_type = config.at( "operation_type" ).get<std::string>();
  json param;
  if ( config.contains( "param" ) )
  {
    param = config.at( "param" );
  }

  if ( operation_type == "mig_mapper" )
  {
    compute::mig_flow_mapper( network, param, op_data, library );
    return;
  }
  if ( operation_type == "maps" )
  {
    param["required_time"] = std::numeric_limits<double>::max();
    param["skip_delay_round"] = true;
    param["area_flow_rounds"] = 1;
    param["ela_rounds"] = 3;
    param["eswp_rounds"] = 0;
    param["enable_logic_sharing"] = true;
    param["logic_sharing_cut_limit"] = 1;
    compute::mig_flow_mapper( network, param, op_data, library );
    return;
  }
  if ( operation_type == "mape" )
  {
    param["required_time"] = std::numeric_limits<double>::max();
    param["skip_delay_round"] = true;
    param["area_flow_rounds"] = 1;
    param["ela_rounds"] = 2;
    param["eswp_rounds"] = 0;
    param["enable_logic_sharing"] = false;
    param["logic_sharing_cut_limit"] = 1;
    compute::mig_flow_mapper( network, param, op_data, library );
    return;
  }
  if ( operation_type == "mapd" )
  {
    param["required_time"] = 0;
    param["skip_delay_round"] = false;
    param["area_flow_rounds"] = 1;
    param["ela_rounds"] = 2;
    param["eswp_rounds"] = 1;
    param["enable_logic_sharing"] = false;
    param["logic_sharing_cut_limit"] = 1;
    compute::mig_flow_mapper( network, param, op_data, library );
    return;
  }
  if ( operation_type == "mapdc" )
  {
    param["required_time"] = std::numeric_limits<double>::max();
    param["skip_delay_round"] = true;
    param["area_flow_rounds"] = 1;
    param["ela_rounds"] = 3;
    param["eswp_rounds"] = 0;
    param["cut_enumeration_ps.cut_limit"] = 8;
    param["enable_logic_sharing"] = true;
    param["logic_sharing_cut_limit"] = 1;
    param["use_dont_cares"] = true;
    compute::mig_flow_mapper( network, param, op_data, library );
    return;
  }

  if ( operation_type == "rewrite" )
  {
    compute::mig_flow_rewrite( network, param, op_data, library );
    return;
  }

  if ( operation_type == "mig_resubstitution" )
  {
    compute::mig_flow_resubstitution( network, param, op_data, library );
    return;
  }
  if ( operation_type == "resubc" )
  {
    param["max_pis"] = 8;
    param["max_divisors"] = 150;
    param["max_inserts"] = 2;
    param["skip_fanout_limit_for_roots"] = 1000;
    param["skip_fanout_limit_for_divisors"] = 1000;
    param["use_dont_cares"] = false;
    param["window_size"] = 12;
    param["preserve_depth"] = false;
    param["max_clauses"] = 1000;
    param["conflict_limit"] = 1000;
    param["random_seed"] = 1;
    param["odc_levels"] = 0;
    param["max_trials"] = 100;
    param["max_divisors_k"] = 50;
    compute::mig_flow_resubstitution( network, param, op_data, library );
    return;
  }
  if ( operation_type == "resubd" )
  {
    param["max_pis"] = 8;
    param["max_divisors"] = 150;
    param["max_inserts"] = 2;
    param["skip_fanout_limit_for_roots"] = 1000;
    param["skip_fanout_limit_for_divisors"] = 1000;
    param["use_dont_cares"] = false;
    param["window_size"] = 12;
    param["preserve_depth"] = true;
    param["max_clauses"] = 1000;
    param["conflict_limit"] = 1000;
    param["random_seed"] = 1;
    param["odc_levels"] = 0;
    param["max_trials"] = 100;
    param["max_divisors_k"] = 50;
    compute::mig_flow_resubstitution( network, param, op_data, library );
    return;
  }

  if ( operation_type == "mig_cut_rewriting" )
  {
    compute::mig_flow_cut_rewriting( network, param, op_data, library );
    return;
  }
  if ( operation_type == "cutr" )
  {
    param["cut_enumeration_ps.cut_size"] = 4;
    param["cut_enumeration_ps.cut_limit"] = 12;
    param["cut_enumeration_ps.minimize_truth_table"] = true;
    param["allow_zero_gain"] = false;
    param["use_dont_cares"] = false;
    param["candidate_selection_strategy"] = 0;
    param["min_cand_cut_size"] = 3;
    param["preserve_depth"] = false;
    compute::mig_flow_cut_rewriting( network, param, op_data, library );
    return;
  }
  if ( operation_type == "cutrz" )
  {
    param["cut_enumeration_ps.cut_size"] = 4;
    param["cut_enumeration_ps.cut_limit"] = 12;
    param["cut_enumeration_ps.minimize_truth_table"] = true;
    param["allow_zero_gain"] = true;
    param["use_dont_cares"] = false;
    param["candidate_selection_strategy"] = 0;
    param["min_cand_cut_size"] = 3;
    param["preserve_depth"] = false;
    compute::mig_flow_cut_rewriting( network, param, op_data, library );
    return;
  }

  if ( operation_type == "mig_algebraic_rewriting" )
  {
    compute::mig_flow_algebraic_rewriting( network, param, op_data, library );
    return;
  }
  if ( operation_type == "alg0" )
  {
    param["strategy"] = 0;
    compute::mig_flow_algebraic_rewriting( network, param, op_data, library );
    return;
  }
  if ( operation_type == "alg1" )
  {
    param["strategy"] = 1;
    compute::mig_flow_algebraic_rewriting( network, param, op_data, library );
    return;
  }
  if ( operation_type == "alg2" )
  {
    param["strategy"] = 2;
    compute::mig_flow_algebraic_rewriting( network, param, op_data, library );
    return;
  }

  if ( operation_type == "detrd" )
  {
    compute::mig_flow_det_randomize( network, param, op_data, library );
    return;
  }
  printf( "[i] - Warning, the operation %s doesn't exist", operation_type );
  return;
}

} // namespace mockturtle