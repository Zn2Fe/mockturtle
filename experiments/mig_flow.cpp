#include <experiments.hpp>
#include <list>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/mig_flow.hpp>
#include <mockturtle/io/aiger_reader.hpp>

#define CHECK_EQV true

#define WRITE_IN_CSV true
#define WRITE_IN_CSV_V true
#define VERBOSE true
#define VERY_VERBOSE true

#define BEST_SIZE true
#define BEST_DEPTH true

struct flow
{
  mockturtle::mig_network mig;
  std::string name;
  mockturtle::compute_data op_data = mockturtle::compute_data{};
  float runtime = 0;
  int op_num = 0;
};

bool compare_size( const flow& first, const flow& second )
{
  return first.op_data.size < second.op_data.size;
}
bool compare_depth( const flow& first, const flow& second )
{
  return first.op_data.depth < second.op_data.depth;
}

bool looping_condition(const flow& before, const flow& after, nlohmann::json loop){
  return before.op_data.size > after.op_data.size;
}


struct csv_verbose_data
{
  std::string path;
  std::string benchmark;
  u_int32_t size_before;
  u_int32_t depth_before;
};



std::list<flow> compute_flow( flow mig_f, nlohmann::json json_flow, csv_verbose_data csv_v )
{
  std::ofstream writer_csv_v;
  std::list<flow> result;
  for ( const auto& item : json_flow.items() )
  {
    std::string operation_type = item.value().at( "operation_type" ).get<std::string>();
    if ( VERY_VERBOSE )
    {
      fmt::print( "[i],[V] - {}   \t| {}   \t| {}{}\n", csv_v.benchmark, operation_type, mig_f.name,operation_type );
    }

    if ( operation_type == "branch" )
    {
      for ( const auto& branch : item.value().at( "branch" ).items() )
      {
        nlohmann::json branch_flow = branch.value();
        bool add = false;
        for ( const auto& newitem : json_flow.items() )
        {
          if ( add )
            branch_flow.push_back( newitem.value() );
          if ( item == newitem )
            add = true;
        }

        flow mig_b = {
            mockturtle::cleanup_dangling( mig_f.mig ),
            mig_f.name,
            mig_f.op_data,
            mig_f.runtime,
            mig_f.op_num };
        std::list<flow> compute_res = compute_flow( mig_b, branch_flow, csv_v );
        for ( flow res : compute_res )
        {
          result.push_back( res );
        }
      }
      return result;
    }

    if ( operation_type == "mark" )
    {
      result.push_back( mig_f );
      continue;
    }

    if ( operation_type == "loop" ){
      flow mig_b = {
            mockturtle::cleanup_dangling( mig_f.mig ),
            mig_f.name,
            mig_f.op_data,
            mig_f.runtime,
            mig_f.op_num };
      mig_b = compute_flow(mig_b,item.value().at("flow"),csv_v).front();
      while (looping_condition(mig_f,mig_b,item.value()))
      {
        mig_f = mig_b;
        mig_b = {
            mockturtle::cleanup_dangling( mig_f.mig ),
            mig_f.name,
            mig_f.op_data,
            mig_f.runtime,
            mig_f.op_num };
        mig_b = compute_flow(mig_b,item.value().at("flow"),csv_v).front();
      }
      continue;      
    }

    mockturtle::compute_data op_data;
    mig_f.mig = mockturtle::compute_network( mig_f.mig, item.value(), &op_data );
    if ( operation_type != "stats" )
    {
      mig_f.name.append( operation_type );
      mig_f.name.append( "_" );
    }
    mig_f.op_data = op_data;
    mig_f.runtime += op_data.runtime;
    mig_f.op_num += 1;

    if ( WRITE_IN_CSV_V )
    {
      writer_csv_v.open( fmt::format( "{}_v.csv", csv_v.path ), std::ios::app );
      writer_csv_v << fmt::format(
                          "{};{};{};{};{};{};{},{};{}",
                          csv_v.benchmark,
                          mig_f.op_num,
                          item.value().at( "operation_type" ).get<std::string>(),
                          mig_f.op_data.size,
                          mig_f.op_data.depth,
                          mig_f.op_data.runtime,
                          csv_v.size_before,
                          csv_v.depth_before,
                          mig_f.name.substr( 0, mig_f.name.length() - 1 ) )
                   << std::endl;
      writer_csv_v.close();
    }
  }
  result.push_back( mig_f );
  return result;
}

