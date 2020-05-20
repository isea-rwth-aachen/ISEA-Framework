#ifndef _GET_POSITION_IN_VECTOR_
#define _GET_POSITION_IN_VECTOR_

#include <vector>



namespace misc
{

///Returns the position in vec of the first element that equals val. If no element equals val, vec.size() is returned.
template<typename T>
typename std::vector<T>::size_type GetPositionInVector(const std::vector<T> &vec, const T& val)
{
    
    size_t j = vec.size();
    for(size_t k = 0; k < vec.size(); ++k)
        if(vec[k] == val)
        {
            j = k;
            break;
        }
    return j;
}

}
#endif
