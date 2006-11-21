//-----------------------------------------------------------------------------
//  nmaxmaterial_ui.h
//
//  (c)2004 Kim, Hyoun Woo
//-----------------------------------------------------------------------------
#ifndef N_MAXMATERIAL_UI_H
#define N_MAXMATERIAL_UI_H

#include "tinyxml/tinyxml.h"

nString AddSpinner(TiXmlElement* elemParam);
nString AddCheckBox(TiXmlElement* elemParam);
nString AddDropdownList(TiXmlElement* elemParam);
nString AddColorPicker(TiXmlElement* elemParam);
nString AddMapButton(TiXmlElement* elemParam, nString defaultValue = "");
nString AddVector4Spinner(TiXmlElement* elemParam);
nString AddLabel(const nString &uiname, const nString &caption, int across=1, bool addDot = false);
nString AddSetDirDlg(TiXmlElement* elemParam);
nString AddEnvelopeCurve(const nString &shdName, TiXmlElement* elemParam);


nString GetDefault(TiXmlElement* elemParam);

#endif
