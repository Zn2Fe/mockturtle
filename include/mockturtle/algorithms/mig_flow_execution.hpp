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
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/fanout_view.hpp>

/*
 *  Json serialization and deserialization for class used in mig_flow
 */
namespace mockturtle{
	using namespace nlohmann; 
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		map_params,
		required_time,
		skip_delay_round,
		//area_flow_rounds,
		ela_rounds,
		//eswp_rounds,
		//switching_activity_patterns,
		enable_logic_sharing,
		logic_sharing_cut_limit //,
		//verbose 
	)
	
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		cut_rewriting_params,
		cut_enumeration_ps.cut_size,
		progress
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		resubstitution_params,
		max_pis,
		max_inserts,
		progress
	)

}


namespace mockturtle{
	using namespace nlohmann;

	class mig_data{
		public:
			u_int32_t size = 0;
			u_int32_t depth = 0;

			void load_data_mig( mig_network mig){
				this->size = mig.num_gates();
				this->depth = depth_view(mig).depth();
			}

			json get_data_json(){
				json res;
				res["size"] = this->size;
				res["depth"]= this->depth;
				return res;
			}
	};

	class operation_algo_data{
			public:
				//operation param
				json param;

				//operation data
				float runtime = 0;
				map_stats flow_map_stat;
				functional_reduction_stats flow_functionnal_reduction_stat;
				cut_rewriting_stats flow_cut_rewriting_stats;
				resubstitution_stats flow_resubstitution_stats;
				mig_algebraic_depth_rewriting_stats flow_mig_algebraic_depth_rewriting_stats;

				//mig_data
				mig_data mig_stats;

				operation_algo_data(){
				}
				operation_algo_data(json object){
					this->param = object["param"];
				}

				json get_data_json(){
					json res = mig_stats.get_data_json();
					res["param"]=this->param;
					res["runtime"] = this->runtime;
					return res;
				}
			};

	mig_network flow_map( mig_network mig, operation_algo_data* data_out){
		mig_npn_resynthesis resyn{ true };
		exact_library_params eps;
		exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );
		map_params map_ps = data_out->param.get<map_params>();
		mig_network res = mig;

		res = map( res, exact_lib, map_ps, &data_out->flow_map_stat );
		res = cleanup_dangling(res);

