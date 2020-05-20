/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : ..TestObserver.cpp
* Creation Date : 18-11-2012
* Last Modified : Mi 13 Aug 2014 09:56:09 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <cxxtest/TestSuite.h>
#include <vector>

#include "../../electrical/twoport.h"

class TestObserver : public CxxTest::TestSuite
{

    public:
    void testObserverOperationsSingleCell();
    void testFilters();

    private:
    std::vector< std::vector< double > > CopyToVector( const double data[7][4] );
    void testFilterCreation( std::vector< boost::shared_ptr< electrical::TwoPort< myMatrixType > > >& twoPorts,
                             const boost::shared_ptr< electrical::TwoPort< myMatrixType > >& rootPort );
};
