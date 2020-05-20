/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : string2lookup_exception.h
* Creation Date : 31-10-2012
* Last Modified : Do 20 Feb 2014 19:19:50 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _STRING2LOOKUP_EXCEPTION_
#define _STRING2LOOKUP_EXCEPTION_
//STD
#include <string>
#include <exception>

namespace convert
{
    using namespace std;
/// Error class if the Lookup is not symetric e.g rows of lookup  is unequal to the rows of the data 
class AsymetricLookupError: public std::exception
{
public:
    AsymetricLookupError(std::string &whe) : where(whe) { };
    virtual ~AsymetricLookupError() throw() { };
    virtual const char* what() const throw()
    {
        return std::string("Rows are having asymetric number of elements" + where ).c_str();
    }
private:
    std::string where;
};

} /* END NAMESPACE */
#endif /* _STRING2LOOKUP_EXCEPTION_ */
