
#ifdef _DS1006
#include <stdexcept>
namespace boost
{

    void throw_exception(std::exception const&e)
    {
            throw e;
    }
}

#endif
