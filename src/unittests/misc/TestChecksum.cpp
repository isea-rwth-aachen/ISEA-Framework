#include "TestChecksum.h"

#include "../../misc/checksum.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include <cxxtest/TestSuite.h>

void TestChecksum::TestStringChecksum()
{
    // echo -n "a rondom string" | sha1sum
    TS_ASSERT_EQUALS( misc::Checksum( "a random string" ), "9d9f76c04e7b3c8ceac454f70a2f3dea3518f187" );
}

void TestChecksum::TestXMLChecksum()
{
    // echo -n "<Configuration><MetaData><Hash/></MetaData></Configuration>" | sha1sum
    const char * xml = R"(
        <Configuration>
            <MetaData>
                <Hash>8adfa016bc2c71b72c1a230a7db4e3d5d5e39764</Hash>
            </MetaData>
        </Configuration>
    )";
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromMem( xml );
    std::string originalHash = "8adfa016bc2c71b72c1a230a7db4e3d5d5e39764";
    std::string actualHash = "b37414ac2accbbca03d12d1bb52e930eb7004ed3";
    TS_ASSERT_EQUALS( misc::GetOriginalXMLChecksum( parser ), originalHash );
    TS_ASSERT_EQUALS( misc::GetXMLChecksum( parser ), actualHash );
}