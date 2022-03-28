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

#include <list>
#include <nlohmann/json.hpp>

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/cut_rewriting.hpp>
#include <mockturtle/algorithms/functional_reduction.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/mig_algebraic_rewriting.hpp>
#include <mockturtle/algorithms/mig_resub.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/fanout_view.hpp>

/*
 *  Json serialization and deserialization for class used in mig_flow
 */
namespace mockturtle
{
using namespace nlohmann;
// map_param :
void to_json( json& j, const map_params& param )
{
  j = json{
      { "required_time", param.required_time },
      { "skip_delay_round", param.skip_delay_round },
      { "area_flow_rounds", param.area_flow_rounds },
      { "ela_rounds", param.ela_rounds },
      { "eswp_rounds", param.eswp_rounds },
      { "switching_activity_patterns", param.switching_activity_patterns },
      { "enable_logic_sharing", param.enable_logic_sharing },
      { "logic_sharing_cut_limit", param.logic_sharing_cut_limit },
      { "verbose", param.verbose } };
}

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
}

void to_json( json& j, const cut_rewriting_params& param )
{
  j = json{
      { "cut_enumeration_ps.cut_size", param.cut_enumeration_ps.cut_size },
      { "cut_enumeration_ps.cut_limit", param.cut_enumeration_ps.cut_limit },
      { "cut_enumeration_ps.minimize_truth_table", param.cut_enumeration_ps.minimize_truth_table },
      { "allow_zero_gain", param.allow_zero_gain },
      { "use_dont_cares", param.use_dont_cares },
      { "candidate_selection_strategy", param.candidate_selection_strategy },
      { "min_cand_cut_size", param.min_cand_cut_size },
      { "preserve_depth", param.preserve_depth },
      { "progress", param.progress },
      { "verbose", param.verbose },
      { "very_verbose", param.very_verbose } };
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

void to_json( json& j, const resubstitution_params& param )
{
  j = json{
      { "max_pis", param.max_pis },
      { "max_divisors", param.max_divisors },
      { "max_inserts", param.max_inserts },
      { "skip_fanout_limit_for_roots", param.skip_fanout_limit_for_roots },
      { "skip_fanout_limit_for_divisors", param.skip_fanout_limit_for_divisors },
      { "progress", param.progress },
      { "verbose", param.verbose },
      { "window_size", param.window_size },
      { "preserve_depth", param.preserve_depth },
      { "max_clauses", param.max_clauses },
      { "conflict_limit", param.conflict_limit },
      { "random_seed", param.random_seed },
      { "odc_levels", param.odc_levels },
      { "max_trials", param.max_trials },
      { "max_divisors_k", param.max_divisors_k }

  };
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
} // namespace mockturtle

namespace mockturtle
{
using namespace nlohmann;

/**
 * @brief
 *
 */
struct mig_flow_param
{
  bool progress;
};

/**
 * @brief
 *
 */
struct mig_flow_stats
{
  /*! \brief Total runtime. */
  stopwatch<>::duration time_total{ 0 };
};

/**
 * @brief
 *
 */
class mig_data
{
public:
  u_int32_t size = 0;
  u_int32_t depth = 0;

  void load_data_mig( mig_network mig )
  {
    this->size = mig.num_gates();
    this->depth = depth_view( mig ).depth();
  }

  json get_data_json()
  {
    json res;
    res["size"] = this->size;
    res["depth"] = this->depth;
    return res;
  }
};

namespace detail
{
class operation_algo_data
{
public:
  // operation param
  json param;

  // operation data
  float runtime = 0;
  map_stats flow_map_stat;
  functional_reduction_stats flow_functionnal_reduction_stat;
  cut_rewriting_stats flow_cut_rewriting_stats;
  resubstitution_stats flow_resubstitution_stats;
  mig_algebraic_depth_rewriting_stats flow_mig_algebraic_depth_rewriting_stats;

  // mig_data
  mig_data mig_stats;

  operation_algo_data()
  {
  }
  operation_algo_data( json object )
  {
    this->param = object["param"];
  }

  json get_data_json()
  {
    json res = mig_stats.get_data_json();
    res["param"] = this->param;
    res["runtime"] = this->runtime;
    return res;
  }
};

// 201
mig_network flow_map( mig_network mig, operation_algo_data* data_out )
{
  mig_npn_resynthesis resyn{ true };
  exact_library_params eps;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );
  map_params map_ps = data_out->param.get<map_params>();
  mig_network res = mig;

  res = map( res, exact_lib, map_ps, &data_out->flow_map_stat );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_map_stat.time_total );
  return res;
}
// 202
mig_network flow_functionnal_reduction( mig_network mig, operation_algo_data* data_out )
{
  functional_reduction_params ps;
  mig_network res = mig;

  functional_reduction( res, ps, &data_out->flow_functionnal_reduction_stat );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_functionnal_reduction_stat.time_total );
  return res;
}
// 203
mig_network flow_cut_rewriting_with_compatibility_graph( mig_network mig, operation_algo_data* data_out )
{
  mig_npn_resynthesis resyn{ true };
  cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
  mig_network res = mig;

  cut_rewriting_with_compatibility_graph( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
  return res;
}
// 204
mig_network flow_cut_rewriting( mig_network mig, operation_algo_data* data_out )
{
  mig_npn_resynthesis resyn{ true };
  cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
  mig_network res = mig;

  cut_rewriting( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
  return res;
}
// 205
mig_network flow_mig_resubstitution( mig_network mig, operation_algo_data* data_out )
{
  resubstitution_params ps = data_out->param.get<resubstitution_params>();
  mig_network res = mig;
  depth_view depth_mig{ res };
  fanout_view fanout_mig{ depth_mig };

  mig_resubstitution( fanout_mig, ps, &data_out->flow_resubstitution_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_resubstitution_stats.time_total );
  return res;
}
// 206
mig_network flow_mig_algebraic_rewriting( mig_network mig, operation_algo_data* data_out )
{
  mig_algebraic_depth_rewriting_params ps;
  mig_network res = mig;
  depth_view mig_depth{ res };

  mig_algebraic_depth_rewriting( mig_depth, ps, &data_out->flow_mig_algebraic_depth_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_mig_algebraic_depth_rewriting_stats.time_total );
  return res;
}

class operation
{
public:
  int type;
  operation( int type )
  {
    this->type = type;
  }
  virtual float get_flow_runtime()
  {
    return 0;
  }
  virtual json save_data_to_json()
  {
    json res;
    res["operation_type"] = type;
    return res;
  }
};

class root_operation : public operation
{
public:
  mig_data root_stat;

  root_operation( mig_network mig ) : operation( 100 )
  {
    root_stat.load_data_mig( mig );
  }

  float get_flow_runtime()
  {
    return 0;
  }
  json save_data_to_json()
  {
    json res = json::array();
    json data = this->root_stat.get_data_json();
    data["operation_type"] = 0;
    res.push_back( data );
    return res;
  }
};

class chained_algo_operation : public operation
{
public:
  operation* parent;
  operation_algo_data* operation_data;

  chained_algo_operation( operation* parent, operation_algo_data* operation_data, int type ) : operation( type )
  {
    this->operation_data = operation_data;
    this->parent = parent;
  }

  float get_flow_runtime()
  {
    float res = this->parent->get_flow_runtime();
    res += this->operation_data->runtime;
    return res;
  }
  json save_data_to_json()
  {
    json res = this->parent->save_data_to_json();
    json data = this->operation_data->get_data_json();
    data["operation_type"] = this->type;
    res.push_back( data );
    return res;
  }
};

class end_operation : public operation
{
public:
  operation* parent;
  mig_network result;
  mig_data end_stat;
  std::string name;

  end_operation( operation* parent, mig_network mig, bool loop = false ) : operation( 199 )
  {
    this->parent = parent;
    if ( not loop )
    {
      this->result = mig;
    }
    this->end_stat.load_data_mig( mig );
  }

  float get_flow_runtime()
  {
    float res = this->parent->get_flow_runtime();
    return res;
  }
  json save_data_to_json()
  {
    json res = this->parent->save_data_to_json();
    json data = this->end_stat.get_data_json();
    data["operation_type"] = 1;
    data["name"] = name;
    res.push_back( data );
    return res;
  }
};

class loop_operation : public operation
{
public:
  operation* parent;
  std::list<end_operation*> operations;

  mig_data data;
  json param;
  float runtime;

  loop_operation( operation* parent, json object ) : operation( 102 )
  {
    this->parent = parent;
    this->param = object["param"];
  }

  float get_flow_runtime()
  {
    float res = this->parent->get_flow_runtime();

    res += this->operations.size() == 0 ? 0 : this->operations.back()->get_flow_runtime();
    return res;
  }
  json save_data_to_json()
  {
    json res = this->parent->save_data_to_json();
    json data = this->data.get_data_json();
    data["operation_type"] = this->type;
    data["param"] = this->param;
    data["runtime"] = this->get_flow_runtime();
    data["flow"] = this->operations.size() == 0 ? json::array() : this->operations.back()->save_data_to_json();
    res.push_back( data );
    return res;
  }
};

mig_network compute_flow( mig_network* mig, json flow, std::list<end_operation*>* op_result, operation* root, mig_flow_param ps, mig_flow_stats* pst, bool loop = false )
{
  mig_network res = cleanup_dangling( *mig );
  operation* actual = root;

  for ( const auto& item : flow["flow"].items() )
  {

    int type_of_operation = item.value()["operation_type"].get<int>();

    if ( type_of_operation == 101 )
    { // branching
      for ( const auto& flows : item.value()["flow"].items() )
      {

        compute_flow( &res, flows.value(), op_result, new root_operation( res ), ps, pst, false );
      }
      continue;
    }

    if ( type_of_operation == 102 )
    {

      mig_network mig_buffer = res;
      loop_operation* loop = new loop_operation( actual, item.value() );
      mig_buffer = compute_flow( &res, item.value(), &loop->operations, new root_operation( res ), ps, pst, true );
      u_int32_t size_before = res.num_gates();
      while ( loop->operations.back()->end_stat.size < size_before )
      {
        res = mig_buffer;
        size_before = loop->operations.back()->end_stat.size;
        mig_buffer = compute_flow( &res, item.value(), &loop->operations, loop->operations.back(), ps, pst, true );
      }
      loop->data.load_data_mig( res );
      actual = loop;
      continue;
    }

    operation_algo_data* operation_data = new operation_algo_data( item.value() );
    switch ( type_of_operation )
    {
    case 201:
      res = flow_map( res, operation_data );
      break;
    case 202:
      res = flow_functionnal_reduction( res, operation_data );
      break;
    case 203:
      res = flow_cut_rewriting_with_compatibility_graph( res, operation_data );
      break;
    case 204:
      res = flow_cut_rewriting( res, operation_data );
      break;
    case 205:
      res = flow_mig_resubstitution( res, operation_data );
      break;
    case 206:
      res = flow_mig_algebraic_rewriting( res, operation_data );
      break;
    default:
      break;
    }
    operation_data->mig_stats.load_data_mig( res );
    actual = new chained_algo_operation( actual, operation_data, type_of_operation );
  }

  if ( not loop )
  {
    end_operation* end = new end_operation( actual, res );
    end->name = flow.at( "name" ).get<std::string>();
    op_result->push_back( end );
  }
  else
  {
    op_result->push_back( new end_operation( actual, res, true ) );
  }

  return res;
}
} // namespace detail

/**
 * @brief
 *
 */
class mig_flow_result
{
private:
  detail::end_operation* real;

public:
  mig_flow_result( detail::end_operation* real )
  {
    this->real = real;
  }

  mig_network mig()
  {
    return real->result;
  }
  mig_data data()
  {
    return real->end_stat;
  }
  json save_data_to_json()
  {
    return real->save_data_to_json();
  }
  std::string name()
  {
    return real->name;
  }
  float get_flow_runtime()
  {
    return 0;
  }
};

/**
 * @brief
 *
 * @param mig
 * @param json_flow
 * @param ps
 * @param pst
 * @return std::list<mig_flow_result*>
 */
std::list<mig_flow_result*> mig_flow_execution( mig_network mig, json json_flow, mig_flow_param const& ps = {}, mig_flow_stats* pst = new mig_flow_stats() )
{
  mig_network mig_calc = mig;
  std::list<detail::end_operation*> result;
  compute_flow( &mig_calc, json_flow, &result, new detail::root_operation( mig ), ps, pst );
  std::list<mig_flow_result*> res;
  for ( detail::end_operation* end : result )
  {
    res.push_back( new mig_flow_result( end ) );
  }
  return res;
}

} // namespace mockturtle
