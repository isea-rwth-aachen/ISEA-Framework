#ifndef _BESSEL_
#define _BESSEL_


namespace geometry
{
///Bessel function of first kind with index 0 with valid domain in [0, 2.4048]
double Bessel0(double x);

///Bessel function of first kind with index 2 with valid domain in [0, 5.1356]
double Bessel2(double x);

extern const double bessel0ZP;///<First zero point of bessel function of first kind with index 0
extern const double bessel2ZP;///<First zero point of bessel function of first kind with index 2
extern const double bessel2Max;///<First maximum point of bessel function of first kind with index 2

}
#endif
