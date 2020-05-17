#pragma once

/// 三次方程式を解く
/// \param c3 三次項係数
/// \param c2 二次項係数
/// \param c1 一次項係数
/// \param c0 定数項係数
/// \param xR [out] 実数解[3]
/// \param xI [out] 虚数解[3]
void cubicEquation(double c3, double c2, double c1, double c0, double *xR, double *xI);