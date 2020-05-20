#include "string2lookup.h"

template std::vector< double > convert::Make1DLookupTable< double >( const std::string& toParse );
template std::vector< std::vector< double > > convert::Make2DLookupTable< double >( const std::string& toParse );
