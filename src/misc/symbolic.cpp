#ifdef _SYMBOLIC_

#include "symbolic.h"

symbolic::Symbolic< double > sqrt( symbolic::Symbolic< double > rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "SQRT(" ) + rhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< double >( tmp );
}


#endif    // _SYMBOLIC_
