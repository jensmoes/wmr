//Jens Troest 2014

//Implementation file for WMR class

#include "wmr.h"
#include "xml_encode.h"
//Method for printing out desired formats. Feel free to add others. JSON springs in to mind :)
string WMR::printXML()
{
  string out = wmr2xml(this);

  return out;
}
