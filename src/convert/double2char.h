/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : double2char.h
* Creation Date : 19-08-2014
* Last Modified : Di 19 Aug 2014 10:26:38 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DOUBLE2AR_
#define _DOUBLE2AR_

#include <boost/scoped_array.hpp>

/// This class creates an interface to convert doubles into an char array
class Double2Char
{
    public:
        Double2Char(double *array, size_t length);
        boost::scoped_array<char> mReturnArray;
    private:
        Double2Char(){};
};




#endif /* _DOUBLE2AR_ */
