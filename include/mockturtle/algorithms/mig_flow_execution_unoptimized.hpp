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
#include <mockturtle/algorithms/functional_reduction.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/cut_rewriting.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/networks/mig.hpp>

/*
 *class operation{
		public :
			std::string type;
			operation_data* data;
			operation* parent;
			operation(){}
			operation( std::string type, operation_data* data_load ){
				this->data = data_load;
			}
			operation( std::string type, operation* parent, operation_data* data_load ){
				this->parent = parent;
				this->data = data_load;
			}

			virtual json save_to_json(){
				json res = this->parent->save_to_json();
				json data_json = this->data->get_data_json();
				data_json["type"]=type;
				res.push_back(data_json);
				return res;
			}
			virtual float get_flow_runtime(){
				float flow_runtime = this->parent->get_flow_runtime();
				flow_runtime += this->data->runtime;
				return flow_runtime;
			}
	};

	class root : public operation{
		public :
		root(operation_data* data):operation( "root", data){

		}
			json save_to_json(){
				json res = json::array();
				json data_json = this->data->get_data_json();
				data_json["type"]= this->type;
				res.push_back(data_json);
				return res;
			}
			float get_flow_runtime(){
				float flow_runtime = 0;
				return flow_runtime;
			}
	};

	class loop : public operation{
		public:
			std::list<operation*> operations;
			mig_network compute_loop(json flow, mig_network mig, operation* parent, mig_flow_algo::mig_flow_config* config){
				mig_network res = mig;

				operation_data* root_data = new operation_data(flow);
				root_data->runtime = parent->data->runtime;
				root_data->load_data_mig(mig,true);
				operation* actual = new root(root_data);

				while (true)
				{
					u_int32_t size_before = res.num_gates();
					mig_network mig_buffer = cleanup_dangling(res);
					std::list<operation*> loop_op_buffer;

					for(const auto& item : flow["flow"].items()){
						int type = item.value()["type"].get<int>();
						switch (type)
						{
							case 101: //branching
							case 102: //ending
								//raise error 
								break;
							case 103: //looping 
								{
									loop* loop_operation = new loop();
									mig_buffer = loop_operation->compute_loop(item.value(),res,actual,config);
									actual = loop_operation;
									loop_op_buffer.push_back(actual);
								}
								break;
							case 201:
							case 202:
							case 203:
							case 204:
								{
									operation_data* data = new operation_data(item.value());
									mig_buffer = mig_flow_algo::base_operation( type, res, config, data );
									bool cec = true; //IMPLEMENT EQUIVALENCE TO this->origin
									data->load_data_mig(mig_buffer,cec);
									actual = new operation( "operation", actual, data );

									loop_op_buffer.push_back(actual);
								}
								break;
							default:
								break;
						}
					}
					
					if(mig_buffer.num_gates() >= size_before){
						break;
					}
					res = mig_buffer;
					this->operations.merge(loop_op_buffer);
				}
				

				operation_data* data_loop = new operation_data(flow);
				bool cec = true;
				data_loop->load_data_mig(res,cec);
				data_loop->runtime = this->operations.back()->get_flow_runtime();
				this->parent = parent;
				this->data = data_loop;
				this->type = "loop";
				return res;
			}

			json save_to_json(){
				json res = this->parent->save_to_json();
				json loop = this->data->get_data_json();
				loop["flow"] = this->operations.back()->save_to_json();
				loop["type"]=type;
				res.push_back(loop);
				return res;
			}

			float get_flow_runtime(){
				float flow_runtime = this->parent->get_flow_runtime();
				flow_runtime += this->data->runtime;
				return flow_runtime;
			}
	};

	class mig_flow{
		public :
			std::list<operation*> operations;
			std::list<operation*> flows;

			mig_network origin;
			root* op_root;

			mig_flow_algo::mig_flow_config* config;
			
			mig_network compute_flow( json flow, mig_network mig, operation* parent_adr){
				operation* actual = parent_adr;
				mig_network res = mig;

				for(const auto& item : flow["flow"].items()){
					int type = item.value()["type"].get<int>();
					mig_network mig_buffer;
					switch (type)
					{
					case 101: //branching
						mig_buffer = cleanup_dangling(res);
						compute_flow( item.value(), mig_buffer, actual );
						break;
					case 102: //ending
						{
							operation_data* data = new operation_data(item.value());
							res = cleanup_dangling(res);
							bool cec = true;
							data->load_data_mig( res, cec);
							actual = new operation( "end", actual, data );
							this->flows.push_back(actual);
						}
						break;
					case 103: //looping 
						{
							loop* loop_operation = new loop();
							res = loop_operation->compute_loop(item.value(),res,actual,this->config);
							actual = loop_operation;
							this->operations.push_back(actual);
						}
						break;
					case 201: //mapping
					case 202:
					case 203:
					case 204:
						{
							operation_data* data = new operation_data(item.value());
							res = mig_flow_algo::base_operation( type, res, this->config, data );
							bool cec = true; //IMPLEMENT EQUIVALENCE TO this->origin
							data->load_data_mig(res,cec);
							actual = new operation( "operation", actual, data );
							this->operations.push_back(actual);
						}
						break;
					default:
						break;
					}
				}
				return res;
			}

			mig_flow(json flow, mig_flow_algo::mig_flow_config* configuration, mig_network mig ){
				this->config = configuration;
				float init_runtime = 0;
				operation_data* root_data = new operation_data(flow);
				root_data->load_data_mig(mig,true);
				this->op_root = new root( root_data);
				this->origin = cleanup_dangling(mig);
				compute_flow(flow, mig, this->op_root);
			}


			json save_to_json_flow(){
				json res = json::array();
				for(const auto& item : this->flows){
					res.push_back(item->save_to_json());
				}
				return res;
			}

	}; 
 * 
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
}

namespace mockturtle{
	using namespace nlohmann;
	
	class operation_data{
		public:
			json param;
			u_int32_t size;
			u_int32_t depth;
			float runtime;

			operation_data(json object){
				this->param = object["param"];
				this->runtime = 0;
			}
			void load_data_mig( mig_network mig){
				this->size = mig.num_gates();
				this->depth = depth_view(mig).depth();
			}
			json get_data_json(){
				json res;
				res["param"]=this->param;
				res["size"] = this->size;
				res["depth"]= this->depth;
				res["runtime"] = this->runtime;
				return res;
			}
	};

	namespace mig_flow_algo{


		mig_network flow_map( mig_network mig,  operation_data* data_out){
			mig_npn_resynthesis resyn{ true };
			exact_library_params eps;
			exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );

			map_params map_ps = data_out->param.get<map_params>();
  			map_stats map_st;

			mig_network res = map( mig, exact_lib, map_ps, &map_st );
			data_out->runtime = to_seconds( map_st.time_total );
			return res;
		}

		mig_network flow_functionnal_reduction(mig_network mig, operation_data* data_out){
			functional_reduction_params ps;
    		functional_reduction_stats st;

			mig_network res = mig;
			functional_reduction( res, ps, &st );
			data_out->runtime = to_seconds(st.time_total);
			return res;

		}

		mig_network flow_cut_rewriting_with_compatibility_graph(mig_network mig, operation_data* data_out){
			xag_npn_resynthesis<mig_network> resyn;
			cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
			cut_rewriting_stats st;

			mig_network res = mig;

			cut_rewriting_with_compatibility_graph( res, resyn, ps, &st);

			data_out->runtime = to_seconds(st.time_total);
			return res;
		}

		mig_network flow_cut_rewriting(mig_network mig, operation_data* data_out){
			xag_npn_resynthesis<mig_network> resyn;
			cut_rewriting_params ps = data_out->param.get<cut_rewriting_params>();
			cut_rewriting_stats st;

			mig_network res = mig;

			cut_rewriting( res, resyn, ps, &st);

			data_out->runtime = to_seconds(st.time_total);
			return res;
		}

		mig_network base_operation( int op_type, mig_network mig, operation_data* data_out){
			switch (op_type)
				{
				case 201:
					return flow_map( mig, data_out );
				case 202:
					return flow_functionnal_reduction( mig, data_out );
				case 203:
					return flow_cut_rewriting_with_compatibility_graph( mig, data_out );
				case 204:
					return flow_cut_rewriting( mig, data_out );
				default:
					break;
				}
			}
	}
	
	class operation{
		public :
			std::string type;
			operation_data* data;
			operation* parent;

			operation(){}
			operation( std::string type, operation* parent, operation_data* data_load ){
				this->parent = parent;
				this->data = data_load;
			}

			virtual json save_to_json(){
				json res = this->parent->save_to_json();
				json data_json = this->data->get_data_json();
				data_json["type"]=type;
				res.push_back(data_json);
				return res;
			}
			virtual float get_flow_runtime(){
				float flow_runtime = this->parent->get_flow_runtime();
				flow_runtime += this->data->runtime;
				return flow_runtime;
			}
	};

	class root : public operation{
		public :
            root(operation_data* data):operation(){
                this->type = "root";
                this->data = data;

            }
			json save_to_json(){
				json res = json::array();
				json data_json = this->data->get_data_json();
				data_json["type"]= this->type;
				res.push_back(data_json);
				return res;
			}
			float get_flow_runtime(){
				float flow_runtime = 0;
				return flow_runtime;
			}
	};

	class loop : public operation{
		public:
			std::list<operation*> operations;

			loop( operation* parent ):operation(){
				this->parent = parent;
			}

			json save_to_json(){
				json res = this->parent->save_to_json();
				json loop = this->data->get_data_json();
				loop["flow"] = this->operations.back()->save_to_json();
				loop["type"]=type;
				res.push_back(loop);
				return res;
			}

			float get_flow_runtime(){
				float flow_runtime = this->parent->get_flow_runtime();
				flow_runtime += this->data->runtime;
				return flow_runtime;
			}
	};

	class mig_flow{
		public :
			std::list<operation*> operations;

			mig_network result;

			mig_network compute_flow( json flow, mig_network mig, operation* parent_adr, std::list<operation*>* list ){
				operation* actual = parent_adr;
				mig_network res = mig;

				for(const auto& item : flow["flow"].items()){
					int type = item.value()["type"].get<int>();
					mig_network mig_buffer;
                    operation_data* data = new operation_data(item.value());
					switch (type)
					{
					case 102: //ending
                        res = cleanup_dangling(res);
                        data->load_data_mig( res );
                        actual = new operation( "end", actual, data );
                        (*list).push_back(actual);
						break;
					case 103: //looping 
						{
							loop* loop_operation = new loop(actual);
							res = cleanup_dangling(compute_flow( item.value(), res, actual, &loop_operation->operations));
                            data->load_data_mig( res );
							actual = loop_operation;
							(*list).push_back(actual);
						}
						break;
					case 201: //mapping
					case 202:
					case 203:
					case 204:
                        res = cleanup_dangling( mig_flow_algo::base_operation( type, res, data ));
                        data->load_data_mig( res );
                        actual = new operation( "operation", actual, data );
                        (*list).push_back(actual);
						break;
					default:
						break;
					}
				}
				return res;
			}

			mig_flow(json flow, mig_network mig ){
                operation_data* data = new operation_data(flow);
                data->load_data_mig(mig);
                data->runtime = 0;
                root* r = new root(data);
                this->operations.push_back(r);
                result = compute_flow( flow, mig, r, &this->operations );
			}

	};
}
