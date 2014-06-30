//Jens Troest (c) 2007

//Definition of the WMR class
//This is a little "abuse" of C++ really, just using it for the ease of the string methods in the std namespace pf C++
//If you are pedantic feel free to rewrite it to pure C or vice versa :)
//Additonal note 2014: I did this bit back in 2007 when I first got the weather station and I am sure there are amny decent C string libraries out there now. I would probably rather rewrite the whole thing to JAVA or another OO language but this serves the purpose with a small footprint, minimal CPU and no leaks (That I can see after running for years)

#ifndef __wmr_h__
#define __wmr_h__
#include <time.h>

#include <string>
using namespace std;


enum FrameEnum { 
  FT_WIND=0, 
  FT_RAIN=1,
  FT_TH=2,
  FT_MUSHR=3,
  FT_TEMP=4,
  FT_BTH=5,
  FT_EXTBTH=6,
  FT_MINUTE=14,
  FT_CLOCK=15,
  FT_ERROR=-1
};
// weather status information
enum StatusEnum{ 
  WST_SUNNY=0xC, 
  WST_HLFCLDY=0x6, 
  WST_CLDY=0x2, 
  WST_RAINY=0x3 
};
/* byte converting macros for improved readability and efficiency */
#define LO(byte)        (byte & 0x0f)
#define HI(byte)        ((byte & 0xf0) >> 4)
#define NUM(byte)       (10 * HI(byte) + LO(byte))
#define BIT(byte, bit)  ((byte & (1 << bit)) >> bit)
//#define ERROR(mem, byte, value)  if (!((mem.err) = (byte == 0xee))) \
//            (mem.val) = (value);

#define FT_MAX            16    /* maximal number of wmr918 frame types */
#define MAX_FRAMELEN      14
#define MAX_CLOCK_CHANGE  1800  // maximally allowed change in deviation of 
                                // system and wmr918 clock in seconds
                  
#define FORMAT_RAW        1
#define FORMAT_CLEAR      2
#define FORMAT_DECODED    3

#define TH_AMOUNT		  4		//Amount of thermohygro sensors. 1 Main unit and 3 on the channels
#define TEMP_AMOUNT       3//Number of channel for Temp sensors

class WMR_SENS {
public:
  int val;
  float fval;
  int over;
};

class WMR_UNIT {
public:
  time_t sys_tm;
  time_t clock_tm;
  int lobat;
  int chan;
  int framelen;
  int clock_valid;   // indicates that a first valid WMR918 full clock frame
  unsigned char frame[MAX_FRAMELEN];
  bool detected;//Mean the unit has been detected by the station
};

class WMR_CLOCK {
public:
  WMR_UNIT unit;
  // contains system time at the moment of receiving wmr918 clock frame
  // and converted time of clock frame
  // (and raw frame data)
  long clock_diff;
  struct tm date_tm;    // clock date/time structure (broken down time)
};

typedef struct WMR_WIND {
  WMR_UNIT unit;
  WMR_SENS dir;
  WMR_SENS gust;
  WMR_SENS avrg;
  WMR_SENS chill;
  int chill_nodata;
} WMR_WIND;

class WMR_RAIN {
public:
  WMR_UNIT unit;
  WMR_SENS curr;
  WMR_SENS total;
  WMR_SENS yesterday;
  struct tm startdate;
};

class WMR_TH {
public:
  WMR_UNIT unit;
  int chan_num;
  WMR_SENS temp;
  WMR_SENS hum;
  WMR_SENS dew;
};

class WMR_T {
public:
  WMR_UNIT unit;
  int chan_num;
  WMR_SENS temp;
};

class WMR_BTH {
public:
  WMR_UNIT unit;
  WMR_SENS temp;
  WMR_SENS hum;
  WMR_SENS dew;
  WMR_SENS press;
  WMR_SENS sealevel;
  int w_status;
  int version;
};


class WMR    /* structure for storing a whole set of wmr918 data */
{
public:
	time_t sys_tm;
	int use_sys_tm;    // indicates an inconsistency in WMR918 clock
	WMR_CLOCK clock;
	WMR_WIND wind;
	WMR_RAIN rain;
	WMR_TH th[TH_AMOUNT];		// 0 is the main sensor. The others are on the respective channels
	WMR_T temp[3];
	WMR_BTH bth;
	string printXML();
};

typedef WMR_UNIT * unitptr_t;
extern const int datalen[FT_MAX];

#endif // #ifdef __wmr_h__

