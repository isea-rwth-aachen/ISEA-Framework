#include "voltagesource_preisacheverett.h"

//TODO: change mSOC to mSOC at time t
namespace electrical
{

template < typename T >
VoltageSource_PreisachEverett< T >::VoltageSource_PreisachEverett( boost::shared_ptr< state::Soc >& socState, double minOCV,
                                                                   boost::shared_ptr< object::Object< double > > obj,
                                                                   const bool observable,   typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
    , OCVmin( minOCV )
    , mSOC( socState )
    , OCV( minOCV )
{
    ( this->ExtremumPoints ).push_back( 0 );     // adds an SOC=0% as a first element of the vector
    double ExtremePoint = mSOC->GetValue();
    if ( ExtremePoint > 0 )
        addMaxima( ExtremePoint );

}

template < typename T >
VoltageSource_PreisachEverett< T >::VoltageSource_PreisachEverett( boost::shared_ptr< state::Soc >& socState, double minOCV,
                                                                    object::Object< double >* obj, const bool observable,
                                                   typename TwoPort< T >::DataType dataValues )
    : VoltageSource_PreisachEverett< T >( socState, minOCV, boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

//TODO: change mSOC 
template < typename T >
void VoltageSource_PreisachEverett< T >::UpdateExtremumPoints()
{  
    if ( OCV == 0 )
        OCV = OCVmin;
    int size_ExtremePoints = (int) ( this->ExtremumPoints ).size();
    double Soc = mSOC->GetValue();
    if ( Soc == ExtremumPoints.back() )    // no changes has Occured in term of SOC of the battery
        return;
    if ( size_ExtremePoints == 0 )    // the vector always starts with 0 SOC
        ExtremumPoints.push_back( 0 );
    if ( Soc >= 100 )
        Soc = 100;
    if ( Soc <= 0 )
        Soc = 0;
    if ( ( size_ExtremePoints == 1 ) &&  ( Soc > 0 ) )    // if the vector only has an element that is 0 the new SOC is automatically added if this SOC is not 0 
        {
            ExtremumPoints.push_back( Soc ); 
            double E_alpha_beta = 2 * mObject->GetValue( ExtremumPoints.at( 0 ), ExtremumPoints.at( 1 ));
            OCV = OCV + E_alpha_beta;
            Everett.push_back( E_alpha_beta );
            return;
    }
    if ( Soc == 0 )
    {
        OCV = OCVmin;
        Everett.clear();
        ExtremumPoints.clear();
        ExtremumPoints.push_back( 0 );
    }
    if ( Soc == 100 )
    {
        Everett.clear();
        ExtremumPoints.clear();
        ExtremumPoints.push_back( 0 );
        ExtremumPoints.push_back( 100 );
        double E_alpha_beta = 2 * mObject->GetValue( 0, 100 );
        OCV = OCVmin + E_alpha_beta;
        Everett.push_back( E_alpha_beta ); 
    }
    
    size_ExtremePoints = (int)( this->ExtremumPoints ).size();
    if ( ( (int)size_ExtremePoints ) % 2 )    // 0: last element that was added to the vector is a local maxima, 1: the other way around
    {
        if ( Soc < ExtremumPoints.back() )
        {
            ExtremumPoints.pop_back();
            OCV = OCV - Everett.back();
            Everett.pop_back();
            addMinima( Soc );
        }
        else
        {
            addMaxima( Soc );
        }

    }
    else
    {
        if ( Soc > ExtremumPoints.back() ) 
        {
            ExtremumPoints.pop_back();
            OCV = OCV - Everett.back();
            Everett.pop_back();
            addMaxima(Soc);
        }
        else
        {
            addMinima( Soc );
        }
    }
}

template < typename T >
void VoltageSource_PreisachEverett< T >::addMaxima( double ExtremePoint )
{
    int size_ExtremePoints = (int)ExtremumPoints.size();
    if ( ( size_ExtremePoints % 2 ) == 0 )
        ExtremePoint++;
    size_ExtremePoints -= 2; 
    while ( ExtremePoint > 0 )
    {
        if ( ExtremePoint >= ExtremumPoints.at( size_ExtremePoints ) )
        {
            ExtremumPoints.pop_back();
            OCV = OCV - Everett.back();
            Everett.pop_back();
            if ( ( ExtremumPoints.size() % 2 ) == 0 )
            {
                ExtremumPoints.pop_back();
                OCV = OCV - Everett.back();
                Everett.pop_back();
            }
            size_ExtremePoints -= 2;
        }
        else
            break;
    }
    ExtremumPoints.push_back( ExtremePoint );
    double E_alpha_beta = 2 * mObject->GetValue( ExtremumPoints.at( ExtremumPoints.size() - 2 ), 
                                             ExtremumPoints.at( ExtremumPoints.size() - 1 ) );
    OCV = OCV + E_alpha_beta; 
    Everett.push_back( E_alpha_beta );
}

template < typename T >
void VoltageSource_PreisachEverett< T >::addMinima( double Soc )
{
    size_t size_ExtremePoints = ExtremumPoints.size();
    if ( ( size_ExtremePoints % 2 ) == 1 )
        size_ExtremePoints++;
    size_ExtremePoints -= 2;
    while ( size_ExtremePoints > 0 )
    {
        if ( Soc <= ExtremumPoints.at( size_ExtremePoints ) )
        {
            ExtremumPoints.pop_back();
            OCV = OCV - Everett.back();
            Everett.pop_back();
            if ( ( ExtremumPoints.size() % 2 ) == 1 )
            {
                ExtremumPoints.pop_back();
                OCV = OCV - Everett.back();
                Everett.pop_back();
            }
            size_ExtremePoints -= 2;
        }
        else
            break;
    }
    ExtremumPoints.push_back( Soc );
    double E_alpha_beta = -2 * mObject->GetValue( ExtremumPoints.at( ExtremumPoints.size() - 1 ),
                                              ExtremumPoints.at( ExtremumPoints.size() - 2 ) );
    OCV = OCV + E_alpha_beta; 
    Everett.push_back( E_alpha_beta );

}

template < typename T >
const char* VoltageSource_PreisachEverett< T >::GetName() const
{
    return "VoltageSource_PreisachEverett";
}

template < typename T >
void VoltageSource_PreisachEverett< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
        this->CalculateCurrentValue();

}

template < typename T >
T* VoltageSource_PreisachEverett< T >::GetVoltage()
{
    UpdateExtremumPoints();    // updates the extremum points vector and the Everett values vector every simulation step
    this->mVoltageValue  = OCV; 
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
    return TwoPort< T >::GetVoltage();
}

}    // namespace electrical

template class electrical::VoltageSource_PreisachEverett< myMatrixType >;