#include "../geometry/geometry2D.h"
#include "../geometry/area.h"
#include "../geometry/block_geometry.h"
#include "../geometry/plane.h"

#include "Geometry2DTest.h"

#ifndef _SYMBOLIC_

#define TS_ASSERT( var1 )    \
    {                        \
        bool var = var1;     \
        if ( !var )          \
        {                    \
            return __LINE__; \
        }                    \
    }
#define TS_ASSERT_EQUALS( var1, var2 ) \
    {                                  \
        double v1 = var1;              \
        double v2 = var2;              \
        if ( v1 != v2 )                \
        {                              \
            return __LINE__;           \
        }                              \
    }
#define TS_ASSERT_DELTA( var1, var2, vDelta ) \
    {                                         \
        double v1 = var1;                     \
        double v2 = var2;                     \
        if ( fabs( v1 - v2 ) > vDelta )       \
        {                                     \
            return __LINE__;                  \
        }                                     \
    }


unsigned int CheckMacros1()
{
    TS_ASSERT( false );
    return 0;
}
unsigned int CheckMacros2()
{
    TS_ASSERT_EQUALS( 0, 1 );
    return 0;
}
unsigned int CheckMacros3()
{
    TS_ASSERT_DELTA( 0.4, 0.6, 0.1 );
    return 0;
}
unsigned int CheckMacros4()
{
    TS_ASSERT_DELTA( 0.8, 0.6, 0.1 );
    return 0;
}
unsigned int CheckMacros5()
{
    TS_ASSERT( true );
    return 0;
}
unsigned int CheckMacros6()
{
    TS_ASSERT_EQUALS( 3.2, 3.2 );
    return 0;
}
unsigned int CheckMacros7()
{
    TS_ASSERT_DELTA( 0.55, 0.6, 0.1 );
    return 0;
}
unsigned int CheckMacros8()
{
    TS_ASSERT_DELTA( 0.65, 0.6, 0.1 );
    return 0;
}


