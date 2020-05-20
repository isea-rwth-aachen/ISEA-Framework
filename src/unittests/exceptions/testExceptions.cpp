#include "testExceptions.h"

#include <cstring>

#include "../../exceptions/error_proto.h"

void TestExceptions::TestOutput()
{
    /*
        try
        {
            ErrorFunction<std::runtime_error> ( "rectangular_block", 100, "error-file", "ElementsDimensionsNegative",
       "blubb");
        }
        catch (std::runtime_error &e)
        {
            std::cout<< e.what() <<std::endl;
        }
    */

    TS_ASSERT_THROWS_EQUALS( ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                                  "ElementsDimensionsNegative", "blubb" ),
                             std::runtime_error & e,
                             strcmp( e.what(), "Every element of Dimensions must be bigger than zero in blubb.\n" ), 0 );
}
