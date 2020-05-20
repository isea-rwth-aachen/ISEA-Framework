/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matlabExport.h
* Creation Date : 26-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATLABEXPORT_
#define _MATLABEXPORT_
#ifdef _SYMBOLIC_
#include "exportSystem.h"

#include <limits>

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/ohmicresistance.h"
#include "../electrical/parallelRCAlg.h"
#include "../electrical/parallelrc.h"
#include "../electrical/paralleltwoport.h"
#include "../electrical/serialtwoport.h"
#include "../electrical/voltagesource.h"
#include "../electrical/warburgCotanh.h"
#include "../electrical/warburgTanh.h"
#include "../electrical/zarc.h"
#include "../electrical/zarcalg.h"
#include "../misc/cellelementCounter.h"
#include <boost/thread/thread.hpp>

/// DESCRIPTION
class TestMatlabExport;
namespace symbolic
{

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH = 3 >
class MatlabExport : public ExportSystem< matType, ExportClass, Parser, DEPTH >
{
    friend class ::TestMatlabExport;
    typedef electrical::TwoPort< matType > TP;

    public:
    MatlabExport( simulation::ElectricalSimulation< myMatrixType, double, false > *sim, std::ostream *stream,
                  const double defaultTimeout = 5 )
        : ExportSystem< matType, ExportClass, Parser, DEPTH >( sim, stream, defaultTimeout )
    {
        if ( !this->mStream || !this->mSimulation )
            return;
        typedef std::numeric_limits< double > dbl;


        this->mStream->precision( dbl::max_digits10 );

        const auto size = this->mSimulation->mStateSystemGroup.GetStateCount();
        auto &x = this->mSimulation->mStateSystemGroup.mStateVector;
        for ( size_t i = 0; i < size; ++i )
        {
            x.coeffRef( i, 0 ) = ScalarUnit( misc::StrCont( "State" ) + misc::StrCont( i + 1 ) );
        }

        this->mSimulation->UpdateSystem();
        this->mSimulation->UpdateSystemValues();

        this->LoopOverAllTwoPorts( this->mRootPort );
        WritePreamble();
    }

    virtual ~MatlabExport(){};
    virtual void PrintVoltageMatrix();

    private:
    virtual void WritePreamble()
    {
        BaseFunction();
        // PrintBaseCurrent();
        CreateLookupTable();
        CreateOdeFunction();
        DefineDXFunction();
        // PrintVoltageMatrix();
        CreateOutputFuncton();
        CreateSOCFunction();
    };

    void PrintBaseCurrent() const;
    void CreateStateVector() const;
    void UpdateStates() const;
    void BaseFunction() const;

    void CreateOutputFuncton() const;
    void CreateOutputFuncton1() const;
    void CreateOutputFuncton3() const;

    void DefineDXFunction() const;
    void DefineDXFunction1() const;
    void DefineDXFunction3() const;

    void CreateOdeFunction() const;
    void CreateOdeFunction1() const;
    void CreateOdeFunction3() const;

    void DefineGlobalStates() const;
    void HelperStates() const;
    void Serialize1Dlookup( const lookup::LookupType1D< double > *obj, std::string argument ) const;
    void CreateSOCFunction() const;
    void CreateSOCFunction1() const;
    void CreateSOCFunction3() const;


    virtual void WriteEnding(){};

    void SerializeElectricalElement( const TP *two );

    void HandleTwoPort( const TP *twoPort );


    virtual void CreateLookupTable();

    void SerializeObjectData( const object::Object< double > *object ) const;
    template < typename ClassT, typename ReturnT >
    void SerializeDataOfFunction( std::vector< boost::shared_ptr< ClassT > > &vec, ReturnT ( ClassT::*Fct_T )() const ) const;

    template < typename VectorType >
    size_t FindPosition( const std::vector< boost::shared_ptr< VectorType > > &vec, const VectorType &ref ) const;

    void CreateStates() const;

