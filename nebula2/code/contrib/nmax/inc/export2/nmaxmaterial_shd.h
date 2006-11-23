//  nmaxmaterial_shd.h.cc
//
//  (C)2005 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXMATERIALSHD_H
#define N_MAXMATERIALSHD_H

//-----------------------------------------------------------------------------
/**
*/
bool EvalCustomMaterialPlugin();

//-----------------------------------------------------------------------------
/**
    Retrieve default texture filename from shader.xml.

    @param shader shader alias which is alread assigned to the given node. i.g. "static", "skinned" etc.
    @param param string of shader parameter such as 'nShaderState::DiffMap0' and so on. 
    @param outvalue texture filename retrieved from database which will be used for default texture.

    @return true, if the default value is found ohterwise false.
*/
bool GetDefaultValueFromDataBase(const nString &shader, const nString &param, nString &outvalue);

#endif
