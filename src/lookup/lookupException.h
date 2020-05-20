/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookupException.h
* Creation Date : 24-05-2012
* Last Modified : Fr 31 Mai 2013 16:55:18 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUPEXCEPTION_
#define _LOOKUPEXCEPTION_

//STD
#include <exception>
#include <string>

// BOOST
#ifndef BOOST_NO_TYPEID
#include <boost/lexical_cast.hpp>
#endif

/// In case measurementpoints are not in ascending order this exception is thrown
class NonSortedLookup: public ::std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "The measurement points are not in ascending order";
    }
};

class UnequalSize: public ::std::exception
{
public:
    UnequalSize(size_t A, size_t B)
        : a(A)
        , b(B)
    { };

    virtual const char* what() const throw()
    {
#ifndef BOOST_NO_TYPEID
        return std::string("Measurement Points and Data do not have the same size[" + boost::lexical_cast<std::string>(a) + std::string("][") + boost::lexical_cast<std::string>(b) + "]").c_str();
#else
        return "Measurement Points and Data do not have the same size";
#endif
    }
private:
    size_t a, b;
};

#endif /* _LOOKUPEXCEPTION_ */