    virtual void Output( const matType *mat ) const;
    virtual void Output( const matType &mat ) const;
    std::vector< std::string > mObjectVector;
    size_t CountObservableTwoPorts();
    size_t CountObservableTwoPorts() const;
    size_t mObservableTwoPorts = 0;
    bool mZarcElementsAlreadyDefined = false;
};

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::Serialize1Dlookup( const lookup::LookupType1D< double > *lookUpType,
                                                                             std::string argument ) const
{
    const size_t numberOfPoints = lookUpType->GetPoints().size();
    ( *this->mStream ) << " x = interp1( [";
    for ( size_t i = 0; i < numberOfPoints; ++i )
    {
        ( *this->mStream ) << lookUpType->GetPoints().at( i );
        if ( i != numberOfPoints - 1 )
            ( *this->mStream ) << ", ";
    }
    ( *this->mStream ) << "], [ ";
    for ( size_t i = 0; i < numberOfPoints; ++i )
    {
        ( *this->mStream ) << lookUpType->GetLookup().at( i );

        if ( i != numberOfPoints - 1 )
            ( *this->mStream ) << ", ";
    }
    ( *this->mStream ) << "], " << argument << ");\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateSOCFunction3() const
{
    ( *this->mStream ) << "function UpdateHalfStates(t_ret, x, I_base,";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }
    ( *this->mStream ) << ")\n";

    // Fast return, we have no states
    if ( this->mElementList.size() == 0 )
    {
        ( *this->mStream ) << "end\n";
        return;
    }

    DefineGlobalStates();
    HelperStates();

    size_t cellElements = CountCellelements( this->mRootPort );
    ( *this->mStream ) << "count = 0;\n"
                       << "cur_vec = zeros(length(t_ret)," << cellElements << ");\n "
                       << "for i = 1:length(t_ret)\n";
    auto &vec = this->mSimulation->mSocStates;
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( this->mElementList[i]->IsCellelement() )
        {
            auto tp = static_cast< const electrical::Cellelement< matType > * >( this->mElementList[i] );

            if ( tp->GetSoc() )
            {
                const size_t pos = FindPosition( vec, *tp->GetSoc() ) + 1;
                ( *this->mStream ) << "cur_vec(i, " << pos << ") = [";
                matType I = tp->GetCurrent();
                I = I * this->mSimulation->mStateSystemGroup.mStateVector;
                Output( I );
                ( *this->mStream ) << "];\n";
            }
            /*
        if ( tp->GetThermalState() )
        {
            const size_t pos = FindPosition( this->mSimulation->mThermalStates, tp->GetThermalState() );
            ( *this->mStream ) << "T(" << pos + 1 << ") = i * ";
            matType I = *this->mElementList[i]->GetCurrent();
            I = I * this->mSimulation->mStateSystemGroup.mStateVector;
            Output( I );
            ( *this->mStream ) << ";\n";
        }
            */
        }
    }
    ( *this->mStream ) << "count = 1+count;\n"
                       << "end\n";
    for ( size_t i = 0; i < cellElements; ++i )
    {
        ( *this->mStream ) << "CAP_CUR(" << ( i + 1 ) << ") = CAP_CUR(" << ( i + 1 ) << ") + "
                           << "trapz( t_ret, cur_vec(:," << ( i + 1 ) << "));\n";
    }


    ( *this->mStream ) << "SOC = CAP_CUR ./ CAP * 100;\n";
    ( *this->mStream ) << "end\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateSOCFunction1() const
{
    ( *this->mStream ) << "function UpdateHalfStates(t_ret, x, I_base,";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }
    ( *this->mStream ) << ")\n";

    // Fast return, we have no states
    if ( this->mElementList.size() == 0 )
    {
        ( *this->mStream ) << "end\n";
        return;
    }

    DefineGlobalStates();
    HelperStates();

    size_t cellElements = CountCellelements( this->mRootPort );
    ( *this->mStream ) << "count = 0;\n"
                       << "cur_vec = zeros(length(t_ret)," << cellElements << ");\n "
                       << "for i = 1:length(t_ret)\n";

