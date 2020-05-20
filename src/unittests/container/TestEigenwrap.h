/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestEigenwrap.h
* Creation Date : 31-01-2017
* Last Modified : Mo 19 Jun 2017 15:54:09 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTEIGENWRAP_
#define _TESTEIGENWRAP_
#include <cxxtest/TestSuite.h>

#include "../../misc/symbolic.h"
#include "../../misc/StrCont.h"

#include "../../container/eigen_wrapper.h"
namespace CxxTest
{

template < typename T >
class ValueTraits;

template <>
class ValueTraits< symbolic::Symbolic< symbolic::OutType > >
{
    public:
    ValueTraits( const symbolic::Symbolic< symbolic::OutType > &t );
    const char *asString( void ) const;

    private:
    misc::StrCont mOutputString;
};
}    //  CxxTest


class TestEigenWrapper : public CxxTest::TestSuite
{
    public:
    void TestEigenWrap();
    void TestEigenEmpty();
};

/// DESCRIPTION
#endif /* _TESTEIGENWRAP_ */
