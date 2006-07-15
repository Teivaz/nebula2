#ifndef N_MAXMATERIAL_UI_H
#define N_MAXMATERIAL_UI_H

#include "tinyxml/tinyxml.h"

nString AddSpinner(TiXmlElement* elemParam);
nString AddCheckBox(TiXmlElement* elemParam);
nString AddDropdownList(TiXmlElement* elemParam);
nString AddColorPicker(TiXmlElement* elemParam);
nString AddMapButton(TiXmlElement* elemParam);
nString AddVector4Spinner(TiXmlElement* elemParam);
nString AddLabel(const nString &uiname, const nString &caption, int across=1, bool addDot = false);

nString GetDefault(TiXmlElement* elemParam);

#endif
