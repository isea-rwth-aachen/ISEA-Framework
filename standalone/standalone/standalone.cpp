#include "standalone.h"

#include <fstream>

#include "../../src/version/version.h"

namespace standalone
{

Standalone::Standalone( const std::string &name )
    : mApp( name )
{
}

void Standalone::PrintVersion( int /* count */ )
{
    printf( "Version: %s\n", VERSION_NUMBER );
    throw CLI::Success();
}

void Standalone::AddOptions()
{
    mApp.add_flag_function( "-v,--version", PrintVersion, "Display version information and exit" )->short_circuit();
}

bool Standalone::ParseCommandLine( int argc, char *argv[] )
{
    this->AddOptions();
    CLI11_PARSE( mApp, argc, argv );
    return true;
}

}    // namespace standalone