		data_out->runtime = to_seconds( data_out->flow_map_stat.time_total );
		return res;
	}

	mig_network flow_functionnal_reduction(mig_network mig, operation_algo_data* data_out){
		functional_reduction_params ps;
		mig_network res = mig;

		functional_reduction( res, ps, &data_out->flow_functionnal_reduction_stat );
		res = cleanup_dangling(res);

		data_out->runtime = to_seconds( data_out->flow_functionnal_reduction_stat.time_total );
		return res;

	}

	/**
	 * @brief doesn't work 
	 * 
	 * @param mig 
	 * @param data_out 
	 * @return mig_network 
	 */
	mig_network flow_cut_rewriting_with_compatibility_graph(mig_network mig, operation_algo_data* data_out){
		xag_npn_resynthesis<mig_network> resyn;
		cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
		mig_network res = mig;

		cut_rewriting_with_compatibility_graph( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
		res = cleanup_dangling(res);

		data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
		return res;
	}

	mig_network flow_cut_rewriting(mig_network mig, operation_algo_data* data_out){
		xag_npn_resynthesis<mig_network> resyn;
		cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
		mig_network res = mig;

		cut_rewriting( res, resyn, ps, &data_out->flow_cut_rewriting_stats );
		res = cleanup_dangling(res);

		data_out->runtime = to_seconds( data_out->flow_cut_rewriting_stats.time_total );
		return res;
	}

	/**
	 * @brief doesn't work 
	 * 
	 * @param mig 
	 * @param data_out 
	 * @return mig_network 
	 */
	mig_network flow_mig_resubstitution(mig_network mig, operation_algo_data* data_out){
		resubstitution_params ps = data_out->param.get<resubstitution_params>();
		mig_network res = mig;
		depth_view depth_mig{res};
		fanout_view fanout_mig{depth_mig};

		mig_resubstitution( fanout_mig, ps, &data_out->flow_resubstitution_stats );
		res = cleanup_dangling(res);

		data_out->runtime = to_seconds( data_out->flow_resubstitution_stats.time_total );
		return res;
	}

	mig_network flow_mig_algebraic_rewriting(mig_network mig, operation_algo_data* data_out){
			mig_algebraic_depth_rewriting_params ps;
			mig_network res = mig;
			depth_view mig_depth{res};

			mig_algebraic_depth_rewriting( mig_depth, ps, &data_out->flow_mig_algebraic_depth_rewriting_stats );
    		res = cleanup_dangling(res);

			data_out->runtime = to_seconds( data_out->flow_mig_algebraic_depth_rewriting_stats.time_total );
			return res;
		}

	class operation{
		public :
			int type;
			operation(int type){
				this->type = type;
			}
			virtual float get_flow_runtime(){
				return 0;
			}
			virtual json save_data_to_json(){
				json res;
				res["type"]=type;
				return res;
			}
	};

	class root_operation: public operation{
		public :
			mig_data root_stat;

			root_operation(mig_network mig):operation(100){
				root_stat.load_data_mig(mig);
			}

			float get_flow_runtime(){
				return 0;
			}
			json save_data_to_json(){
				json res = json::array();
				res.push_back(this->root_stat.get_data_json());
				return res;
			}

	};

	class chained_algo_operation: public operation{
		public :
			operation* parent;
			operation_algo_data* operation_data;

			chained_algo_operation(operation* parent, operation_algo_data* operation_data, int type ):operation(type){
				this->operation_data = operation_data;
				this->parent = parent;
			}

			float get_flow_runtime(){
				float res = this->parent->get_flow_runtime();
				res += this->operation_data->runtime;
				return res;
			}
			json save_data_to_json(){
				json res = this->parent->save_data_to_json();
				json data = this->operation_data->get_data_json();
				data["type"] = this->type;
				res.push_back(data);
				return res;
			}
	};

	class end_operation: public operation{
		public :
			operation* parent;
			mig_network result;
			mig_data end_stat;

			end_operation( operation* parent, mig_network mig, bool loop = false):operation(199){
				this->parent = parent;
				if(not loop){
					this->result = mig;	
				}
				this->end_stat.load_data_mig(mig);
			}

			float get_flow_runtime(){
				float res = this->parent->get_flow_runtime();
				return res;
			}
			json save_data_to_json(){
				json res = this->parent->save_data_to_json();
				res.push_back(this->end_stat.get_data_json());
				return res;
			}
	};

	class loop_operation: public operation{
		public :
			operation* parent;
			std::list<end_operation*> operations;
			mig_data data;

			loop_operation(operation* parent):operation(102){
				this->parent = parent;
			}
			virtual float get_flow_runtime(){
				float res = this->parent->get_flow_runtime();
				
				res += this->operations.size() == 0 ? 0 : this->operations.back()->get_flow_runtime();
				return res;
			}
			json save_data_to_json(){
				json res = this->parent->save_data_to_json();
				json data = this->data.get_data_json();
				data["type"] = this->type;
				data["flow"] = this->operations.size() == 0 ? json::array():this->operations.back()->save_data_to_json();
				res.push_back(data);
				return res;
			}
	};
	
	mig_network compute_flow(mig_network* mig, json flow, std::list<end_operation*>* op_result, operation* root , bool loop = false){
		mig_network res = *mig;
		operation* actual = root;

		for(const auto& item : flow["flow"].items()){
			int type_of_operation = item.value()["type"].get<int>();
			if( type_of_operation == 101){ //branching
				mig_network copy = cleanup_dangling(res);
				compute_flow(&copy, item.value(), op_result, new root_operation(copy));
				continue;
			}
			if(type_of_operation ==102){
				mig_network mig_buffer = res;
				loop_operation* loop = new loop_operation(actual); 
				mig_buffer = compute_flow( &res, item.value(), &loop->operations, new root_operation(res), true);
				u_int32_t size_before = res.num_gates();
				while (loop->operations.back()->end_stat.size < size_before)
				{
					res = mig_buffer;
					size_before = loop->operations.back()->end_stat.size;
					mig_buffer = compute_flow( &res, item.value(), &loop->operations, loop->operations.back(), true);
				}

				loop->operations.pop_back();
				loop->data.load_data_mig(res);
				actual = loop;
				continue;
			}
			operation_algo_data* operation_data = new operation_algo_data(item.value());
			switch (type_of_operation)
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
			operation_data->mig_stats.load_data_mig(res);
			actual = new chained_algo_operation( actual, operation_data, type_of_operation );
			actual->type = type_of_operation;
		}
		if(not loop){
			op_result->push_back(new end_operation( actual, res ));
		}else{
			op_result->push_back(new end_operation( actual, res, true ));
		}
		return res;
	}
	
}
