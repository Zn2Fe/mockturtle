#include <experiments.hpp>
#include <list>
#include <lorina/aiger.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/mig_flow.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mutex>
#include <thread>

#define CHECK_EQV false // doesn't work for parralel flow (multiple ref to same file)

#define WRITE_IN_CSV true
#define WRITE_IN_CSV_VERBOSE true

#define VERBOSE true
#define VERY_VERBOSE true

using namespace mockturtle;
using experiment_t = experiments::experiment<std::string, std::string, uint32_t, uint32_t, uint32_t, uint32_t, float, bool>;

struct const_flow_data
{
  std::string benchmark;
  u_int32_t size_before;
  u_int32_t depth_before;
  std::string csv_verbose;
};

struct flow
{
  mockturtle::mig_network mig;
  std::string name;
  mockturtle::compute_data op_data = mockturtle::compute_data{};
  float runtime = 0;
  nlohmann::json json_flow = NULL;
  const_flow_data origin_data = const_flow_data{ "", 0, 0, "" };
  bool cec = false;
};

experiment_t exp_res( "mig_flow", "benchmark", "flow", "size", "size_mig", "depth", "depth_mig", "runtime", "equivalent" );
std::list<flow> flow_to_do;

#pragma region mutex
std::deque<std::atomic<bool>> state;
std::mutex exp_mutex;
#pragma enregion

bool looping_condition( const flow& before, const flow& after, nlohmann::json loop )
{
  if(loop.at("opti")=="size"){
      return before.op_data.size > after.op_data.size;
  }
  if(loop.at("opti")=="depth"){
      return before.op_data.depth > after.op_data.depth;
  }
  return before.op_data.size > after.op_data.size;
}

std::list<flow> compute_flow( flow& mig_f, std::list<flow>& flow_branching, mockturtle::libraries& compute_libraries )
{
  std::list<flow> result;
  for ( const auto& item : mig_f.json_flow.items() )
  {
    std::string operation_type = item.value().at( "operation_type" ).get<std::string>();
    if ( VERY_VERBOSE )
    {
      fmt::print( "[i] - {}   \t| {}   \t| {}{}\n", mig_f.origin_data.benchmark, operation_type, mig_f.name, operation_type );
    }

    if ( operation_type == "branch" )
    {
      for ( const auto& branch : item.value().at( "branch" ).items() )
      {
        nlohmann::json branch_flow = branch.value();
        bool add = false;
        for ( const auto& newitem : mig_f.json_flow.items() )
        {
          if ( add )
            branch_flow.push_back( newitem.value() );
          if ( item == newitem )
            add = true;
        }
        flow mig_b{ mig_f };
        mockturtle::compute_data new_data{ mig_f.op_data };
        mig_b.op_data = new_data;
        mig_b.mig = mockturtle::cleanup_dangling( mig_f.mig );
        mig_b.json_flow = branch_flow;
        flow_branching.push_back( mig_b );
      }
      return result;
    }

    if ( operation_type == "mark" )
    {
      result.push_back( mig_f );
      continue;
    }

    if ( operation_type == "loop" )
    {
      std::list<flow> ignore;
      flow mig_before;
      flow mig_after{ mig_f };
      mig_after.json_flow = item.value().at( "loop" );
      do
      {
        mig_before = mig_after;
        mig_after.mig = mockturtle::cleanup_dangling( mig_after.mig );
        mig_after = compute_flow( mig_after, ignore, compute_libraries ).back();
      } while ( looping_condition( mig_before, mig_after, item.value() ) );
      mig_f.mig = mockturtle::cleanup_dangling( mig_before.mig );
      mig_f.name = mig_before.name;
      mig_f.op_data = mig_before.op_data;
      mig_f.runtime = mig_before.runtime;
      continue;
    }

    mockturtle::compute_data op_data;
    mockturtle::compute_network( mig_f.mig, item.value(), &op_data, compute_libraries );

    if ( WRITE_IN_CSV_VERBOSE )
    {
      std::ofstream writer;
      writer.open( fmt::format( "{}_VERBOSE.csv", mig_f.origin_data.csv_verbose ), std::ios::app );
      if ( !writer.is_open() )
      {
        sleep( 0.1 );
        writer.open( fmt::format( "{}_VERBOSE.csv", mig_f.origin_data.csv_verbose ), std::ios::app );
      }
      writer << fmt::format(
          "{};{};{};{};{};{};{};{}\n",
          mig_f.origin_data.benchmark,
          mig_f.name.substr( 0, mig_f.name.length() - 1 ),
          operation_type,
          mig_f.op_data.size,
          op_data.size,
          mig_f.op_data.depth,
          op_data.depth,
          op_data.runtime );
      writer.close();
    }

    mig_f.name.append( operation_type );
    mig_f.name.append( "_" );
    mig_f.op_data = op_data;
    mig_f.runtime += op_data.runtime;
  }
  result.push_back( mig_f );
  return result;
}

bool check_state()
{
  for ( const auto& current_state : state )
  {
    if ( current_state.load() )
      return true;
  }
  return false;
}

