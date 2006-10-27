//  nmaxmaterial_shd.h.cc
//
//  (C)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXMATERIALSHD_H
#define N_MAXMATERIALSHD_H

class nString;

//-----------------------------------------------------------------------------
/**
*/
bool EvalCustomMaterialPlugin();

//-----------------------------------------------------------------------------
/**
*/
bool GetDefaultValueFromDataBase(const nString &shader, const nString &param, nString &outvalue);

#endif
