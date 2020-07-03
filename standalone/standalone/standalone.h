#ifndef _STANDALONE_
#define _STANDALONE_

#include <cstring>
#include <string>

#include <boost/numeric/odeint.hpp>

#include "../../libraries/cli11/CLI11.hpp"

namespace standalone
{
class Standalone
{
    public:
    Standalone( const std::string &name );
    Standalone( const Standalone & ) = delete;
    Standalone &operator=( const Standalone & ) = delete;
    static void PrintVersion( int /* count */ );
    virtual bool ParseCommandLine( int argc, char *argv[] );

    CLI::App mApp;

    protected:
    virtual void AddOptions();
};
}    // namespace standalone

#endif /* _STANDALONE */
