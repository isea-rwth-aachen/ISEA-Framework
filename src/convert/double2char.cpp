/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : double2char.cpp
* Creation Date : 19-08-2014
* Last Modified : Di 19 Aug 2014 10:28:26 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "double2char.h"


Double2Char::Double2Char(double *array, size_t length)
    :
        mReturnArray(new char[length + 1])
{
	size_t start = 2;
	if (length > 1 )
	{
		// Make sure we don't terminate the string premature for RT
		// If we find a zero ('\0') here, we change it to a '\1'
		// This will happen if the double is an multiple of 256. Then a char cast will yield '\0'
		if (static_cast<char>(static_cast<int> (array[0]) + 0.5) == '\0')
		{
			mReturnArray[0] = '\1';
			start = 1;
		}
		
		else if (static_cast<char>(static_cast<int> (array[1]) + 0.5) == '\0')
		{
			mReturnArray[0] = '\1';
			mReturnArray[1] = '\1';
		}
		else
		{
			start = 0;
		}
		
	}
	else
	{
		//This is an errorness branch, so we terminate the string directly -> '\0'
		if (length == 0)
		{
			mReturnArray[0] = '\0';
			start = 1;
		}
		else if (length == 1)
		{
			mReturnArray[0] = '\0';
			mReturnArray[1] = '\0';
		}
		else
		{
			start = 0;
		}
	}
    for( size_t i = start ; i < length ; ++i )
        mReturnArray[i] = static_cast<char>( static_cast<int> ( array[i] + 0.5 ) );
    mReturnArray[length] = '\0';
}
