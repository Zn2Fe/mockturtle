#include <experiments.hpp>
#include <list>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/mig_flow.hpp>
#include <mockturtle/io/aiger_reader.hpp>

#define WRITE_IN_CSV true
#define WRITE_IN_CSV_V true

struct flow
{
  mockturtle::mig_network mig;
  std::string name;
  mockturtle::compute_data op_data = mockturtle::compute_data{};
  float runtime = 0;
  int op_num = 0;
};

struct csv_verbose_data
{
  std::string path;
  std::string benchmark;
  u_int32_t size_before;
  u_int32_t depth_before;
};

std::list<flow> compute_flow( flow mig, nlohmann::json json_flow, csv_verbose_data csv_v )
{
  std::ofstream writer;
  flow mig_f = {
    mockturtle::cleanup_dangling( mig.mig ),
    mig.name,
    mig.op_data,
    mig.runtime,
    mig.op_num 
  };
  std::list<flow> result;
  for ( const auto& item : json_flow["flow"].items() )
  {
    std::string operation_type = item.value().at( "operation_type" ).get<std::string>();
    if ( operation_type == "branch" )
    {
      for ( const auto& branch : item.value().at( "flow" ).items() )
      {
        nlohmann::json branch_flow = branch.value();
        bool add = false;
        for ( const auto& newitem : json_flow["flow"].items() )
        {
          if ( add )
            branch_flow["flow"].push_back( newitem.value() );
          if ( item == newitem )
            add = true;
        }
        for ( flow res : compute_flow( mig_f, item.value(), csv_v ) )
        {
          result.push_back( res );
        }
      }
      return result;
    }
    mockturtle::compute_data op_data;
    mig_f.mig = mockturtle::compute_network( mig_f.mig, item.value(), &op_data );
    mig_f.name.append( operation_type );
    mig_f.name.append( "_" );
    mig_f.op_data = op_data;
    mig_f.runtime += op_data.runtime;
    if ( WRITE_IN_CSV_V )
    {
      writer.open( fmt::format( "{}_v.csv", csv_v.path ), std::ios::app );
      writer << fmt::format(
          "{};{};{};{};{};{};{},{};{}\n",
          csv_v.benchmark,
          mig_f.name.substr( 0, mig_f.name.length() - 1 ),
          mig_f.op_num,
          item.value().at( "operation_type" ).get<std::string>(),
          mig_f.op_data.size,
          mig_f.op_data.depth,
          mig_f.op_data.runtime,
          csv_v.size_before,
          csv_v.depth_before );
      writer.close();
    }
    mig_f.op_num += 1;
  }
  result.push_back( mig_f );
  return result;
}

int main( int argc, char* argv[] )
{
  using namespace experiments;
  using namespace nlohmann;
  using namespace mockturtle;
  // experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float /*, bool*/> exp(
  //     "mapper", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime" /*, "equivalent"*/ );

  std::string input_path = ( argc > 1 ) ? argv[1] : "/home/ndevaux/mig_flow_result/test.json";
  std::ifstream input( input_path );
  json json_flow;
  input >> json_flow;
  input.close();

  std::string name = ( argc > 2 ) ? argv[2] : "NoName";

  std::ofstream writer;

  if ( WRITE_IN_CSV )
  {
    writer.open( fmt::format( "{}.csv", argv[2] ) );
    writer << "benchmark;flow;size;size_mig;depth;depth_mig;runtime" << std::endl;
    writer.close();
  }
  if ( WRITE_IN_CSV_V )
  {
    writer.open( fmt::format( "{}_v.csv", argv[2] ) );
    writer << "benchmark;flow;op_num;op_type;size_mig;depth_mig;runtime;size;depth" << std::endl;
    writer.close();
  }

  for ( auto const& benchmark : json_flow.at( "do" ).items() )
  {
    mig_network mig;
    if ( lorina::read_aiger( benchmark_path( benchmark.value().get<std::string>() ), aiger_reader( mig ) ) != lorina::return_code::success )
    {
      continue;
    }
    u_int32_t size_before = mig.num_gates();
    u_int32_t depth_before = depth_view( mig ).depth();
    std::list<flow> migs;
    flow mig_f = { mig, "" };
    ;

    csv_verbose_data csv_v{ argv[2], benchmark.value().get<std::string>(), size_before, depth_before };
    migs = compute_flow( mig_f, json_flow, csv_v );

    for ( flow mig_f : migs )
    {
      if ( WRITE_IN_CSV )
      {
        writer.open( fmt::format( "{}.csv", argv[2] ), std::ios::app );
        writer << fmt::format(
                      "{};{};{};{};{};{};{}",
                      benchmark.value().get<std::string>(),
                      mig_f.name.substr( 0, mig_f.name.length() - 1 ),
                      size_before,
                      mig_f.op_data.size,
                      depth_before,
                      mig_f.op_data.depth,
                      mig_f.runtime )
               << std::endl;
        writer.close();
      }
    }
  }

  return 0;
}