unsigned int Geometry2DTest()
{
    using namespace geometry;
    using namespace thermal;
    using std::vector;
    const double sDelta = 0.000001;


    if ( CheckMacros1() == 0 )
        return __LINE__;
    if ( CheckMacros2() == 0 )
        return __LINE__;
    if ( CheckMacros3() == 0 )
        return __LINE__;
    if ( CheckMacros4() == 0 )
        return __LINE__;
    if ( CheckMacros5() != 0 )
        return __LINE__;
    if ( CheckMacros6() != 0 )
        return __LINE__;
    if ( CheckMacros7() != 0 )
        return __LINE__;
    if ( CheckMacros8() != 0 )
        return __LINE__;


    vector< TwoDim<> > vertices( 4 );

    vertices[0] = TwoDim<>( 0.0, 0.0 );
    vertices[1] = TwoDim<>( 0.0, 4.0 );
    vertices[2] = TwoDim<>( 6.0, 4.0 );
    vertices[3] = TwoDim<>( 6.0, 0.0 );
    Geometry2D<> big( vertices );

    vertices[0] = TwoDim<>( 3.0, 1.0 );
    vertices[1] = TwoDim<>( 5.0, 1.0 );
    vertices[2] = TwoDim<>( 5.0, 3.0 );
    vertices[3] = TwoDim<>( 3.0, 3.0 );
    Geometry2D<> small( vertices );

    vertices.resize( 8 );
    vertices[0] = TwoDim<>( 0.0, 2.0 );
    vertices[1] = TwoDim<>( 0.0, 6.0 );
    vertices[2] = TwoDim<>( 7.0, 6.0 );
    vertices[3] = TwoDim<>( 7.0, 2.0 );
    vertices[4] = TwoDim<>( 4.0, 2.0 );
    vertices[5] = TwoDim<>( 4.0, 5.0 );
    vertices[6] = TwoDim<>( 2.0, 5.0 );
    vertices[7] = TwoDim<>( 2.0, 2.0 );
    Geometry2D<> wound( vertices );


    TS_ASSERT_DELTA( small.CalculateAreaValue(), 4.0, sDelta );
    TS_ASSERT_DELTA( big.CalculateAreaValue(), 24.0, sDelta );
    TS_ASSERT_DELTA( wound.CalculateAreaValue(), 22.0, sDelta );


    vector< Geometry2D<> > intersection;

    small.Intersection( big, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue(), 4.0, sDelta );


    big.Intersection( small, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue(), 4.0, sDelta );

    small.Intersection( wound, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue(), 1.0, sDelta );


    wound.Intersection( small, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue(), 1.0, sDelta );

    big.Intersection( wound, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 2 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue() + intersection[1].CalculateAreaValue(), 8.0, sDelta );


    wound.Intersection( big, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 2 );
    TS_ASSERT_DELTA( intersection[0].CalculateAreaValue() + intersection[1].CalculateAreaValue(), 8.0, sDelta );


    Tolerance<> tolerance( 0.15 );

    vertices.resize( 4 );
    vertices[0] = TwoDim<>( 5.9, 1.0 );
    vertices[1] = TwoDim<>( 5.9, 3.0 );
    vertices[2] = TwoDim<>( 8.0, 3.0 );
    vertices[3] = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> adjacent1( vertices );

    vertices[0] = TwoDim<>( 6.1, 1.0 );
    vertices[1] = TwoDim<>( 6.1, 3.0 );
    vertices[2] = TwoDim<>( 8.0, 3.0 );
    vertices[3] = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> adjacent2( vertices );

    vertices[0] = TwoDim<>( 6.2, 1.0 );
    vertices[1] = TwoDim<>( 6.2, 3.0 );
    vertices[2] = TwoDim<>( 8.0, 3.0 );
    vertices[3] = TwoDim<>( 8.0, 1.0 );
    Geometry2D<> nonAdjacent( vertices );

    vertices.resize( 5 );
    vertices[0] = TwoDim<>( 0.0, 0.0 );
    vertices[1] = TwoDim<>( 4.0, 0.0 );
    vertices[2] = TwoDim<>( 3.0, 3.0 );
    vertices[3] = TwoDim<>( 2.0, 0.1 );
    vertices[4] = TwoDim<>( 1.0, 3.0 );
    Geometry2D<> notNeglactable( vertices );

    TS_ASSERT( big.IsAdjacentTo( adjacent1, tolerance ) );
    TS_ASSERT( big.IsAdjacentTo( adjacent2, tolerance ) );
    TS_ASSERT_EQUALS( big.IsAdjacentTo( nonAdjacent, tolerance ), false );

    big.Intersection( adjacent1, intersection );
    TS_ASSERT_EQUALS( intersection.size(), 1 );
    TS_ASSERT( intersection[0].IsNeglectable( tolerance ) );

    big.Intersection( adjacent1, intersection, tolerance );
    TS_ASSERT( intersection.empty() );

    TS_ASSERT_EQUALS( notNeglactable.IsNeglectable( tolerance ), false );


    vertices.resize( 4 );
    vertices[0] = TwoDim<>( 1.0, 1.0 );
    vertices[1] = TwoDim<>( -1.0, 3.0 );
    vertices[2] = TwoDim<>( 1.0, 5.0 );
    vertices[3] = TwoDim<>( 3.0, 3.0 );
    Geometry2D<> crookedRhombus( vertices );

    TwoDim<> centroid( 1.0, 3.0 );
    TwoDim<> lowerEdge, higherEdge;
    crookedRhombus.Envelope( lowerEdge, higherEdge );
    TS_ASSERT_DELTA( centroid.DistanceTo( crookedRhombus.Centroid() ), 0.0, sDelta );
    TS_ASSERT_DELTA( lowerEdge.DistanceTo( TwoDim<>( -1.0, 1.0 ) ), 0.0, sDelta );
    TS_ASSERT_DELTA( higherEdge.DistanceTo( TwoDim<>( 3.0, 5.0 ) ), 0.0, sDelta );


    vertices.pop_back();
    vertices.pop_back();
    Geometry2D<> line( vertices );
    TS_ASSERT_EQUALS( line.CalculateAreaValue(), 0.0 );

    line.GetPoints( vertices );
    TS_ASSERT_EQUALS( vertices.size(), 2 );
    TS_ASSERT_DELTA( vertices[0].Get1(), 1.0, sDelta );
    TS_ASSERT_DELTA( vertices[0].Get2(), 1.0, sDelta );
    TS_ASSERT_DELTA( vertices[1].Get1(), -1.0, sDelta );
    TS_ASSERT_DELTA( vertices[1].Get2(), 3.0, sDelta );


    {
        vector< TwoDim<> > base( 4 );
        base[0] = TwoDim<>( -1.0, 0.0 );
        base[1] = TwoDim<>( -1.0, 2.0 );
        base[2] = TwoDim<>( 1.0, 2.0 );
        base[3] = TwoDim<>( 1.0, 0.0 );
        Geometry2D<> square( base );

        vector< TwoDim<> > vertices( 7 );
        vertices[0] = TwoDim<>( -2.0, 1.0 );
        vertices[1] = TwoDim<>( -1.0, 2.0 );
        vertices[2] = TwoDim<>( 0.0, 3.0 );
        vertices[3] = TwoDim<>( 1.0, 2.0 );
        vertices[4] = TwoDim<>( 2.0, 1.0 );
        vertices[5] = TwoDim<>( 2.0, 5.0 );
        vertices[6] = TwoDim<>( -2.0, 5.0 );
        Geometry2D<> compareWithSquare1( vertices );

        vertices.clear();
        vertices.push_back( TwoDim<>( -3.0, 2.0 ) );
        vertices.push_back( TwoDim<>( -1.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 1.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 3.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 0.0, 3.0 ) );
        Geometry2D<> compareWithSquare2( vertices );

        vertices.clear();
        vertices.push_back( TwoDim<>( -2.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 0.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 2.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 0.0, 3.0 ) );
        Geometry2D<> compareWithSquare3( vertices );

        vertices.clear();
        vertices.push_back( TwoDim<>( 0.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 2.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 1.0, 4.0 ) );
        Geometry2D<> compareWithSquare4( vertices );

        vertices.clear();
        vertices.push_back( TwoDim<>( 1.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 3.0, 2.0 ) );
        vertices.push_back( TwoDim<>( 2.0, 4.0 ) );
        Geometry2D<> compareWithSquare5( vertices );

        TS_ASSERT( square.IsAdjacentTo( compareWithSquare1, tolerance ) == false );
        TS_ASSERT( square.IsAdjacentTo( compareWithSquare2, tolerance ) );
        TS_ASSERT( square.IsAdjacentTo( compareWithSquare3, tolerance ) );
        TS_ASSERT( square.IsAdjacentTo( compareWithSquare4, tolerance ) );
        TS_ASSERT( square.IsAdjacentTo( compareWithSquare5, tolerance ) == false );
    }


    vertices.resize( 3 );
    map< size_t, TwoDim<> > arcs;
    arcs[1] = TwoDim<>( 0.0, 0.0 );


    vertices[0] = TwoDim<>( 0.0, 0.0 );
    vertices[1] = TwoDim<>( 0.0, 2.05 );
    vertices[2] = TwoDim<>( 2.05, 0.0 );
    Geometry2D<> innerArea( vertices, arcs, 0.1 );


    vertices.resize( 4 );
    arcs[3] = TwoDim<>( 0.0, 0.0 );
    vertices[0] = TwoDim<>( 2.0 / sqrt( 2.0 ), 2.0 / sqrt( 2.0 ) );
    vertices[1] = TwoDim<>( 4.0 / sqrt( 2.0 ), 4.0 / sqrt( 2.0 ) );
    vertices[2] = TwoDim<>( 4.0 / sqrt( 2.0 ), -4.0 / sqrt( 2.0 ) );
    vertices[3] = TwoDim<>( 2.0 / sqrt( 2.0 ), -2.0 / sqrt( 2.0 ) );
    Geometry2D<> outerArea( vertices, arcs, 0.136 );


    vertices[0] = TwoDim<>( 0.0, 1.9 );
    vertices[1] = TwoDim<>( 0.0, 2.1 );
    vertices[2] = TwoDim<>( 2.1, 0.0 );
    vertices[3] = TwoDim<>( 1.9, 0.0 );
    Geometry2D<> middleArea( vertices, arcs, 0.091 );


    TS_ASSERT_DELTA( innerArea.CalculateAreaValue(), 2.05 * 2.05 * Angle<>::pi / 4.0, sDelta );
    TS_ASSERT_DELTA( middleArea.CalculateAreaValue(), ( 2.1 * 2.1 - 1.9 * 1.9 ) * Angle<>::pi / 4.0, sDelta );
    TS_ASSERT_DELTA( outerArea.CalculateAreaValue(), ( 4.0 * 4.0 - 2.0 * 2.0 ) * Angle<>::pi / 4.0, sDelta );


    {
        /*Tolerance<> tolerance(0.0, Angle<>::Rad(0), 0.1);
        vector<Geometry2D<> > intersection;
        innerArea.Intersection(outerArea, intersection, tolerance);
        BOOST_FOREACH(const Geometry2D<> &geometry2D, intersection)
            TS_ASSERT(geometry2D.IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance));



        middleArea.Intersection(innerArea, intersection, tolerance);
        TS_ASSERT_EQUALS(intersection.size(), 1);
        TS_ASSERT_EQUALS(intersection[0].IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance), false);



        middleArea.Intersection(outerArea, intersection, tolerance);
        TS_ASSERT_EQUALS(intersection.size(), 1);
        TS_ASSERT_EQUALS(intersection[0].IsArcIntersectionRest(TwoDim<>(0.0, 0.0), tolerance), false);*/
    }


    return 0;
}
#endif