    ( *this->mStream ) << "cur_vec(i,:) = ([";
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( this->mElementList[i]->IsCellelement() )
        {
            auto tp = static_cast< const electrical::Cellelement< matType > * >( this->mElementList[i] );

            if ( tp->GetSoc() )
            {
                matType I = tp->GetCurrent();
                Output( I );
                if ( i != this->mElementList.size() - 1 )
                    ( *this->mStream ) << "; ... \n" << std::endl;
            }
            /*
        if ( tp->GetThermalState() )
        {
            const size_t pos = FindPosition( this->mSimulation->mThermalStates, tp->GetThermalState() );
            ( *this->mStream ) << "T(" << pos + 1 << ") = i * ";
            matType I = *this->mElementList[i]->GetCurrent();
            I = I * this->mSimulation->mStateSystemGroup.mStateVector;
            Output( I );
            ( *this->mStream ) << ";\n";
        }
            */
        }
    }
    ( *this->mStream ) << "] * [x,1]')';\n";
    ( *this->mStream ) << "count = 1+count;\n"
                       << "end\n";
    for ( size_t i = 0; i < cellElements; ++i )
    {
        ( *this->mStream ) << "CAP_CUR(" << ( i + 1 ) << ") = CAP_CUR(" << ( i + 1 ) << ") + "
                           << "trapz( t_ret, cur_vec(:," << ( i + 1 ) << "));\n";
    }


    ( *this->mStream ) << "SOC = CAP_CUR ./ CAP * 100;\n";
    ( *this->mStream ) << "end\n";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateSOCFunction() const
{
    if ( DEPTH == 3 )
        CreateSOCFunction3();
    else if ( DEPTH == 1 )
        CreateSOCFunction1();
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
size_t MatlabExport< matType, ExportClass, Parser, DEPTH >::CountObservableTwoPorts() const
{
    size_t returnValue = 0;
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( this->mElementList[i]->IsObservable() )
            ++returnValue;
    }
    return returnValue;
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
size_t MatlabExport< matType, ExportClass, Parser, DEPTH >::CountObservableTwoPorts()
{
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( this->mElementList[i]->IsObservable() )
            ++mObservableTwoPorts;
    }
    return mObservableTwoPorts;
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOutputFuncton() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream )
     << "function V = VoltageOutput(t, state, t_cur, cur, soc_vec, temp_vec, majorStep, internalValues)\n";

    DefineGlobalStates();
    ( *this->mStream ) << "V = zeros(length(t)," << CountObservableTwoPorts() << ");\n"
                       << "[C, ia, ic] = unique(majorStep);\n"
                       << "timeCounter = 1;\n"
                       << "for i = 1: length(t)\n"
                       << " for j = 1: size(soc_vec,2)\n"
                       << "  SOC(j) = interp1( t(majorStep(ia)), soc_vec((ia),j), t(i), 'previous');\n"
                       << " end\n"
                       << " for j = 1: size(temp_vec,2)\n"
                       << "  T(j) = interp1( t(majorStep(ia)), temp_vec(ia,j), t(i), 'previous');\n"
                       << " end\n"
                       << " if i >majorStep(timeCounter) && timeCounter + 1 < length(majorStep)\n"
                       << "timeCounter = timeCounter + 1;\n"
                       << "end\n";

    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i ) << " = internalValues(timeCounter," << i + 1 << ");\n";
    }


    ( *this->mStream ) << " I_base = interp1( t_cur, cur, t(i), 'previous');\n"
                       << " x = state(i,:);\n";

    if ( DEPTH == 3 )
        CreateOutputFuncton3();
    else if ( DEPTH == 1 )
        CreateOutputFuncton1();

    ( *this->mStream ) << " end\n"
                       << "end\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOutputFuncton3() const
{
    ( *this->mStream ) << "V(i,:) = [";

    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( !this->mElementList[i]->IsObservable() )
            continue;

        matType V = *this->mElementList[i]->GetConstVoltage();
        V = V * this->mSimulation->mStateSystemGroup.mStateVector;
        Output( V );
        if ( i != this->mElementList.size() - 1 )
            ( *this->mStream ) << ", ... \n" << std::endl;
    }
    ( *this->mStream ) << "];\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOutputFuncton1() const
{
    ( *this->mStream ) << "V(i,:) = ([";

    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        if ( !this->mElementList[i]->IsObservable() )
            continue;

        matType V = *this->mElementList[i]->GetConstVoltage();
        // V = V * this->mSimulation->mStateSystemGroup.mStateVector;
        Output( V );
        if ( i != this->mElementList.size() - 1 )
            ( *this->mStream ) << "; ... \n" << std::endl;
    }
    ( *this->mStream ) << "] * [x,1]')';\n";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::BaseFunction() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream ) << "function [t_vec, V, i, soc_vec, temp_vec] = basefunction()\n";

    DefineGlobalStates();
    HelperStates();
    ( *this->mStream ) << "% i =[10,10,0]; t = [0,10,20];  %example input\n";

    CreateStates();
    ( *this->mStream ) << "t_cur = union(t,t(1):5:t(end));\n"
                       << "i_cur = interp1( t, i, t_cur, 'previous');\n"
                       << "[t_vec, x_vec, soc_vec, temp_vec, majorStep, internalValues] = Stepper(t_cur,i_cur, t);\n"
                       << "V = VoltageOutput(t_vec, x_vec, t, i, soc_vec, temp_vec, majorStep, internalValues )\n"
                       << "end\n";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOdeFunction3() const
{
    const auto size = this->mSimulation->mStateSystemGroup.GetStateCount();
    const matType state = this->mSimulation->mStateSystemGroup.mStateVector.topRows( size );
    ( *this->mStream )
     << "function [time, state, localSOC, localT, majorStep,internalValues] = Stepper(t, current, t_major)\n"
     << "majorStep = [1];\n"
     << "time = [];\n"
     << "state =[];\n"
     << "internalValues =[];\n"
     << "I_base = 0;\n;";
    DefineGlobalStates();
    ( *this->mStream ) << "localSOC =SOC;\n"
                       << "localT =T;\n";
    CreateStateVector();
    ( *this->mStream ) << " vopt = odeset (\'RelTol\', 1e-8, \'AbsTol\', 1e-8,\'NormControl\', \'on\');\n"
                       << " for i = 1: (length(t)-1)\n"
                       << "  if find(t_major == t(i))\n"
                       << "   t(i)\n"
                       << "  end\n";
    for ( const auto &name : mObjectVector )
    {
        ( *this->mStream ) << name << " = fct_" << name << ";\n";
    }


    ( *this->mStream ) << "internalValues = [internalValues; ";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }

    ( *this->mStream ) << " ];\n"
                       << "  I_base = current(i);\n"
                       << " x = [";
    Output( state );
    ( *this->mStream ) << "];\n"
                       << "  [t_ret, y] = ode45 (@CalculateDX, [t(i) t(i+1)], x,vopt,I_base, ";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }


    ( *this->mStream ) << ");\n"
                       << "  time = [time; t_ret(1)];\n"
                       << "  state = [state; y(1,:)];\n"
                       << "  x = y(end,:)';\n"
                       << "  majorStep = [majorStep; length(time)];\n"
                       << "  localSOC = [localSOC; SOC];\n"
                       << "  localT = [localT; T];\n"
                       << "  UpdateHalfStates(t_ret, x, I_base, ";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }

    ( *this->mStream ) << ");\n"
                       << " end\n"
                       << "time = [time; t_ret(end)];\n"
                       << "state = [state; y(end,:)];\n"
                       << "end\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOdeFunction1() const
{
    const auto size = this->mSimulation->mStateSystemGroup.GetStateCount();
    const matType state = this->mSimulation->mStateSystemGroup.mStateVector.topRows( size );
    ( *this->mStream )
     << "function [time, state, localSOC, localT, majorStep,internalValues] = Stepper(t, current, t_major)\n"
     << "majorStep = [1];\n"
     << "time = [];\n"
     << "state =[];\n"
     << "internalValues =[];\n"
     << "I_base = 0;\n;";
    DefineGlobalStates();
    ( *this->mStream ) << "localSOC =SOC;\n"
                       << "localT =T;\n";
    CreateStateVector();
    ( *this->mStream ) << " vopt = odeset (\'RelTol\', 1e-8, \'AbsTol\', 1e-8,\'NormControl\', \'on\');\n"
                       << " for i = 1: (length(t)-1)\n"
                       << "  if find(t_major == t(i))\n"
                       << "   t(i)\n"
                       << "  end\n";

    for ( const auto &name : mObjectVector )
    {
        ( *this->mStream ) << name << " = fct_" << name << ";\n";
    }


    ( *this->mStream ) << "internalValues = [internalValues; ";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }

    ( *this->mStream ) << " ];\n"
                       << "  I_base = current(i);\n";

    ( *this->mStream ) << "A_DAE = [ ";
    Output( this->mSimulation->mEqSystem->GetA() );
    ( *this->mStream ) << "];\n";
    const auto dglsize = this->mSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount();
    // const auto algsize = this->mSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationCount();
    const std::string A_ALG1 = "A_DAE( " + boost::lexical_cast< std::string >( dglsize + 1 ) +
                               ":end, 1:" + boost::lexical_cast< std::string >( dglsize ) + ")";
    const std::string A_ALG2 = "A_DAE( " + boost::lexical_cast< std::string >( dglsize + 1 ) + ":end, " +
                               boost::lexical_cast< std::string >( dglsize + 1 ) + ":end)";

    ( *this->mStream ) << "inv_A_ALG2_ALG1 = " + A_ALG2 + "\\ " + A_ALG1 + ";\n";


    ( *this->mStream ) << "C_DAE = [";
    Output( this->mSimulation->mEqSystem->GetC() );
    ( *this->mStream ) << "];\n";

    // const std::string A_DGL = "A_DAE(1: " + boost::lexical_cast< std::string >( dglsize ) + ",1:end)";
    const std::string C_ALG = "C_DAE(" + boost::lexical_cast< std::string >( dglsize + 1 ) + ":end ,1)";
    ( *this->mStream ) << "A_DGL = A_DAE(1: " + boost::lexical_cast< std::string >( dglsize ) + ",1:end);\n";
    ( *this->mStream ) << "C_DGL = C_DAE(1:" + boost::lexical_cast< std::string >( dglsize ) + " ,1);\n";
    ( *this->mStream ) << "C_ODE = [C_DGL; inv_A_ALG2_ALG1 * C_DGL;];\n";


    ( *this->mStream ) << "A_ODE = [A_DGL;  inv_A_ALG2_ALG1 * A_DGL];\n";
    ( *this->mStream ) << "x = [x(1:" + boost::lexical_cast< std::string >( dglsize ) + "); " + A_ALG2 + " \\ (- " +
                           C_ALG + " - " + A_ALG1 + " * x(1:" + boost::lexical_cast< std::string >( dglsize ) + "))];\n";

    ( *this->mStream ) << "  [t_ret, y] = ode45 (@CalculateDX, [t(i) t(i+1)], x, vopt , A_ODE, C_ODE);\n"
                       << "  time = [time; t_ret(1)];\n"
                       << "  state = [state; y(1,:)];\n"
                       << "  x = y(end,:)';\n"
                       << "  majorStep = [majorStep; length(time)];\n"
                       << "  localSOC = [localSOC; SOC];\n"
                       << "  localT = [localT; T];\n"
                       << "  UpdateHalfStates(t_ret, x, I_base, ";

    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }

    ( *this->mStream ) << ");\n";
    ( *this->mStream ) << " end\n"
                       << "time = [time; t_ret(end)];\n"
                       << "state = [state; y(end,:)];\n"
                       << "end\n";

    /*

    Output( state );
( *this->mStream ) << "];\n"
                  << "  [t_ret, y] = ode45 (@CalculateDX, [t(i) t(i+1)], x,vopt,I_base, ";
for ( size_t i = 0; i < mObjectVector.size(); ++i )
{
   ( *this->mStream ) << mObjectVector.at( i );
   if ( i != mObjectVector.size() - 1 )
       ( *this->mStream ) << ", ";
}


( *this->mStream ) << ");\n"
                  << "  time = [time; t_ret];\n"
                  << "  state = [state; y];\n"
                  << "  x = y(end,:)';\n"
                  << "  x = x';\n"
                  << "  majorStep = [majorStep; length(time)];\n"
                  << "  localSOC = [localSOC; SOC];\n"
                  << "  localT = [localT; T];\n"
                  << "  UpdateHalfStates(t_ret, x, I_base, ";
for ( size_t i = 0; i < mObjectVector.size(); ++i )
{
   ( *this->mStream ) << mObjectVector.at( i );
   if ( i != mObjectVector.size() - 1 )
       ( *this->mStream ) << ", ";
}

( *this->mStream ) << ");\n"
                  << " end\n"
                  << "end\n";
                  */
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateOdeFunction() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    if ( DEPTH == 3 )
        CreateOdeFunction3();
    if ( DEPTH == 1 )
        CreateOdeFunction1();
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::DefineDXFunction3() const
{
    ( *this->mStream ) << "function dxdt = CalculateDX(dt, x, I_base, ";
    for ( size_t i = 0; i < mObjectVector.size(); ++i )
    {
        ( *this->mStream ) << mObjectVector.at( i );
        if ( i != mObjectVector.size() - 1 )
            ( *this->mStream ) << ", ";
    }
    ( *this->mStream ) << ")\n";

    matType A_ODE = this->mSimulation->mEqSystem->GetAODE();
    auto C_ODE = this->mSimulation->mEqSystem->GetCODE();
    const auto size = this->mSimulation->mStateSystemGroup.GetStateCount();
    matType x( size, 1 );
    for ( size_t i = 0; i < size; ++i )
    {
        x.coeffRef( i, 0 ) = ScalarUnit( misc::StrCont( "State" ) + misc::StrCont( i + 1 ) );
    }
    matType dxdt = A_ODE * x;
    dxdt += C_ODE;
    ( *this->mStream ) << "dxdt = [ ";
    Output( dxdt );
    ( *this->mStream ) << " ]; \nend\n";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::DefineDXFunction1() const
{

    ( *this->mStream ) << "function dxdt = CalculateDX(dt, x,  A_ODE, C_ODE)\n";
    ( *this->mStream ) << "dxdt = A_ODE * x + C_ODE; \nend\n";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::DefineDXFunction() const
{
    if ( !this->mSimulation || !this->mStream )
        return;
    if ( DEPTH == 3 )
        DefineDXFunction3();
    else if ( DEPTH == 1 )
        DefineDXFunction1();
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateStateVector() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream ) << "x = [ ";
    for ( size_t i = 0; i < this->mSimulation->mStateSystemGroup.GetStateCount(); ++i )
    {
        ( *this->mStream ) << "0";
        if ( i != this->mSimulation->mStateSystemGroup.GetStateCount() - 1 )
        {
            ( *this->mStream ) << "; ";
        }
    }
    ( *this->mStream ) << " ]; \n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::UpdateStates() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream ) << "x = [ ";
    const matType state = this->mSimulation->mStateSystemGroup.mStateVector;
    Output( state );
    ( *this->mStream ) << " ]; \n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
template < typename ClassT, typename ReturnT >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::SerializeDataOfFunction( std::vector< boost::shared_ptr< ClassT > > &vec,
                                                                                   ReturnT ( ClassT::*Fct_T )() const ) const
{
    auto counter = 0u;
    ( *this->mStream ) << "[ ";
    for ( const auto it : vec )
    {
        ( *this->mStream ) << ( ( *it ).*Fct_T )();
        if ( counter != vec.size() - 1 )
            ( *this->mStream ) << ", ";
        ++counter;
    }
    ( *this->mStream ) << " ]; ";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::PrintBaseCurrent() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream ) << "I_base = 0;";
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateStates() const
{
    if ( !this->mSimulation || !this->mStream )
        return;

    ( *this->mStream ) << "SOC = ";
    SerializeDataOfFunction( this->mSimulation->mSocStates,
                             &::electrical::state::Soc::GetValue< ::electrical::state::SocGetFormat::PERCENT > );
    ( *this->mStream ) << "\n";

    ( *this->mStream ) << "CAP = ";
    SerializeDataOfFunction( this->mSimulation->mSocStates,
                             &::electrical::state::Soc::GetInitialCapacity< ::electrical::state::SocGetFormat::AH > );
    ( *this->mStream ) << "\n";

    ( *this->mStream ) << "CAP_CUR = ";
    SerializeDataOfFunction( this->mSimulation->mSocStates,
                             &::electrical::state::Soc::GetValue< ::electrical::state::SocGetFormat::AS > );
    ( *this->mStream ) << "\n";

    ( *this->mStream ) << "T = ";
    SerializeDataOfFunction( this->mSimulation->mThermalStates,
                             &::state::ThermalState< double >::GetValue< TemperatureGetFormat::CELSIUS > );
    ( *this->mStream ) << "\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort( const TP *twoPort )
{
    if ( dynamic_cast< const electrical::Capacity< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::Capacity< matType > * >( twoPort );
        std::string name = "C" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetElementObject() );
        mObjectVector.push_back( name );
    }
    else if ( dynamic_cast< const electrical::OhmicResistance< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::OhmicResistance< matType > * >( twoPort );
        std::string name = "R" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetElementObject() );
        mObjectVector.push_back( name );
    }
    else if ( dynamic_cast< const electrical::VoltageSource< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::VoltageSource< matType > * >( twoPort );
        std::string name = "V" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetElementObject() );
        mObjectVector.push_back( name );
    }
    else if ( dynamic_cast< const electrical::ParallelRC< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::ParallelRC< matType > * >( twoPort );
        std::string name = "R" + boost::lexical_cast< std::string >( twoPort->GetID() );

        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetElementObject() );
        //( *this->mStream ) << "\n";
        mObjectVector.push_back( name );

        name = "Tau" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetTauObject() );
        mObjectVector.push_back( name );
    }
    else if ( dynamic_cast< const electrical::ParallelRCAlg< matType > * >( twoPort ) )
    {

        std::string name = "R" + boost::lexical_cast< std::string >( twoPort->GetID() );
        const auto tp = static_cast< const electrical::ParallelRCAlg< matType > * >( twoPort );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetElementObject() );
        //( *this->mStream ) << "\n";
        mObjectVector.push_back( name );


        name = "C" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetCObject() );
        mObjectVector.push_back( name );
    }
    else if ( dynamic_cast< const electrical::Zarc< matType > * >( twoPort ) )
    {
        std::string name = "R" + boost::lexical_cast< std::string >( twoPort->GetID() );
        const auto tp = static_cast< const electrical::Zarc< matType > * >( twoPort );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetRObject() );
        ( *this->mStream ) << "\n";
        mObjectVector.push_back( name );

        name = "Tau" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetTauObject() );
        ( *this->mStream ) << "\n";
        mObjectVector.push_back( name );

        name = "Phi" + boost::lexical_cast< std::string >( twoPort->GetID() );
        ( *this->mStream ) << "function x = fct_" + name;
        SerializeObjectData( tp->GetPhiObject() );
        ( *this->mStream ) << "\n";
        mObjectVector.push_back( name );

        std::vector< const lookup::LookupType1D< double > * > objectVec = {
         tp->GetLookupPhi2RFactorAussen().GetLookupType().get(), tp->GetLookupPhi2RFactorInnen().GetLookupType().get(),
         tp->GetLookupPhi2RFactorAussen().GetLookupType().get()};
        std::vector< std::string > nameVec = {"LookupPhi2RFactorAussen", "LookupPhi2RFactorInnen",
                                              "LookupPhi2TauFactor"};

        assert( objectVec.size() == nameVec.size() );

        for ( size_t i = 0; i < objectVec.size(); ++i )
        {

            auto object = objectVec[i];
            ( *this->mStream ) << "function x = fct_" << nameVec[i] << twoPort->GetID() << "()\n";
            std::string arg = "fct_Phi" + boost::lexical_cast< std::string >( twoPort->GetID() ) + "()";
            Serialize1Dlookup( object, arg );
            ( *this->mStream ) << "end\n";
            mObjectVector.push_back( nameVec.at( i ) + boost::lexical_cast< std::string >( twoPort->GetID() ) );
        }
    }
    else if ( dynamic_cast< const electrical::ZarcAlg< matType > * >( twoPort ) )
    {
        // const auto tp = static_cast< const electrical::ZarcAlg< matType > * >( twoPort );
    }
    else if ( dynamic_cast< const electrical::Rmphn< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::Rmphn< matType > * >( twoPort );
        auto obj_vec = tp->GetObjects();
        std::vector< std::string > namePrefix = {"MP", "CT"};
        for ( size_t i = 0; i < namePrefix.size(); ++i )
        {
            std::string name = namePrefix.at( i ) + boost::lexical_cast< std::string >( twoPort->GetID() );
            ( *this->mStream ) << "function x = fct_" + name;
            SerializeObjectData( obj_vec.at( i ).get() );
            mObjectVector.push_back( name );
        }
    }
    else if ( dynamic_cast< const electrical::SphericalDiffusion< matType > * >( twoPort ) )
    {
        const auto tp = static_cast< const electrical::SphericalDiffusion< matType > * >( twoPort );

        auto obj_vec = tp->GetObjects();
        std::vector< std::string > namePrefix = {"R", "Tau"};
        for ( size_t i = 0; i < namePrefix.size(); ++i )
        {
            std::string name = namePrefix.at( i ) + boost::lexical_cast< std::string >( twoPort->GetID() );
            ( *this->mStream ) << "function x = fct_" + name;
            SerializeObjectData( obj_vec.at( i ).get() );
            mObjectVector.push_back( name );
        }
    }
    else
    {
        std::cout << twoPort->GetName() << std::endl;
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownElement" );
        // ERROR
    }
    //    ( *this->mStream ) << "; \n";
}

