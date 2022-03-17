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


namespace mockturtle
{
	using namespace nlohmann;

	struct flow_config{
		exact_library<mig_network, mig_npn_resynthesis> exact_lib;
	};

	namespace flow_algorithm{
		
		mig_network flow_map( mig_network mig, flow_config* config, json param,float* runtime ){
			map_params map_ps;
  			map_stats map_st;
			// TODO : param
			map_ps.skip_delay_round = true;
			map_ps.required_time = std::numeric_limits<double>::max(),
			map_ps.ela_rounds = 3;
			map_ps.enable_logic_sharing = true;
			map_ps.logic_sharing_cut_limit = 1;

			mig_network res = map( mig, config->exact_lib, map_ps, &map_st );
			*runtime += to_seconds( map_st.time_total );
			return res;
		}


	}
 	


	class mig_flow{
		public :
			class Operation{
				public:
					Operation* parent;
					int type;
					json param;
					float runtime;
					float flow_runtime;
					u_int32_t size;
					u_int32_t depth;
					Operation(mig_network mig){
						this->size = mig.num_gates();
						this->depth = depth_view(mig).depth();
					}
					Operation(Operation* op, mig_network mig, float individual_runtime,json param,int type){
						this->parent = op;	
						this->flow_runtime = op->flow_runtime + individual_runtime;
						this->runtime = individual_runtime;	

						this->size = mig.num_gates();
						this->depth = depth_view(mig).depth();
						this->param = param;
						this->type = type;
					}
			};
			class Root : public Operation{
				public:
					Root(Operation mig) : Operation(mig){
						this->type = 0;
						this->runtime = 0;
						this->flow_runtime = 0;
					}
			};
			std::list<Operation> flows;
			flow_config* config;

			Operation compute_flow(mig_flow* self, json flow, mig_network*  mig, Operation parent ){
				Operation actual = parent;

				for(const auto& item : flow["flow"].items()){
					float individual_runtime;
					switch (item.value()["type"].get<int>())
					{
					case 101: //branching
						compute_flow(self,item.value(),&cleanup_dangling(*mig),actual);
						break;
					case 102: //ending
						self->flows.push_back(actual);
						break;
					case 103://looping
						while (true){
							u_int32_t size_start_loop = mig->num_gates();
							mig_network loop_res = cleanup_dangling(*mig);
							Operation op_loop = compute_flow(self,item.value(),&loop_res,actual);
							if(loop_res.num_gates() >= size_start_loop){
								break;
							}
							*mig = loop_res;
							actual = op_loop;
						}

						break;
					case 201: //mapping
						*mig = flow_algorithm::flow_map(*mig,self->config,item.value()["param"],&individual_runtime);	
						break;
					default:
						break;
					}
					if(item.value()["type"].get<int>()%100 == 2){
						Operation next_op(&actual,*mig,individual_runtime,item.value()["param"],item.value()["type"].get<int>());
						actual = next_op;
					}
				}
			}

			mig_flow( json flow, flow_config* configuration, mig_network mig ){
				Root root(mig);
				this->config = configuration;
				compute_flow( this, flow, &mig, (Operation) root );
			}

			void save_to_file(){
				
			}

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
}/* namespace mockturtle */