void thread_run( int thread_id, std::string path_csv )
{
  flow mig_to_do;
  bool working = false;
  mockturtle::libraries compute_libraries;
  if ( VERBOSE )
  {
    fmt::print( "[i] - Create Libraries : ...\n" );
  }
  mig_npn_resynthesis resyn{ true };
  compute_libraries.resyn = &resyn;
  exact_library_params eps;
  eps.use_dont_cares = true;
  exact_library<mig_network, mig_npn_resynthesis> exact_lib( resyn, eps );
  compute_libraries.exact_lib = &exact_lib;
  if ( VERBOSE )
  {
    fmt::print( "[i] - Start Threads : ...\n" );
  }
  while ( true )
  {
    exp_mutex.lock();
    if ( flow_to_do.size() > 0 )
    {
      working = true;
      state.at( thread_id ).store( true );
      mig_to_do = flow_to_do.back();
      flow_to_do.pop_back();
    }
    exp_mutex.unlock();

    if ( !working )
    {
      if ( !check_state() )
      {
        return;
      }
      sleep( 1 );
      continue;
    }

    std::list<flow> migs;
    std::list<flow> branches;
    migs = compute_flow( mig_to_do, branches, compute_libraries );

    for ( flow& mig_b : migs )
    {
      mig_b.cec = ( CHECK_EQV ) ? ( mig_b.origin_data.benchmark == "hyp" ) ? true : experiments::abc_cec( mig_b.mig, mig_b.origin_data.benchmark ) : false;
    }

    exp_mutex.lock();
    working = false;
    state.at( thread_id ).store( false );
    for ( flow mig_f : branches )
    {
      flow_to_do.push_back( mig_f );
    }
    for ( flow mig_f : migs )
    {
      exp_res(
          mig_f.origin_data.benchmark,
          mig_f.name.substr( 0, mig_f.name.length() - 1 ),
          mig_f.origin_data.size_before,
          mig_f.op_data.size,
          mig_f.origin_data.depth_before,
          mig_f.op_data.depth,
          mig_f.runtime,
          mig_f.cec );
    }
    exp_mutex.unlock();

    if ( WRITE_IN_CSV )
    {
      std::ofstream writer;
      writer.open( fmt::format( "{}.csv", path_csv ), std::ios::app );
      if ( !writer.is_open() )
      {
        sleep( 0.1 );
        writer.open( fmt::format( "{}.csv", path_csv ), std::ios::app );
      }
      for ( const flow& mig_f : migs )
      {
        if ( VERBOSE )
        {
          //  fmt::print( "[i] - Saving\n");
        }
        writer << fmt::format(
            "{};{};{};{};{};{};{};{}\n",
            mig_f.origin_data.benchmark,
            mig_f.name.substr( 0, mig_f.name.length() - 1 ),
            mig_f.origin_data.size_before,
            mig_f.op_data.size,
            mig_f.origin_data.depth_before,
            mig_f.op_data.depth,
            mig_f.runtime,
            mig_f.cec );
      }
    }
  }
}

int main( int argc, char* argv[] )
{
  using namespace experiments;

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
    writer.open( fmt::format( "{}.csv", name ) );
    writer << "benchmark;flow;size;size_mig;depth;depth_mig;runtime;cec" << std::endl;
    writer.close();
  }

  if ( WRITE_IN_CSV_VERBOSE )
  {
    if ( VERBOSE )
      fmt::print( "[i] - Creating output CSV_VERBOSE file\n" );
    writer.open( fmt::format( "{}_VERBOSE.csv", name ) );
    writer << "benchmark;flow;operation;size_before;size_after;depth_before;depth_after;runtime" << std::endl;
    writer.close();
  }

  for ( auto const& benchmark_json : json_flow.at( "do" ).items() )
  {
    std::string benchmark = benchmark_json.value().get<std::string>();
    mockturtle::mig_network mig;
    if ( lorina::read_aiger( benchmark_path( benchmark ), mockturtle::aiger_reader( mig ) ) != lorina::return_code::success )
    {
      continue;
    }
    flow mig_f = { mig, "" };
    mig_f.origin_data = const_flow_data{ benchmark, mig.num_gates(), mockturtle::depth_view( mig ).depth(), name };
    mig_f.json_flow = json_flow.at( "flow" );
    flow_to_do.push_back( mig_f );
  }
  char *eptr;
  const auto processor_count = (argc > 3) ? strtoul(argv[3],& eptr,10) : std::thread::hardware_concurrency();
  for ( auto i = 0u; i < processor_count; ++i )
  {
    state.emplace_back( false );
  }

  if ( VERBOSE )
  {
    fmt::print( "[i] - Computing : ...\n" );
  }

  std::vector<std::thread> threads;
  for ( auto i = 0u; i < processor_count; ++i )
  {
    threads.emplace_back( thread_run, i, name );
  }

  /* wait threads */
  for ( auto i = 0u; i < processor_count; ++i )
  {
    threads[i].join();
  }

  exp_res.save();
  exp_res.table();
  return 0;
}