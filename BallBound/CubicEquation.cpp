// CubicEquation.cpp : 三次方程式の解を求める
//
#include <cmath>
#include "CubicEquation.h"

inline double pow2(double x) { return x * x;  }
inline double pow3(double x) { return x * x * x; }
inline double cbrt(double x) { return pow(x, 1.0 / 3.0); }
const double sqrt3 = sqrt(3.0);

//-----------------------------------------------------------------------------
// 複素数
struct complex {
	double R;
	double I;

	double getNorm(void) const
	{
		return sqrt(pow2(R) + pow2(I));
	};
	double getDot(void) const
	{
		return pow2(R) + pow2(I);
	};

	double getAngle(void) const
	{
		return atan2(I, R);
	};
};

//-----------------------------------------------------------------------------
// 三次方程式を解く
// \param c3 三次項係数
// \param c2 二次項係数
// \param c1 一次項係数
// \param c0 定数項係数
// \param xR [out] 実数解[3]
// \param xI [out] 虚数解[3]
void cubicEquation(double c3, double c2, double c1, double c0, double *xR, double *xI)
{
	double t = -c2 / (3 * c3);

	// a, b
	double a = (c1 / c3) - (pow2(c2) / (3 * pow2(c3)));
	double b = (c0 / c3) - ((c1 * c2) / (3 * pow2(c3))) - ((2 * pow3(c2)) / (27 * pow3(c3)));
	double R = pow2(b / 2) + pow3(a / 3);

	// u, v
	complex u;
	complex v;
	if (0 <= R)
	{
		u.R = -(b / 2) + sqrt(R);
		u.I = 0;

		v.R = -(b / 2) - sqrt(R);
		v.I = 0;
	}
	else
	{
		u.R = (-b / 2);
		u.I = sqrt(-R);

		v.R = -(b / 2);
		v.I = -sqrt(-R);
	}
	double uN = u.getNorm();
	double uA = u.getAngle();
	double vN = v.getNorm();
	double vA = v.getAngle();

	// p, q
	complex p;
	if (0 < uN)
	{
		p.R = cbrt(uN) * cos(uA / 3);
		p.I = cbrt(uN) * sin(uA / 3);
	}
	else
	{
		p.R = 0;
		p.I = 0;
	}
	double pN = p.getDot();
	double pA = p.getAngle();

	complex q;
	if (0 < pN)
	{
		q.R = -(a * p.R) / (3 * pN);
		q.I = (a * p.I) / (3 * pN);
	}
	else
	{
		q.R = cbrt(vN) * cos(vA / 3);
		q.I = cbrt(vN) * sin(vA / 3);
	}
	double qN = q.getDot();
	double qA = q.getAngle();

	// x1, x2, x3
//	complex x1, x2, x3;
	double XR = p.R + q.R;
	double  S = sqrt3 * (-p.I + q.I);
	xR[0] = t + XR;
	xR[1] = t + (-XR + S) / 2;
	xR[2] = t + (-XR - S) / 2;

	double XI = (p.I + q.I);
	double  W = (p.R - q.R);
	xI[0] = XI;
	xI[1] = (sqrt3 * W - XI) / 2;
	xI[2] = (-sqrt3 * W - XI) / 2;

/*
	// wp, w2q
	complex wp, w2q;
	wp.R  = (-p.R - p.I * sqrt3) / 2;
	wp.I  = ( p.R * sqrt3 - p.I) / 2;
	w2q.R = (-q.R + q.I * sqrt3) / 2;
	w2q.I = (-q.R * sqrt3 - q.I) / 2;

	// w2p, wq
	complex w2p, wq;
	w2p.R = (-p.R + p.I * sqrt3) / 2;
	w2p.I = (-p.R * sqrt3 - p.I) / 2;
	wq.R  = (-q.R - q.I * sqrt3) / 2;
	wq.I  = (q.R * sqrt3 - q.I) / 2;

	// x1, x2, x3
//	complex x1, x2, x3;
	xR[0] = t + p.R + q.R;
	xI[0] =     p.I + q.I;
	xR[1] = t + wp.R + w2q.R;
	xI[1] =     wp.I + w2q.I;
	xR[2] = t + w2p.R + wq.R;
	xI[2] =     w2p.I + wq.I;
*/
}

