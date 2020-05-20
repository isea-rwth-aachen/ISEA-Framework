// a filter that writes an xml file including the new aging parameters after each aging step

#ifndef _XMLFILTER_
#define _XMLFILTER_

#include "../../aging/aging_twoport.h"
#include "../../electrical/cellelement.h"
#include "../../xmlparser/xmlparameter.h"
#include "filter.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class XMLFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;

    XMLFilter( const std::string &filename, const boost::shared_ptr< xmlparser::XmlParameter > &rootNode )
        : FilterT()
        , mFilename( filename )
        , mRootNode( rootNode ){};

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        FilterT::ProcessData( data, t );
        mAgingStep++;
    };

    // appends the aging step number to the xml filename
    std::string MakeFilename( size_t cycle )
    {
        size_t lastSlash = this->mFilename.find_last_of( '/' );
        size_t filenameStart;
        if ( lastSlash == std::string::npos )
            filenameStart = 0;
        else
            filenameStart = lastSlash + 1;
        size_t extensionStart = this->mFilename.find( '.', filenameStart );
        if ( extensionStart == std::string::npos )
            extensionStart = mFilename.length();
        std::string filenameWithoutExtension = this->mFilename.substr( 0, extensionStart );
        std::string extension = this->mFilename.substr( extensionStart );
        return filenameWithoutExtension + "_" + std::to_string( cycle ) + extension;
    }

    protected:
    std::string mFilename;
    boost::shared_ptr< xmlparser::XmlParameter > mRootNode;
    size_t mAgingStep = 0;
};

#ifdef BUILD_AGING_SIMULATION

template < typename T >
class XMLFilterAging : public XMLFilter< T, aging::AgingTwoPort, PreparationType< T > >
{
    public:
    typedef Filter< T, aging::AgingTwoPort, PreparationType< T > > FilterT;

    XMLFilterAging( const std::string &filename, const boost::shared_ptr< xmlparser::XmlParameter > &rootNode )
        : XMLFilter< T, aging::AgingTwoPort >( filename, rootNode ){};

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t );
};

template < typename T >
void XMLFilterAging< T >::ProcessData( const typename FilterT::Data_t & /* data */, const double /* t */ )
{
    /*
    if ( this->mAgingStep > 0 )
    {
        std::vector< double > anodeLamValues;
        std::vector< double > cathodeLamValues;
        std::vector< double > lliValues;
        std::vector< double > resistanceFactors;

        std::vector< double > cellAgeValues;
        std::vector< double > chargeThroughputValues;

        boost::shared_ptr< xmlparser::XmlParameter > agingNode;

        ::state::AgingState *agingState;

        // Get the values from each cell
        for ( size_t i = 0; i < data.size(); ++i )
        {
            agingState = data[i]->GetAgingState().get();

            anodeLamValues.push_back( agingState->GetAnodeLam() );
            cathodeLamValues.push_back( agingState->GetCathodeLam() );
            lliValues.push_back( agingState->GetLli() );
            resistanceFactors.push_back( agingState->GetResistanceFactor() );

            cellAgeValues.push_back( agingState->GetCellAge() );
            chargeThroughputValues.push_back( agingState->GetChargeThroughput() );
        }

        // create the aging node if it does not exist
        if ( !this->mRootNode->HasElementDirectChild( "Aging" ) )
        {
            agingNode = this->mRootNode->AddChild( "Aging" );

            agingNode->AddChild( "AnodeLAM" );
            agingNode->AddChild( "CathodeLAM" );
            agingNode->AddChild( "LLI" );
            agingNode->AddChild( "ResistanceFactor" );
            agingNode->AddChild( "CellAge" );
            agingNode->AddChild( "ChargeThroughput" );

        }

        else
        {
            agingNode = this->mRootNode->GetElementChild( "Aging" );
        }

        // Write the values to the xml nodes
        agingNode->GetElementChild( "AnodeLAM" )->SetDoubleVecValues( anodeLamValues );
        agingNode->GetElementChild( "CathodeLAM" )->SetDoubleVecValues( cathodeLamValues );
        agingNode->GetElementChild( "LLI" )->SetDoubleVecValues( lliValues );
        agingNode->GetElementChild( "ResistanceFactor" )->SetDoubleVecValues( resistanceFactors );
        agingNode->GetElementChild( "CellAge" )->SetDoubleVecValues( cellAgeValues );
        agingNode->GetElementChild( "ChargeThroughput" )->SetDoubleVecValues( chargeThroughputValues );

        this->mRootNode->SaveDocument( this->MakeFilename( this->mAgingStep ).c_str() );
    }

    XMLFilter< T, electrical::Cellelement, PreparationType< T > >::ProcessData( data, t );
*/
}
#endif
}    // namespace observer

#endif
