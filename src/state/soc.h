/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : soc.h
* Creation Date : 01-10-2012
* Last Modified : Do 23 Jan 2014 13:51:05 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _SOC_
#define _SOC_

// STD
#include <vector>

// ETC
#include "../exceptions/error_proto.h"

#include "dgl_state.h"

class TestSoc;
class TestXML;

namespace state
{

enum class SocGetFormat
{
    PERCENT,
    AS,
    AH
};

enum class SocSetFormat
{
    ABSOLUT,
    DELTA,
    FACTOR
};

/// This class describes the behaviour of the State of Charge depending on the capacity
class Soc : public State
{
    friend class ::TestSoc;
    friend class ::TestXML;

    public:
    Soc( const double initialCapacity, const double actualCapacity, const double initialSoc,
         const double minimumValue = 0, const double maximumValue = 100 );
    virtual ~Soc(){};

    template < SocGetFormat format = SocGetFormat::PERCENT >
    double GetValue() const;
    double GetValue() const;
    const double& GetValueRef() const;

    template < SocGetFormat format = SocGetFormat::AS >
    double GetInitialCapacity() const;

    template < SocGetFormat format = SocGetFormat::AS >
    double GetActualCapacity() const;

    template < SocSetFormat setFormat = SocSetFormat::ABSOLUT >
    void SetStoredEnergy( const double energy );

    template < SocSetFormat setFormat = SocSetFormat::ABSOLUT >
    void SetOffset( const double offset );

    template < SocGetFormat getFormat = SocGetFormat::AS >
    double GetOffset() const;

    template < SocSetFormat setFormat = SocSetFormat::ABSOLUT >
    void SetCapacity( const double capacity );

    template < SocSetFormat setFormat = SocSetFormat::ABSOLUT >
    void SetCapacityFactor( const double capacityFactor );

    template < SocGetFormat format = SocGetFormat::PERCENT >
    double GetMinimumValue() const;

    template < SocGetFormat format = SocGetFormat::PERCENT >
    double GetMaximumValue() const;

    /// get the ratio between actual and initial capacity
    double GetCapacityFactor() const;

    void ResetToInitialValue();

    private:
    const double mInitialCapacity;    // [As]

    double mActualCapacity;    // [As]
    double mActualSoc;         // [%]
    // portion of the soc that is caused by an offset from aging. This offset is already included in mActualSoc.
    double mOffset;    // [%]

    const double mMinimumValue;
    const double mMaximumValue;
    const double mInitialValue;
};

template < SocGetFormat format >
double Soc::GetValue() const
{
    double returnValue = 0;

    if ( format == SocGetFormat::PERCENT )
    {
        returnValue = mActualSoc * 100;
    }
    else if ( format == SocGetFormat::AS )
    {
        returnValue = mActualSoc * mActualCapacity;
    }
    else if ( format == SocGetFormat::AH )
    {
        returnValue = mActualSoc * mActualCapacity / 3600;
    }
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

template < SocGetFormat format >
double Soc::GetOffset() const
{
    double returnValue = 0;

    if ( format == SocGetFormat::PERCENT )
    {
        returnValue = mOffset * 100;
    }
    else if ( format == SocGetFormat::AS )
    {
        returnValue = mOffset * mActualCapacity;
    }
    else if ( format == SocGetFormat::AH )
    {
        returnValue = mOffset * mActualCapacity / 3600;
    }
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

template < SocGetFormat getFormat >
double Soc::GetInitialCapacity() const
{
    double returnValue = 0;

    switch ( getFormat )
    {
        case SocGetFormat::AS:
            returnValue = mInitialCapacity;
            break;

        case SocGetFormat::AH:
            returnValue = mInitialCapacity / 3600;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

template < SocGetFormat getFormat >
double Soc::GetActualCapacity() const
{
    double returnValue = 0;

    switch ( getFormat )
    {
        case SocGetFormat::AS:
            returnValue = mActualCapacity;
            break;

        case SocGetFormat::AH:
            returnValue = mActualCapacity / 3600;
            break;

        case SocGetFormat::PERCENT:
            returnValue = mActualCapacity / mInitialCapacity * 100;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

template < SocSetFormat setFormat >
void Soc::SetStoredEnergy( const double value )
{
    switch ( setFormat )
    {
        case SocSetFormat::ABSOLUT:
            mActualSoc = value / mActualCapacity;
            break;

        case SocSetFormat::DELTA:
            mActualSoc += value / mActualCapacity;
            break;

        case SocSetFormat::FACTOR:
            mActualSoc = value;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }
}

template < SocSetFormat setFormat >
void Soc::SetOffset( const double value )
{
    double oldOffset = mOffset;
    switch ( setFormat )
    {
        case SocSetFormat::ABSOLUT:
            mOffset = value / mActualCapacity;
            break;

        case SocSetFormat::DELTA:
            mOffset += value / mActualCapacity;
            break;

        case SocSetFormat::FACTOR:
            mOffset = value;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }
    mActualSoc += mOffset - oldOffset;
}

template < SocSetFormat setFormat >
void Soc::SetCapacity( const double value )
{
    const double storedEnergy = mActualSoc * mActualCapacity;
    const double offsetEnergy = mOffset * mActualCapacity;

    switch ( setFormat )
    {
        case SocSetFormat::ABSOLUT:
            mActualCapacity = value * 3600;
            break;

        case SocSetFormat::DELTA:
            mActualCapacity += value * 3600;
            break;

        case SocSetFormat::FACTOR:
            mActualCapacity = value * mInitialCapacity;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    mActualSoc = storedEnergy / mActualCapacity;
    mOffset = offsetEnergy / mActualCapacity;
}

template < SocSetFormat setFormat >
void Soc::SetCapacityFactor( const double factor )
{
    const double storedEnergy = mActualSoc * mActualCapacity;
    const double offsetEnergy = mOffset * mActualCapacity;

    switch ( setFormat )
    {
        case SocSetFormat::ABSOLUT:
            mActualCapacity = mInitialCapacity * factor;
            break;

        case SocSetFormat::DELTA:
            mActualCapacity += mInitialCapacity * factor;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    mActualSoc = storedEnergy / mActualCapacity;
    mOffset = offsetEnergy / mActualCapacity;
}

template < SocGetFormat format >
double Soc::GetMinimumValue() const
{
    double returnValue = 0;

    if ( format == SocGetFormat::PERCENT )
    {
        returnValue = mMinimumValue * 100;
    }
    else if ( format == SocGetFormat::AS )
    {
        returnValue = mMinimumValue * mActualCapacity;
    }
    else if ( format == SocGetFormat::AH )
    {
        returnValue = mMinimumValue * mActualCapacity / 3600;
    }
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

template < SocGetFormat format >
double Soc::GetMaximumValue() const
{
    double returnValue = 0;

    if ( format == SocGetFormat::PERCENT )
    {
        returnValue = mMaximumValue * 100;
    }
    else if ( format == SocGetFormat::AS )
    {
        returnValue = mMaximumValue * mActualCapacity;
    }
    else if ( format == SocGetFormat::AH )
    {
        returnValue = mMaximumValue * mActualCapacity / 3600;
    }
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "Soc" );
    }

    return returnValue;
}

}    // namespace state

#endif /* _SOC_ */
