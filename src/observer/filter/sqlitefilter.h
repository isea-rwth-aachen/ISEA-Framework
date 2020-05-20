/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : sqlitefilter.h
* Creation Date : 05-01-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifdef _SQLITE_
#ifndef _SQLITEFILTER_
#define _SQLITEFILTER_
/// DESCRIPTION
#include <sqlite3.h>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "../observer.h"
#include "../../electrical/cellelement.h"
#include "../../thermal/blocks/elements/thermal_element.h"

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class SqliteFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    typedef Filter< T, TConcrete, ArgumentType > FilterT;


    SqliteFilter( std::string filename )
        : FilterT()
    {
        int rc = sqlite3_open( filename.c_str(), &mDb );
        if ( rc )
        {
            fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg( mDb ) );
            return;
        }
    };


    virtual ~SqliteFilter()
    {
        if ( mErrorMsg )
        {
            sqlite3_free( mErrorMsg );
            mErrorMsg = 0;
        }
        sqlite3_close( mDb );
    }

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        UNUSED( data );
        FilterT::ProcessData( data, t );
    };
    virtual void PrepareFilter( ArgumentType & /* prepData*/ ){};

    protected:
    sqlite3 *mDb{0};
    char *mErrorMsg{0};
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class SqliteFilterBase : public SqliteFilter< T, TConcrete, ArgumentType >
{

    public:
    virtual void PrepareFilter( ArgumentType &prepData ) = 0;
    SqliteFilterBase( std::string filename )
        : SqliteFilter< T, TConcrete, ArgumentType >( filename ){};
};

template < typename T >
class SqliteFilterBase< T, electrical::TwoPort, PreparationType< T > >
 : public SqliteFilter< T, electrical::TwoPort, PreparationType< T > >
{

    private:
    electrical::TwoPort< T > *mRootPort{0};

    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    public:
    SqliteFilterBase( std::string filename )
        : SqliteFilter< T, electrical::TwoPort, PreparationType< T > >( filename )
    {

        std::string sql =
         "CREATE TABLE electrical_elements("
         "ID INT PRIMARY KEY     NOT NULL,"
         "NAME           TEXT    NOT NULL);";
        int rc = sqlite3_exec( this->mDb, sql.c_str(), 0, 0, &this->mErrorMsg );
        if ( rc != SQLITE_OK )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SQLPassThrough", this->mErrorMsg,
                                                 sql.c_str() );    // Should result in error
        }
    };

    ~SqliteFilterBase() {}

    virtual void PrepareFilter( PreparationType< T > &prePareData ) { mRootPort = prePareData.mRootPort; };


    void InitializedDataBaseEntries( const typename FilterT::Data_t &data )
    {
        std::string sql =
         "CREATE TABLE ROOTPORT"
         "(TIME REAL, VOLTAGE         REAL,"
         "CURRENT         REAL,"
         "POWER         REAL);";

        int rc = sqlite3_exec( this->mDb, sql.c_str(), 0, 0, &this->mErrorMsg );
        if ( rc != SQLITE_OK )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SQLPassThrough", this->mErrorMsg,
                                                 sql.c_str() );    // Should result in error
        }

        for ( size_t i = 0; i < data.size(); ++i )
        {
            mTwoPortNamesForDatabase.push_back( data[i]->GetName() + boost::lexical_cast< std::string >( i ) );
            sql = "CREATE TABLE " + mTwoPortNamesForDatabase[i] +
                  "(TIME REAL,"
                  "VOLTAGE         REAL,"
                  "CURRENT         REAL,"
                  "POWER         REAL,"
                  "TEMPERATURE         REAL,"
                  "SOC         REAL );";
            sql += "INSERT INTO electrical_elements (ID, NAME) VALUES ( " +
                   boost::lexical_cast< std::string >( i + 1 ) + ", '" + mTwoPortNamesForDatabase[i] + "');";

            int rc = sqlite3_exec( this->mDb, sql.c_str(), 0, 0, &this->mErrorMsg );
            if ( rc != SQLITE_OK )
            {
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SQLPassThrough",
                                                     this->mErrorMsg, sql.c_str() );    // Should result in error
            }
        }
        mInitialized = true;
    }

    virtual void ProcessData( const typename FilterT::Data_t &data, const double t )
    {
        if ( !mInitialized )
            InitializedDataBaseEntries( data );
        std::string query = "";

        std::string singleEntry = "";
        for ( size_t i = 0; i < data.size(); ++i )
        {
            electrical::TwoPort< T > *port = data[i];
            singleEntry = "INSERT INTO " + mTwoPortNamesForDatabase[i] +
                          "( TIME, VOLTAGE, CURRENT, POWER, TEMPERATURE, SOC) VALUES (" +
                          boost::lexical_cast< std::string >( t ) + ", " +
                          boost::lexical_cast< std::string >( port->GetVoltageValue() ) + ", " +
                          boost::lexical_cast< std::string >( port->GetCurrentValue() ) + ", " +
                          boost::lexical_cast< std::string >( port->GetPowerValue() ) + ", ";

            if ( port->IsCellelement() )
            {
                electrical::Cellelement< T > *cell = static_cast< electrical::Cellelement< T > * >( port );
                singleEntry += boost::lexical_cast< std::string >( cell->GetSocStateValue() ) + ", " +
                               boost::lexical_cast< std::string >( cell->GetThermalState()->GetValue() );
            }
            else
            {
                singleEntry += "-1.0, -273";
            }
            singleEntry += "); ";
            query += singleEntry;
        }

        if ( mRootPort )
        {
            electrical::TwoPort< T > *port = mRootPort;
            singleEntry = "INSERT INTO ROOTPORT( TIME, VOLTAGE, CURRENT, POWER) VALUES (" +
                          boost::lexical_cast< std::string >( t ) + ", " +
                          boost::lexical_cast< std::string >( port->GetVoltageValue() ) + ", " +
                          boost::lexical_cast< std::string >( port->GetCurrentValue() ) + ", " +
                          boost::lexical_cast< std::string >( port->GetPowerValue() ) + ");";
            query += singleEntry;
        }

        int rc = sqlite3_exec( this->mDb, query.c_str(), 0, 0, &this->mErrorMsg );
        if ( rc != SQLITE_OK )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SQLPassThrough", this->mErrorMsg,
                                                 query.c_str() );    // Should result in error
        }
        SqliteFilter< T, electrical::TwoPort, PreparationType< T > >::ProcessData( data, t );
    }

    private:
    bool mInitialized{false};
    std::vector< std::string > mTwoPortNamesForDatabase;
};

template < typename T >
class SqliteFilterTwoPort : public SqliteFilterBase< T, electrical::TwoPort, PreparationType< T > >
{
    public:
    SqliteFilterTwoPort( std::string filename )
        : SqliteFilterBase< T, electrical::TwoPort, PreparationType< T > >( filename ){};
};
}

#endif /* _SQLITEFILTER_ */
#endif /* _SQLITE_ */
