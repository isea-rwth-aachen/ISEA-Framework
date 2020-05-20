#ifndef _STRCONT_
#define _STRCONT_

#include <boost/shared_array.hpp>
#ifdef __NO_STRING__
#include <vector>
#else
#include <string>
#endif

class TestStrCont;

namespace misc
{

/// StrCont contains a c-style string that is persistent as long as it exists
class StrCont
{
    friend class ::TestStrCont;

    public:
    StrCont();
    ///@param[in] inputString Is copied into instance
    StrCont( const char *inputString );
    ///@param[in] inputInt Is transformed into string and copied into instance
    StrCont( const int &inputInt );
    ///@param[in] inputInt Is transformed into string and copied into instance
    StrCont( const size_t &inputInt );
    ///@param[in] inputInt Is transformed into string and copied into instance
    StrCont( const std::ptrdiff_t &inputInt );
    ///@param[in] inputDouble Is transformed into string and copied into instance
    StrCont( const double &inputDouble );
    void swap( StrCont &rhs );
    virtual ~StrCont();
    operator const char *() const;
    StrCont &operator+=( const StrCont &rhs );
    size_t size() const { return mContent.size(); };

    private:
#ifdef __NO_STRING__
    std::vector< char > mContent;
#else
    std::string mContent;
#endif
};

StrCont operator+( const StrCont &lhs, const StrCont &rhs );
// bool operator==(const StrCont &lhs, const StrCont &rhs);
}    // namespace misc

#endif