int main( int argc, char* argv[] )
{
  using namespace experiments;
  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool> exp(
      "mig_flow", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime", "equivalent" );
  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float> exp_size(
      "mig_flow", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime" );
  experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float> exp_depth(
      "mig_flow", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime" );

  if ( VERBOSE )
  {
    fmt::print( "[i] - Loading input json file\n" );
  }
  std::string input_path = ( argc > 1 ) ? argv[1] : "/home/ndevaux/mig_flow_result/test.json";
  std::ifstream input( input_path );
  nlohmann::json json_flow;
  input >> json_flow;
  input.close();
  std::string name = ( argc > 2 ) ? argv[2] : "NoName";

  std::ofstream writer;
  if ( WRITE_IN_CSV )
  {
    if ( VERBOSE )
      fmt::print( "[i] - Creating output CSV file\n" );
    writer.open( fmt::format( "{}.csv", argv[2] ) );
    writer << "benchmark;flow;size;size_mig;depth;depth_mig;runtime" << std::endl;
    writer.close();
  }
  if ( WRITE_IN_CSV_V )
  {
    if ( VERBOSE )
      fmt::print( "[i] - Creating output CSV_verbose file\n" );
    writer.open( fmt::format( "{}_v.csv", argv[2] ) );
    writer << "benchmark;op_num;op_type;size_mig;depth_mig;runtime;size;depth;flow" << std::endl;
    writer.close();
  }

  for ( auto const& benchmark_json : json_flow.at( "do" ).items() )
  {
    std::string benchmark = benchmark_json.value().get<std::string>();

    fmt::print( "[i] - Processing {}\n", benchmark );


    mockturtle::mig_network mig;
    if ( VERY_VERBOSE )
    {
      fmt::print( "[i],[V] - Loading benchmark\n" );
    }
    if ( lorina::read_aiger( benchmark_path( benchmark ), mockturtle::aiger_reader( mig ) ) != lorina::return_code::success )
    {
      continue;
    }

    if ( VERY_VERBOSE )
    {
      fmt::print( "[i],[V] - Loading data\n" );
    }
    u_int32_t size_before = mig.num_gates();
    u_int32_t depth_before = mockturtle::depth_view( mig ).depth();
    std::list<flow> migs;
    flow mig_f = { mig, "" };
    csv_verbose_data csv_v{ argv[2], benchmark, size_before, depth_before };

    if ( VERY_VERBOSE )
    {
      fmt::print( "[i],[V] - Start Computing\n" );
    }
    migs = compute_flow( mig_f, json_flow.at( "flow" ), csv_v );

    if ( VERBOSE )
    {
      fmt::print( "[i] - Checking Equivalence & saving experiments" );
    }
    for ( flow mig_f : migs )
    {
      if ( VERY_VERBOSE )
        fmt::print( "[i],[V] - Checking Equivalence for {} - {}\n", benchmark, mig_f.name.substr( 0, mig_f.name.length() - 1 ) );
      const auto cec = benchmark == "hyp" ? true : ( CHECK_EQV ) ? true
                                                                 : abc_cec( mig_f.mig, benchmark );
      if ( VERY_VERBOSE )
        fmt::print( "[i],[V] - Saving result\n" );
      exp( benchmark, mig_f.name.substr( 0, mig_f.name.length() - 1 ), size_before, mig_f.op_data.size, depth_before, mig_f.op_data.depth, mig_f.runtime, cec );
    }
    if ( BEST_SIZE )
    {
      std::list<flow> mig_copy(migs);
      mig_copy.sort( compare_size );
      u_int32_t best_size = mig_copy.front().op_data.size;
      mig_copy.remove_if([=](const flow & f){
        return f.op_data.size > best_size;
      });
      mig_copy.sort(compare_depth);
      for ( flow mig_f : mig_copy ){
        exp_size( benchmark, mig_f.name.substr( 0, mig_f.name.length() - 1 ), size_before, mig_f.op_data.size, depth_before, mig_f.op_data.depth, mig_f.runtime );
      }
    }
    if ( BEST_DEPTH )
    {
      std::list<flow> mig_copy(migs);
      mig_copy.sort( compare_depth );
      u_int32_t best_depth = mig_copy.front().op_data.depth;
      mig_copy.remove_if([=](const flow & f){
        return f.op_data.depth > best_depth;
      });
      mig_copy.sort(compare_size);
      for ( flow mig_f : mig_copy ){
        exp_depth( benchmark, mig_f.name.substr( 0, mig_f.name.length() - 1 ), size_before, mig_f.op_data.size, depth_before, mig_f.op_data.depth, mig_f.runtime );
      }
    }


    if ( WRITE_IN_CSV )
    {
      writer.open( fmt::format( "{}.csv", argv[2] ), std::ios::app );

      for ( flow mig_f : migs )
      {
        if ( VERBOSE )
          fmt::print( "[i] - Writing result of \"{}\" to CSV file\n", mig_f.name.substr( 0, mig_f.name.length() - 1 ) );
        writer << fmt::format(
            "{};{};{};{};{};{};{}\n",
            benchmark,
            mig_f.name.substr( 0, mig_f.name.length() - 1 ),
            size_before,
            mig_f.op_data.size,
            depth_before,
            mig_f.op_data.depth,
            mig_f.runtime );
      }
      writer.close();
    }
  }
  fmt::print( "\n" );
  exp.save();
  exp.table();
  if ( BEST_SIZE )
  {
    fmt::print( "\nBEST SIZE\n" );
    exp_size.save();
    exp_size.table();
  }
  if ( BEST_DEPTH )
  {
    fmt::print( "\nBEST DEPTH\n" );
    exp_depth.save();
    exp_depth.table();
  }
  fmt::print( "\n\n" );
  return 0;
}