/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : esbVisualizer.h
* Creation Date : 09-01-2014
* Last Modified : Di 04 Feb 2014 19:08:26 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ESBVISUALIZER_
#define _ESBVISUALIZER_
#ifndef __NO_STRING__

#include "../misc/matrixInclude.h"

#include <cstring>
#include <fstream>
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>

#include "baseExport.h"
#include "../electrical/twoport_withchild.h"
namespace visualizer
{

/// This class creates a .dot file where the electrical network is described
template < typename matType >
class EsbVisualizer : public BaseExport< matType >
{
    public:
    EsbVisualizer( const electrical::TwoPort< matType > *rootTwoport, std::ostream *of );
    virtual ~EsbVisualizer(){};

    private:
    EsbVisualizer() = delete;

    std::ostream &mOutputfile;
    size_t mCounter, mCSVCounter;
    std::map< const electrical::TwoPort< matType > *, std::string > mTwoPort2Name;
    typedef typename std::map< const electrical::TwoPort< matType > *, std::string >::iterator iterator;

    void CreateDotFile();
    virtual void WritePreamble();
    void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort );
    void HandleSerialTwoPort( const electrical::TwoPortWithChild< matType > *twoPortWithChild );
    void HandleParallelTwoPort( const electrical::TwoPortWithChild< matType > *twoPortWithChild );
    virtual void WriteEnding();

    int CheckForMatchInObservableElements( const electrical::TwoPort< matType > *twoPort );    //< This only works if
    // every node is only
    // visted once!

    std::string MergeNameAndNumber2String( std::string name, int number );
    std::string FindAndReturnTwoPortName( const electrical::TwoPort< matType > *twoPortWithChild );

    protected:
};

template < typename matType >
EsbVisualizer< matType >::EsbVisualizer( const electrical::TwoPort< matType > *rootTwoport, std::ostream *of )
    : BaseExport< matType >( rootTwoport )
    , mOutputfile( *of )
    , mCounter( 0 )
    , mCSVCounter( 0 )
{
    CreateDotFile();
}

template < typename matType >
void EsbVisualizer< matType >::CreateDotFile()
{
    WritePreamble();
    LoopOverAllTwoPorts( this->mRootPort );
    WriteEnding();
}

template < typename matType >
void EsbVisualizer< matType >::LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort )
{

    const char *name = twoPort->GetName();
    std::string twoPortName = FindAndReturnTwoPortName( twoPort );
    // This should return a number corresponding to the CSV-number element
    int count = CheckForMatchInObservableElements( twoPort );

    if ( count != -1 )
        mOutputfile << twoPortName << " [label=\"" << name << "[" << count << "]\"]\n";
    else
        mOutputfile << twoPortName << " [label=" << name << "]\n";


    if ( twoPort->HasChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithChild =
         static_cast< const electrical::TwoPortWithChild< matType > * >( twoPort );
        if ( twoPortWithChild->IsSerialTwoPort() )
            HandleSerialTwoPort( twoPortWithChild );
        else
            HandleParallelTwoPort( twoPortWithChild );

        for ( size_t i = 0; i < twoPortWithChild->size(); ++i )
            LoopOverAllTwoPorts( twoPortWithChild->at( i ) );
    }
}

template < typename matType >
int EsbVisualizer< matType >::CheckForMatchInObservableElements( const electrical::TwoPort< matType > *twoPort )
{
    int count = -1;
    if ( twoPort->IsObservable() )
    {
        count = mCSVCounter;
        ++mCSVCounter;
    }
    return count;
}

template < typename matType >
void EsbVisualizer< matType >::WritePreamble()
{

    mOutputfile << "#dot  -Tpng esb.dot > esb.png\n";
    mOutputfile << "digraph Network\n{\n";
}

template < typename matType >
void EsbVisualizer< matType >::HandleSerialTwoPort( const electrical::TwoPortWithChild< matType > *twoPortWithChild )
{
    HandleParallelTwoPort( twoPortWithChild );
    // TODO:
    // This should be changed to visualize the structure of Serialtwoport
    /*
    if ( twoPortWithChild->size() == 0)
        return;
    string name = FindAndReturnTwoPortName( twoPortWithChild->at(0) );

    mOutputfile << FindAndReturnTwoPortName(twoPortWithChild) << " -> " << name << "\n";

    for( size_t i = 1 ; i < twoPortWithChild->size() ; ++i )
    {
        mOutputfile << FindAndReturnTwoPortName( twoPortWithChild->at( i - 1 ) ) << " -> " << FindAndReturnTwoPortName(
    twoPortWithChild->at( i ) ) << "\n";
    }
    */
}

template < typename matType >
void EsbVisualizer< matType >::HandleParallelTwoPort( const electrical::TwoPortWithChild< matType > *twoPortWithChild )
{
    for ( size_t i = 0; i < twoPortWithChild->size(); ++i )
        mOutputfile << FindAndReturnTwoPortName( twoPortWithChild ) << " -> "
                    << FindAndReturnTwoPortName( twoPortWithChild->at( i ) ) << "\n";
}

template < typename matType >
void EsbVisualizer< matType >::WriteEnding()
{
    mOutputfile << "}\n";
}

template < typename matType >
std::string EsbVisualizer< matType >::MergeNameAndNumber2String( std::string name, int number )
{
    return name + boost::lexical_cast< std::string >( number );
}

template < typename matType >
std::string EsbVisualizer< matType >::FindAndReturnTwoPortName( const electrical::TwoPort< matType > *twoPort )
{
    std::string name;
    iterator it = mTwoPort2Name.find( twoPort );
    if ( it == mTwoPort2Name.end() )
    {
        name = MergeNameAndNumber2String( twoPort->GetName(), mCounter );
        mTwoPort2Name[twoPort] = MergeNameAndNumber2String( twoPort->GetName(), mCounter );
        ++mCounter;
    }
    else
        name = it->second;
    return name;
}


} /* visualizer */
#endif /* __NO_STRING__*/
#endif /* _ESBVISUALIZER_ */