/*
template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::Capacity< matType > * >(
const
TP
*twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::OhmicResistance< matType
> *
>(
const TP *twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::VoltageSource< matType >
* >(
const
TP *twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::ParallelRC< matType > *
>(
const
TP
*twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::ParallelRCAlg< matType >
* >(
const
TP *twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::Zarc< matType > * >(
const TP
*twoPort ) const;

template < typename matType, typename ExportClass,  typename Parser, size_t DEPTH >
template <>
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HandleTwoPort< const electrical::ZarcAlg< matType > * >(
const
TP
*twoPort ) const;
*/


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::CreateLookupTable()
{
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        auto twoPort = this->mElementList[i];
        if ( twoPort->HasChildren() && std::string( twoPort->GetName() ) != "Rmphn" )
            continue;
        HandleTwoPort( twoPort );

        /*
if ( dynamic_cast< const electrical::Capacity< matType > * >( twoPort ) ||
     dynamic_cast< const electrical::OhmicResistance< matType > * >( twoPort ) ||
     dynamic_cast< const electrical::VoltageSource< matType > * >( twoPort ) )
{
    SerializeElectricalElement( twoPort );
}
// Elements with several objects
else if ( dynamic_cast< const electrical::ParallelRC< matType > * >( twoPort ) ||
          dynamic_cast< const electrical::ParallelRCAlg< matType > * >( twoPort ) )
{
    //            SerializeParallelRC( twoPort );
}
else if ( dynamic_cast< const electrical::Zarc< matType > * >( twoPort ) ||
          dynamic_cast< const electrical::ZarcAlg< matType > * >( twoPort ) )
{
    //            SerializeZarc( twoPort );
}
else
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing" );
}
*/
    }
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::HelperStates() const
{
    std::vector< std::string > globalStates = {"CAP", "CAP_CUR"};
    for ( size_t i = 0; i < globalStates.size(); ++i )
    {
        ( *this->mStream ) << "global " << globalStates[i] << ";\n";
    }
}
template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::DefineGlobalStates() const
{
    std::vector< std::string > globalStates = {"SOC", "T"};
    for ( size_t i = 0; i < globalStates.size(); ++i )
    {
        ( *this->mStream ) << "global " << globalStates[i] << ";\n";
    }
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::SerializeElectricalElement( const TP * /*two*/ )
{
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
template < typename VectorType >
size_t MatlabExport< matType, ExportClass, Parser, DEPTH >::FindPosition( const std::vector< boost::shared_ptr< VectorType > > &vec,
                                                                          const VectorType &ref ) const
{
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        if ( vec[i].get() == &ref )
            return i;
    }

    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownElement" );
    return 0;    // Disabling warngings from gcc
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::SerializeObjectData( const object::Object< double > *object ) const
{
    if ( dynamic_cast< const object::LookupObj2dWithState< double > * >( object ) )
    {
        const object::LookupObj2dWithState< double > *lookupObj2dWithState =
         static_cast< const object::LookupObj2dWithState< double > * >( object );
        const lookup::LookupType2D< double > *loopUpType = lookupObj2dWithState->GetLookup().GetLookupType().get();


        ( *this->mStream ) << "()\n";
        DefineGlobalStates();
        ( *this->mStream ) << " x = interp2(";

        const size_t numberOfCols = loopUpType->GetPointsCol().size();
        const size_t numberOfRows = loopUpType->GetPointsRow().size();
        /// Check if state is SOC
        if ( dynamic_cast< const electrical::state::Soc * >( lookupObj2dWithState->GetStateRow() ) )
        {

            ( *this->mStream ) << "[";
            for ( size_t i = 0; i < numberOfRows; ++i )
            {
                ( *this->mStream ) << loopUpType->GetPointsRow().at( i );
                if ( i != numberOfRows - 1 )
                    ( *this->mStream ) << ", ";
            }
            ( *this->mStream ) << "], [";
            for ( size_t i = 0; i < numberOfCols; ++i )
            {
                ( *this->mStream ) << loopUpType->GetPointsCol().at( i );
                if ( i != numberOfCols - 1 )
                    ( *this->mStream ) << ", ";
            }
            ( *this->mStream ) << "], [";

            // Matlab organizes its matrices in coloumn-major order
            for ( size_t j = 0; j < numberOfCols; ++j )
            {
                for ( size_t i = 0; i < numberOfRows; ++i )
                {
                    ( *this->mStream ) << loopUpType->GetLookup().at( j ).at( i );
                    if ( i != numberOfRows - 1 )
                        ( *this->mStream ) << ", ";
                }
                ( *this->mStream ) << ";\n";
            }
            ( *this->mStream ) << "], ";

            auto stateRow = static_cast< const electrical::state::Soc * >( lookupObj2dWithState->GetStateRow() );
            auto &vecRow = this->mSimulation->mSocStates;
            auto posRow = FindPosition( vecRow, *stateRow ) + 1;

            auto stateCol = static_cast< const ::state::ThermalState< double > * >( lookupObj2dWithState->GetStateCol() );
            auto &vecCol = this->mSimulation->mThermalStates;
            auto posCol = FindPosition( vecCol, *stateCol ) + 1;

            ( *this->mStream ) << "SOC(" << posCol << "), "
                               << "T(" << posRow << "));\n"
                               << "end\n";
        }
        /// Check if state is Temperature
        else
        {
            ( *this->mStream ) << "[";
            for ( size_t i = 0; i < numberOfRows; ++i )
            {
                ( *this->mStream ) << loopUpType->GetPointsRow().at( i );
                if ( i != numberOfRows - 1 )
                    ( *this->mStream ) << ", ";
            }
            ( *this->mStream ) << "], [";
            for ( size_t i = 0; i < numberOfCols; ++i )
            {
                ( *this->mStream ) << loopUpType->GetPointsCol().at( i );
                if ( i != numberOfCols - 1 )
                    ( *this->mStream ) << ", ";
            }
            ( *this->mStream ) << "], [";

            // Matlab organizes its matrices in coloumn-major order
            for ( size_t j = 0; j < numberOfCols; ++j )
            {
                for ( size_t i = 0; i < numberOfRows; ++i )
                {
                    ( *this->mStream ) << loopUpType->GetLookup().at( j ).at( i );

                    if ( i != numberOfRows - 1 )
                        ( *this->mStream ) << ", ";
                }
                ( *this->mStream ) << ";\n";
            }
            ( *this->mStream ) << "], ";

            auto stateCol = static_cast< const electrical::state::Soc * >( lookupObj2dWithState->GetStateCol() );
            auto &vecCol = this->mSimulation->mSocStates;
            auto posCol = FindPosition( vecCol, *stateCol ) + 1;

            auto stateRow = static_cast< const ::state::ThermalState< double > * >( lookupObj2dWithState->GetStateRow() );
            auto &vecRow = this->mSimulation->mThermalStates;
            auto posRow = FindPosition( vecRow, *stateRow ) + 1;

            ( *this->mStream ) << "T(" << posCol << "), "
                               << "SOC(" << posRow << "));\n"
                               << "end\n";

            /*
            const size_t numberOfRows = loopUpType->GetPointsRow().size();
            for ( size_t i = 0; i < numberOfRows; ++i )
                ( *this->mStream ) << loopUpType->GetPointsRow().at( i );

            const size_t numberOfCols = loopUpType->GetPointsCol().size();
            for ( size_t i = 0; i < numberOfCols; ++i )
                ( *this->mStream ) << loopUpType->GetPointsCol().at( i );

            // Matlab organizes its matrices in coloumn-major order
            for ( size_t j = 0; j < numberOfCols; ++j )
            {
                for ( size_t i = 0; i < numberOfRows; ++i )
                {
                    ( *this->mStream ) << loopUpType->GetLookup().at( j ).at( i );
                }
                ( *this->mStream ) << ";\n";
            }
            */
        }
    }
    else if ( dynamic_cast< const object::LookupObj1dWithState< double > * >( object ) )
    {
        const lookup::LookupType1D< double > *loopUpType =
         static_cast< const object::LookupObj1dWithState< double > * >( object )->GetLookup().GetLookupType().get();

        ( *this->mStream ) << "()\n";
        const size_t numberOfPoints = loopUpType->GetPoints().size();
        DefineGlobalStates();
        ( *this->mStream ) << " x = interp1( [";
        for ( size_t i = 0; i < numberOfPoints; ++i )
        {
            ( *this->mStream ) << loopUpType->GetPoints().at( i );
            if ( i != numberOfPoints - 1 )
                ( *this->mStream ) << ", ";
        }
        ( *this->mStream ) << "], [ ";
        for ( size_t i = 0; i < numberOfPoints; ++i )
        {
            ( *this->mStream ) << loopUpType->GetLookup().at( i );

            if ( i != numberOfPoints - 1 )
                ( *this->mStream ) << ", ";
        }
        auto castObje = static_cast< const object::LookupObj1dWithState< double > * >( object );
        if ( dynamic_cast< const electrical::state::Soc * >( castObje->GetState() ) )
        {
            auto state = static_cast< const electrical::state::Soc * >( castObje->GetState() );
            auto &vec = this->mSimulation->mSocStates;
            auto pos = FindPosition( vec, *state );
            ( *this->mStream ) << "], SOC(" + boost::lexical_cast< std::string >( pos + 1 ) + ")";
        }
        if ( dynamic_cast< const ::state::ThermalState< double > * >( castObje->GetState() ) )
        {
            auto state = static_cast< const ::state::ThermalState< double > * >( castObje->GetState() );
            auto &vec = this->mSimulation->mThermalStates;
            auto pos = FindPosition( vec, *state );
            ( *this->mStream ) << "], T(" + boost::lexical_cast< std::string >( pos + 1 ) + ")";
        }


        ( *this->mStream ) << ");\n"
                           << "end\n";
    }
    else if ( dynamic_cast< const object::ConstObj< double > * >( object ) )
    {
        ( *this->mStream ) << "()\n x = " << object->GetValue() << ";\nend\n";
    }
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorObject", "2" );
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::PrintVoltageMatrix()
{
    if ( !this->mStream || !this->mSimulation )
        return;

    ( *this->mStream ) << " U = [";
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {
        matType V = *this->mElementList[i]->GetConstVoltage();
        V = V * this->mSimulation->mStateSystemGroup.mStateVector;
        Output( V );
        if ( i != this->mElementList.size() - 1 )
            ( *this->mStream ) << "; ... \n" << std::endl;
    }

    ( *this->mStream ) << " ]\n";

    ( *this->mStream ) << " I = [ ";
    for ( size_t i = 0; i < this->mElementList.size(); ++i )
    {

        Output( this->mElementList[i]->GetCurrent() * this->mSimulation->mStateSystemGroup.mStateVector );

        if ( i != this->mElementList.size() - 1 )
            ( *this->mStream ) << "; ... \n" << std::endl;
    }
    ( *this->mStream ) << " ]\n";


    ( *this->mStream ) << " A = [ ";
    Output( this->mSimulation->mEqSystem->GetA() );
    ( *this->mStream ) << "]\n";


    ( *this->mStream ) << " C_DAE = [ ";
    Output( this->mSimulation->mEqSystem->GetC() );
    ( *this->mStream ) << "]\n";

    matType A_ODE = this->mSimulation->mEqSystem->GetAODE();

    ( *this->mStream ) << "A_ODE =[ ";
    Output( A_ODE );
    ( *this->mStream ) << "]\n";

    ( *this->mStream ) << " C_ODE = [ ";
    Output( this->mSimulation->mEqSystem->GetCODE() );
    ( *this->mStream ) << "]\n";
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::Output( const matType *mat ) const
{
    if ( !mat )
    {
        ( *this->mStream ) << " ";
        return;
    }
    this->Output( *mat );
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void MatlabExport< matType, ExportClass, Parser, DEPTH >::Output( const matType &mat ) const
{
    std::vector< std::vector< std::string > > tmp_vector{static_cast< size_t >( mat.rows() ),
                                                         std::vector< std::string >( mat.cols() )};
    boost::thread_group group;
    for ( size_t i = 0; i < mat.rows(); ++i )
    {
        for ( size_t j = 0; j < mat.cols(); ++j )
        {
            if ( mat.coeff( i, j ).IsEmpty() )
            {
                tmp_vector[i][j] = " 0 ";
                continue;
            }
            group.add_thread( new boost::thread( boost::bind( &ExportSystem< matType, ExportClass, Parser, DEPTH >::PreParse,
                                                              this, mat.coeff( i, j ), boost::ref( tmp_vector[i][j] ) ) ) );
        }
    }
    group.join_all();

    for ( size_t i = 0; i < mat.rows(); ++i )
    {
        for ( size_t j = 0; j < mat.cols(); ++j )
        {
            if ( j != 0 )
            {
                ( *this->mStream ) << " , ";
            }
            ( *this->mStream ) << tmp_vector[i][j];
        }
        if ( i != mat.rows() - 1 )
            ( *this->mStream ) << "; ... \n";
    }
}
}    // namespace symbolic
#endif /* _SYMBOLIC_ */

#endif /* _MATLABEXPORT_ */
