/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicOptimization.h
* Creation Date : 05-03-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLICOPTIMIZATION_
#define _SYMBOLICOPTIMIZATION_

#include <string>

namespace symbolic
{

/// DESCRIPTION
class SymbolicOptimization
{
    public:
    SymbolicOptimization(){};
    ~SymbolicOptimization(){};
    bool Optimize( std::string &input, std::string &returnStr );

    private:
    protected:
};
}
#endif /* _SYMBOLICOPTIMIZATION_ */
