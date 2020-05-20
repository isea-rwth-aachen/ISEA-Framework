/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : svgLibraryInterpreter.cpp
* Creation Date : 15-04-2015
* Last Modified : Do 15 Okt 2015 12:16:45 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DS1006
#include "svgLibraryInterpreter.h"

// STD
#include <algorithm>
#include <stdexcept>

#ifndef __EXCEPTIONS__
#include <cstdio>
#endif

// BOOST
#include <boost/lexical_cast.hpp>


// ETC
#include "../misc/macros.h"
#include "../exceptions/error_proto.h"

namespace svg
{

const char *standardLibrary =
 "<svg\n"
 "   version=\"1.1\"\n"
 "   width=\"65\"\n"
 "   height=\"65\">  \n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"OhmicResistance\">\n"
 "   <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"Capacity\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 27.5,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 37.5,32.5 27.5,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 27.5,17.5 0,30\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 37.5,17.5 0,30\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"WarburgTanh\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "    <text\n"
 "       style=\"font-size:50px;line-height:125%;fill:#000000;fill-opacity:1;font-family:Sans\"\n"
 "       x=\"12\"\n"
 "       y=\"46\"\n"
 "   >w</text>\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"Zarc\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <text\n"
 "       style=\"font-size:50px;line-height:125%;fill:#000000;fill-opacity:1;font-family:Sans\"\n"
 "       x=\"19.5\"\n"
 "       y=\"46\">z</text>\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"WarburgCotanh\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "    <text\n"
 "       style=\"font-size:50px;line-height:125%;fill:#000000;fill-opacity:1;font-family:Sans\"\n"
 "       x=\"-52.4\"\n"
 "       y=\"47\"\n"
 "       transform=\"matrix(0,-1,1,0,0,0)\"\n"
 "       >w</text>\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"VoltageSource\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 30,18.5 0,28\"\n"
 "       id=\"path21990\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 35,18.5 0,28\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "   <circle\n"
 "       cx=\"32.5\"\n"
 "       cy=\"32.5\"\n"
 "       r=\"17.5\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"ParallelRC\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 7,14 8,0\" />\n"
 "    <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 50,14 8,0\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"7.5\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "    <path\n"
 "       d=\"m 0,32.5 7,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 58,32.5 7,0\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 27.5,40 0,20\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 37.5,40 0,20\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 7,50 20,0\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 38,50 20,0\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 7,12.5 0,39\" />\n"
 "   <path\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\"\n"
 "       d=\"m 58,12.5 0,39\" />\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"Inductance\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-miterlimit:"
 "4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:inline;overflow:"
 "visible\" />\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"ConstantPhaseelement\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 30,0\" \n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 35,32.5 30,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 33,22.5 -4,10 4,10\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "   <path\n"
 "       d=\"m 39,22.5 -4,10 4,10\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"SerialConnection\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 65,0\" \n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "  </g>\n"
 "  <g\n"
 "     transform=\"translate(0,0)\"\n"
 "     id=\"ParallelConnection\">\n"
 "    <path\n"
 "       d=\"m 0,0 0,65\" \n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:square;stroke-linejoin:miter;stroke-miterlimit:4;"
 "stroke-opacity:1;stroke-dasharray:none\" />\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"Cellelement\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "    <text\n"
 "       style=\"font-size:50px;line-height:125%;fill:#000000;fill-opacity:1;font-family:Sans\"\n"
 "       x=\"19.5\"\n"
 "       y=\"46\"\n"
 "       >c</text>\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"SphericalDiffusion\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 " <circle r=\"11.243096\" cy=\"32.5\" cx=\"32.5\" "
 "style=\"fill:#ffffff;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:3;stroke-linecap:square;stroke-"
 "linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" id=\"path10293\" />\n"
 "  </g>\n"
 "  <g\n"
 "     id=\"Rmphn\"\n"
 "     transform=\"translate(0,0)\">\n"
 "    <path\n"
 "       d=\"m 0,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <path\n"
 "       d=\"m 50,32.5 15,0\"\n"
 "       "
 "style=\"fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-"
 "opacity:1;stroke-dasharray:none\" />\n"
 "    <rect\n"
 "       width=\"35\"\n"
 "       height=\"15\"\n"
 "       x=\"15\"\n"
 "       y=\"25\"\n"
 "       "
 "style=\"color:#000000;fill:none;stroke:#000000;stroke-width:3;stroke-linecap:butt;stroke-linejoin:round;stroke-"
 "miterlimit:4;stroke-opacity:1;stroke-dasharray:none;stroke-dashoffset:0;marker:none;visibility:visible;display:"
 "inline;overflow:visible\" />\n"
 "    <text\n"
 "       style=\"font-size:16px;line-height:125%;fill:#000000;fill-opacity:1;font-family:Sans\"\n"
 "       x=\"16\"\n"
 "       y=\"37\"\n"
 "   >rmp</text>\n"
 "  </g>\n"
 "</svg>";

SvgLibraryInterpreter::SvgLibraryInterpreter( const std::string &svgLibrary )
    : TAG_START( "<g" )
    , TAG_END( "</g>" )
    , TRANSFORM_START( "transform=\"" )
    , TRANSFORM_END( ")\"" )
{
    if ( svgLibrary.empty() )
    {
        mSvgString = std::string( standardLibrary );
    }

    else
        mSvgString = svgLibrary;

    std::transform( mSvgString.begin(), mSvgString.end(), mSvgString.begin(), ::tolower );
    mRegisteredTwoPorts.push_back( "ohmicresistance" );
    mRegisteredTwoPorts.push_back( "voltagesource" );
    mRegisteredTwoPorts.push_back( "capacity" );
    mRegisteredTwoPorts.push_back( "parallelrc" );
    mRegisteredTwoPorts.push_back( "zarc" );
    mRegisteredTwoPorts.push_back( "warburgtanh" );
    mRegisteredTwoPorts.push_back( "warburgcotanh" );
    mRegisteredTwoPorts.push_back( "inductance" );
    mRegisteredTwoPorts.push_back( "constantphaseelement" );
    mRegisteredTwoPorts.push_back( "serialconnection" );
    mRegisteredTwoPorts.push_back( "parallelconnection" );
    mRegisteredTwoPorts.push_back( "cellelement" );
    mRegisteredTwoPorts.push_back( "sphericaldiffusion" );
    mRegisteredTwoPorts.push_back( "rmphn" );

    ParseSvgLibrary();
}

void SvgLibraryInterpreter::ParseSvgLibrary()
{
    for ( size_t i = 0; i < mRegisteredTwoPorts.size(); ++i )
    {
        FindAttribute( mRegisteredTwoPorts[i] );
    }
}

void SvgLibraryInterpreter::FindAttribute( std::string tagId )
{
    size_t startIndex = mSvgString.find( tagId );

    if ( startIndex == std::string::npos )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TagNotFound", tagId.c_str() );

    size_t tagStart = mSvgString.rfind( TAG_START, startIndex );
    if ( tagStart == std::string::npos )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TagCompoundNotFound", TAG_START.c_str(),
                                             tagId.c_str() );

