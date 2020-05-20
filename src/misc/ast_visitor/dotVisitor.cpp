/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "dotVisitor.h"
namespace symbolic
{
namespace ast
{
template struct DotVisitor< &std::cout, true >;
template struct DotVisitor< &std::cout, false >;
}
}
