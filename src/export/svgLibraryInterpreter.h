/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : svgLibraryInterpreter.h
* Creation Date : 15-04-2015
* Last Modified : 29.07.2015 19:33:16
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SVGLIBRARYINTERPRETER_
#define _SVGLIBRARYINTERPRETER_

#ifndef _DS1006

// STD
#include <string>
#include <map>
#include <vector>

class TestSvgLibraryInterpreter;
class TestSvgExport;

namespace svg
{
/// The SvgLibraryInterpreter is a simple interpreter which handles getting <tags> from an svg from a certain svg and
/// returning the tags with a given position on the svg sheet
class SvgLibraryInterpreter
{
    friend class ::TestSvgLibraryInterpreter;
    friend class ::TestSvgExport;

    public:
    /// The basic library interpreter creation routine that needs a string (library)
    SvgLibraryInterpreter( const std::string &svgLibrary = "" );
    ~SvgLibraryInterpreter(){};

    /// Returns the content of the tagID with the given position
    /// \param tagId finds tagId in svgLibrary
    /// \param x reference for the calculation of the translation coordinates
    /// \param y reference for the calculation of the translation coordinates
    std::string GetTagWithPosition( std::string tagId, double x, double y ) const;

    /// Returns a connection beetween twoports with the given position
    /// \param x reference for the calculation of the translation coordinates
    /// \param y reference for the calculation of the translation coordinates
    /// \param length reference for the calculation of the length of the connection beetween twoports
    std::string GetParallelConnection( double x, double y, double length = 1 ) const;
    std::string GetSerialConnection( double x, double y, double length = 1 ) const;

    private:
    typedef std::map< std::string, std::string > Name2TagMap;

    /// This function tries to find a certain tag inside of mSvgString and if found the tag inserted into a map
    /// \param tagId find tagId in svgLibrary
    void FindAttribute( std::string tagId );

    /// Insert the content of a tag inside a map for faster handling when GetTagWithPosition is called
    /// \param start is the starting index in mSvgString of the tag
    /// \param end is the ending index in mSvgString of the tag
    void InsertTag( size_t start, size_t end, std::string tag );

    /// ParseSvgLibrary is the basic parsing function. All tags in mRegisteredTwoPorts are trying to be found
    /// if no matching tag is found runtime errors are thrown
    void ParseSvgLibrary();

    /// Returns the content of the tagId
    /// \param tagID finds tagID in svgLibrary
    std::string GetTag( std::string tagId ) const;

    std::string mSvgString;
    Name2TagMap mTwoPortName2Tag;
    std::vector< std::string > mRegisteredTwoPorts;

    const std::string TAG_START;
    const std::string TAG_END;

    const std::string TRANSFORM_START;
    const std::string TRANSFORM_END;
};
}

#endif /*  _DS1006 */
#endif /* _SVGLIBRARYINTERPRETER_ */