    size_t tagEnd = mSvgString.find( TAG_END, startIndex ) + TAG_END.size();
    if ( tagStart == std::string::npos )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TagCompoundNotFound", TAG_END.c_str(),
                                             tagId.c_str() );

    InsertTag( tagStart, tagEnd, tagId );
}

void SvgLibraryInterpreter::InsertTag( size_t start, size_t end, std::string tag )
{
    std::transform( tag.begin(), tag.end(), tag.begin(), ::tolower );
    if ( mTwoPortName2Tag.count( tag ) > 0 )
        return;

    mTwoPortName2Tag.insert( std::pair< std::string, std::string >( tag, mSvgString.substr( start, end - start ) ) );
}

std::string SvgLibraryInterpreter::GetTag( std::string tagId ) const
{
    std::transform( tagId.begin(), tagId.end(), tagId.begin(), ::tolower );
    if ( mTwoPortName2Tag.count( tagId ) == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TagNotFound", tagId.c_str() );

    return mTwoPortName2Tag.find( tagId )->second;
}

std::string SvgLibraryInterpreter::GetTagWithPosition( std::string tagId, double x, double y ) const
{
    std::string returnstring = GetTag( tagId );

    size_t startTransform = returnstring.find( TRANSFORM_START );
    size_t spacer = returnstring.find( ",", startTransform ) + 1;
    size_t endTransform = returnstring.find( TRANSFORM_END, startTransform );
    const std::string transformstring = "transform=\"translate(";
    size_t startOffset = startTransform + transformstring.size();
    size_t EndOffset = returnstring.find( ")", spacer );

    std::string x_string = returnstring.substr( startOffset, spacer - startOffset - 1 );
    std::string y_string = returnstring.substr( spacer, EndOffset - spacer );

    returnstring.replace( startTransform, endTransform - startTransform + 1,    // +1 weil ) noch mit muss
                          "transform=\"translate(" + boost::lexical_cast< std::string >( x ) + "," +
                           boost::lexical_cast< std::string >( y ) + ")" );
    return returnstring;
}

std::string SvgLibraryInterpreter::GetParallelConnection( double x, double y, double length ) const
{
    std::string returnstring = GetTagWithPosition( "parallelconnection", x, y );

    const size_t lengthDeclaration = returnstring.find( "65\"" );

    returnstring.replace( lengthDeclaration, 2, boost::lexical_cast< std::string >( 65 * length ) );

    return returnstring;
}

std::string SvgLibraryInterpreter::GetSerialConnection( double x, double y, double length ) const
{
    std::string returnstring = GetTagWithPosition( "serialconnection", x, y );

    const size_t lengthDeclaration = returnstring.find( "65," );

    returnstring.replace( lengthDeclaration, 2, boost::lexical_cast< std::string >( 65 * length ) );

    return returnstring;
}
}
#endif /* _DS1006 */
