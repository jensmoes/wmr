#ifndef _xml_encode_h_
#define _xml_encode_h_
//Prototypes
// data frame types
#include "wmr.h"
#define MAX_TEMP_STRING_LENGTH 200
#include <string>
using namespace std;
string wmr2xml(WMR* wmr);
string clock2xml(WMR_CLOCK clock);
string bat2xml(int bat);
string time2xml(time_t time);
string systime2xml(time_t time);
string hum2xml(WMR_SENS* hum);
string wind2xml(WMR_WIND* wind);
string temp2xml(WMR_SENS* temp, const char* name);
string rain2xml(WMR_RAIN *rain);
string bth2xml(WMR_BTH *bth);
string th2xml(WMR_TH* th, int number);
string t2xml(WMR_T* t, int number);

#endif
