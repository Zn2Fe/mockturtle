#include <string>
#include <vector>

#include <fmt/format.h>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/node_resynthesis.hpp>
#include <mockturtle/algorithms/node_resynthesis/mig_npn.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <mockturtle/algorithms/mig_flow_execution.hpp>
#include <experiments.hpp>

#include <nlohmann/json.hpp>


/*namespace mockturtle{
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

  	void compute_flow(mig_network* mig, json flow, std::list<end_operation*>* op_result ){
		mig_network res = *mig;
		operation* actual = new root_operation(res);

		for(const auto& item : flow["flow"].items()){
			if(item.value()["type"].get<int>() == 101){ //branching
				mig_network copy = cleanup_dangling(res);
				compute_flow(&copy, item.value(), op_result);
				continue;
			}
			operation_algo_data* operation_data = new operation_algo_data(item.value());
			switch (item.value()["type"].get<int>())
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
			actual = new chained_algo_operation(actual, operation_data);
		}
		op_result->push_back(new end_operation( actual, res ));
	}

}*/

int main()
{
	using namespace experiments;
  	using namespace mockturtle;

  	experiment<std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool> exp(
		"mapper", "benchmark" ,"size", "size_mig", "depth", "depth_mig", "runtime1", "equivalent" );
	
	fmt::print( "[i] processing technology library\n" );
	
	json json_result = json::array();
  	float total_runtime = 0;
	stopwatch<> ::duration time{0};
  	{
	stopwatch t( time );
		for ( auto const& benchmark : epfl_benchmarks())
		{
			json json_loop_result;
			json_loop_result["benchmark"]=benchmark;
			json_loop_result["flows"]= json::array();

			fmt::print( "[i] processing {}\n", benchmark );
			mig_network mig;

			if ( lorina::read_aiger( benchmark_path( benchmark ), aiger_reader( mig ) ) != lorina::return_code::success ){continue;}

			std::ifstream i( fmt::format( "{}/mig_flow/config.json", EXPERIMENTS_PATH ) );
			json json_flow;
			i >> json_flow;

			u_int32_t size_before = mig.num_gates();
			u_int32_t depth_before = depth_view(mig).depth();

			std::list<end_operation*> result; 
			compute_flow( &mig, json_flow, &result, new root_operation(mig) );

			for( end_operation* res_op : result )
			{
				const auto cec = benchmark == "hyp" ? true : abc_cec( res_op->result, benchmark );
				exp(benchmark,size_before,res_op->end_stat.size,depth_before,res_op->end_stat.depth,res_op->get_flow_runtime(),cec);
				total_runtime += res_op->get_flow_runtime();
				json_loop_result["flows"].push_back(res_op->save_data_to_json());
			}
			json_result.push_back(json_loop_result);
		}
  	std::ofstream o(fmt::format("{}/mig_flow/result/result2.json",EXPERIMENTS_PATH));
  	o << std::setw(4) << json_result << std::endl;
	}
	exp.save();
	exp.table();
	fmt::print("\nTotal runtime : {}",total_runtime);
	std::cout << fmt::format( "{:5.2f} seconds passed\n", to_seconds( time ) );
  	return 0;
}