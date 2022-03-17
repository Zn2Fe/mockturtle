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
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/networks/mig.hpp>

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
	
	namespace mig_flow_algo{

		struct mig_flow_config{
			exact_library<mig_network, mig_npn_resynthesis> exact_lib;
		};

		mig_network flow_map( mig_network mig, mig_flow_config* config, json param,float* runtime ){
			map_params map_ps = param.get<map_params>();
  			map_stats map_st;

			mig_network res = map( mig, config->exact_lib, map_ps, &map_st );
			*runtime += to_seconds( map_st.time_total );
			return res;
		}

		mig_network base_operation( int op_type, mig_network mig, mig_flow_config* config, json param,float* runtime ){
				switch (op_type)
				{
				case 201:
					return mig_flow_algo::flow_map( mig, config, param, runtime );
				default:
					break;
				}
			}
	}
	
	class operation_data{
		public:
			operation_data(json param, mig_network mig, float runtime){

			}
	};

	class operation{
		public :
			operation_data* data;
			operation* parent;

			operation(){}
			operation( operation* parent, json object, mig_network mig, float runtime ){
				this->parent = parent;
				this->data = new operation_data(object,mig,runtime);
			}
			
	};

	class end :  public operation{
		public :
			end(operation* op) : operation(){

			}
	};

	class mig_flow{
		private :
			std::list<operation*> operations;
		public: //param
			std::list<operation*> flows;
			operation root;
			mig_flow_algo::mig_flow_config* config;
			
			void compute_flow( json flow, mig_network mig, operation* parent_adr){
				operation* actual = parent_adr;
				mig_network res = mig;
				for(const auto& item : flow["flow"].items()){
					int type = item.value()["type"].get<int>();
					switch (type)
					{
					case 102: //ending
						{
							
							this->flows.push_back(new end(actual));

						}
						break;
					case 201: //mapping
						{
							float individual_runtime;
							res = mig_flow_algo::base_operation(type,res,this->config, item.value()["param"], &individual_runtime);
							actual = new operation( actual, item.value(), res, individual_runtime );
							this->operations.push_back(actual);
						}
						break;
					default:
						break;
					}
				}
			}

			mig_flow( json flow, mig_flow_algo::mig_flow_config* configuration, mig_network mig){
				this->config = configuration;
				this->root = operation();
				compute_flow(flow, mig, &(this->root));
			}

			void save_to_file(){

			}

	};


}

