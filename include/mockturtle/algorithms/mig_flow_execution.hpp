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


class mig_data
{
public:
  float runtime = 0;

  u_int32_t size = 0;
  u_int32_t depth = 0;

  void load_data_from_mig( mig_network mig )
  {
    this->size = mig.num_gates();
    this->depth = depth_view( mig ).depth();
  }

  json get_data_json()
  {
    json res;
    res["size"] = this->size;
    res["depth"] = this->depth;
    res["runtime"] = this->runtime;
    return res;
  }
};

class operation_algo_data: public mig_data
{
public:

  map_stats flow_map_stat;
  functional_reduction_stats flow_functionnal_reduction_stat;
  cut_rewriting_stats flow_cut_rewriting_stats;
  resubstitution_stats flow_resubstitution_stats;
  mig_algebraic_depth_rewriting_stats flow_mig_algebraic_depth_rewriting_stats;
};

namespace detail
{
  


namespace compute{
// 201
mig_network flow_map( mig_network mig, operation_algo_data* data_out, json param )
{
  mig_npn_resynthesis resyn{ true };
  exact_library_params eps;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );
  map_params map_ps = param.get<map_params>();
  mig_network res = mig;

  res = map( res, exact_lib, map_ps, &data_out->flow_map_stat );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_map_stat.time_total );
  return res;
}
// 202
mig_network flow_functionnal_reduction( mig_network mig, operation_algo_data* data_out, json param )
{
  functional_reduction_params ps;
  mig_network res = mig;

  functional_reduction( res, ps, &data_out->flow_functionnal_reduction_stat );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_functionnal_reduction_stat.time_total );
  return res;
}
// 203
mig_network flow_cut_rewriting_with_compatibility_graph( mig_network mig, operation_algo_data* data_out, json param)
{
  mig_npn_resynthesis resyn{ true };
  cut_rewriting_params ps = param.get<cut_rewriting_params>();
  mig_network res = mig;

  cut_rewriting_with_compatibility_graph( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
  return res;
}
// 204
mig_network flow_cut_rewriting( mig_network mig, operation_algo_data* data_out, json param )
{
  mig_npn_resynthesis resyn{ true };
  cut_rewriting_params ps = param.get<cut_rewriting_params>();
  mig_network res = mig;

  cut_rewriting( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
  return res;
}
// 205
mig_network flow_mig_resubstitution( mig_network mig, operation_algo_data* data_out, json param  )
{
  resubstitution_params ps = param.get<resubstitution_params>();
  mig_network res = mig;
  depth_view depth_mig{ res };
  fanout_view fanout_mig{ depth_mig };

  mig_resubstitution( fanout_mig, ps, &data_out->flow_resubstitution_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_resubstitution_stats.time_total );
  return res;
}
// 206
mig_network flow_mig_algebraic_rewriting( mig_network mig, operation_algo_data* data_out, json param )
{
  mig_algebraic_depth_rewriting_params ps;
  mig_network res = mig;
  depth_view mig_depth{ res };

  mig_algebraic_depth_rewriting( mig_depth, ps, &data_out->flow_mig_algebraic_depth_rewriting_stats );
  res = cleanup_dangling( res );

  data_out->runtime = to_seconds( data_out->flow_mig_algebraic_depth_rewriting_stats.time_total );
  return res;
}
}


class operation
{
public:
  int operation_type = -1;
  json param;
  mig_data stats;

  virtual float get_flow_runtime()
  {
    return this->stats.runtime;
  }
  json data(){
    json res = this->stats.get_data_json();
    res["operation_type"] = operation_type;
    res["param"] = param;
    return res;
  }

  virtual json save_data_to_json()
  {
    return this->data();
  }
};

class root_operation : public operation
{
public:

  root_operation( mig_network mig, json param )
  {
    this->stats.load_data_from_mig( mig );
    this->param = param;
    this->operation_type = 0;
  }

  json save_data_to_json()
  {
    json res = json::array();
    json data = this->data();
    res.push_back( data );
    return res;
  }
};

class chained_algo_operation : public operation
{
public:
  operation* parent;

  chained_algo_operation( operation* parent, operation_algo_data* operation_data, json param, int type )
  {
    this->parent = parent;
    this->stats = *operation_data;
    this->param = param;
    this->operation_type = type;
    
  }

  float get_flow_runtime()
  {
    float res = this->parent->get_flow_runtime();
    res += this->stats.runtime;
    return res;
  }

  json save_data_to_json()
  {
    json res = this->parent->save_data_to_json();
    json data = this->data();
    res.push_back( data );
    return res;
  }
};

class end_operation : public operation
{
public:
  operation* parent;
  mig_network result;
  std::string name;

  end_operation( operation* parent, mig_network mig, std::string name, bool loop = false )
  {
    this->parent = parent;
    this->stats.load_data_from_mig(mig);
    this->operation_type = 1;
    this->name = name;
    if ( not loop )
    {
      this->result = mig;
    }
  }

  float get_flow_runtime()
  {
    float res = this->parent->get_flow_runtime();
    return res;
  }

  json save_data_to_json()
  {
    json res = this->parent->save_data_to_json();
    json data = this->data();
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
private:
  bool size_end(){
    if (this->operations.size()<2){
      return this->operations.back()->stats.size <= this->parent->stats.size;
    }
    return this->operations.back()->stats.size <= (--this->operations.back())->stats.size;
  }
  bool max_rep(){
    return this->param.at("max_rep").get<int>() >= this->operations.size();
  }

public:

  loop_operation( operation* parent, json param )
  {
    this->parent = parent;
    this->operation_type = 3;
    this->param = param;
  }

  bool check_end(bool in){
    std::string end_condition;
    try
    {
      end_condition = this->param.at( "end_condition" ).get<std::string>();
    }
      catch ( const json::exception& )
    {
      return in;
    }
    if(end_condition == "no_size_increase"){
      return in;
    }
    if(end_condition == "max_rep"){
      return this->max_rep();
    }
    
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
    json data = this->data();
    data["flow"] = this->operations.size() == 0 ? json::array() : this->operations.back()->save_data_to_json();
    res.push_back( data );
    return res;
  }
};


mig_network compute_flow( mig_network mig, json flow, std::list<end_operation*>* op_result, operation* root, mig_flow_param ps, mig_flow_stats* pst, bool loop = false )
{
  mig_network res = mig;
  
  operation* actual = root;

  for ( const auto& item : flow["flow"].items() )
  {

    int type_of_operation = item.value()["operation_type"].get<int>();

    if ( type_of_operation == 2 )
    { // branching
      for ( const auto& flows : item.value()["flow"].items() )
      {
        compute_flow( res, flows.value(), op_result, actual, ps, pst, false );
      }
      continue;
    }

    if ( type_of_operation == 3 )
    {

      mig_network mig_buffer = res;
      loop_operation* loop = new loop_operation( actual, item.value()["param"] );
      mig_buffer = compute_flow( res, item.value(), &loop->operations, new root_operation( res, loop->param ), ps, pst, true );
      u_int32_t size_before = res.num_gates();
      while ( loop->check_end(loop->operations.back()->stats.size < size_before))
      {
        res = mig_buffer;
        size_before = loop->operations.back()->stats.size;
        mig_buffer = compute_flow( res, item.value(), &loop->operations, loop->operations.back(), ps, pst, true );
      }
      loop->stats.load_data_from_mig( res );
      actual = loop;
      continue;
    }

    operation_algo_data* operation_data = new operation_algo_data();
    switch ( type_of_operation )
    {
    case 101:
      res = compute::flow_map( res, operation_data, item.value()["param"] );
      break;
    case 102:
      res = compute::flow_functionnal_reduction( res, operation_data, item.value()["param"] );
      break;
    case 103:
      res = compute::flow_cut_rewriting_with_compatibility_graph( res, operation_data, item.value()["param"] );
      break;
    case 104:
      res = compute::flow_cut_rewriting( res, operation_data, item.value()["param"] );
      break;
    case 105:
      res = compute::flow_mig_resubstitution( res, operation_data, item.value()["param"] );
      break;
    case 106:
      res = compute::flow_mig_algebraic_rewriting( res, operation_data, item.value()["param"] );
      break;
    default:
      break;
    }
    chained_algo_operation* op = new chained_algo_operation( actual, operation_data, item.value()["param"],type_of_operation);
    op->stats.load_data_from_mig(res);
    actual = op;
  }

  if ( not loop )
  {
    end_operation* end = new end_operation( actual, res,flow.at( "name" ).get<std::string>() );
    op_result->push_back( end );
  }
  else
  {
    std::string loop_name = "loop";
    end_operation* end = new end_operation( actual, res,loop_name,true );
    op_result->push_back( end );
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
    return real->stats;
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
  detail::root_operation* root = new detail::root_operation(mig,json_flow["param"]);
  compute_flow( mig_calc, json_flow, &result, root , ps, pst );
  
  std::list<mig_flow_result*> res;
  for ( detail::end_operation* end : result )
  {
    res.push_back( new mig_flow_result( end ) );
  }

  return res;
}

} // namespace mockturtle
