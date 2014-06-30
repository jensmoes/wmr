//Jens Troest (C) 2007

//Decoder of frames from the WMR918 protocol.
//This library will take complete frames detected from the relevant data source (RS232 for the WM9xx's) and add them to a WMR object

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "decode.h"
extern const int datalen[FT_MAX] =
{
  8,
  13,
  6,
  6,
  4,
  10,
  11,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  2,
  6
}; 

	
int wmr_decode(WMR *wmr, int *channel, unitptr_t *unit, unsigned char* framebuf)
{
  int frametype = framebuf[0];
  int clock_diff;   // temporarily holds deviation of system and WMR918 clocks.
  int chan = -1;
  wmr->sys_tm = time(NULL);
  switch (frametype) {
    case FT_WIND:
      *unit = &(wmr->wind.unit);
      wmr->wind.unit.sys_tm = wmr->sys_tm;
	  wmr->wind.unit.clock_valid = wmr->clock.unit.clock_valid;
   	  if(wmr->clock.unit.clock_valid)
	  {
		  // add time passed since last clock frame
		  wmr->wind.unit.clock_tm = wmr->clock.unit.clock_tm
			  + (long)difftime(wmr->wind.unit.sys_tm, wmr->clock.unit.sys_tm);
	  }
      wmr->wind.unit.lobat = BIT(framebuf[1], 6);
      wmr->wind.dir.val = (LO(framebuf[3]) * 100)
          + (HI(framebuf[2]) * 10) 
          + LO(framebuf[2]);
      
      wmr->wind.gust.val = HI(framebuf[3])
          + (LO(framebuf[4]) * 10)	
          + (HI(framebuf[4]) * 100); 
      wmr->wind.gust.fval = (float)wmr->wind.gust.val / 10;
      wmr->wind.gust.over = BIT(framebuf[1], 4);

      wmr->wind.avrg.val = LO(framebuf[5]) 
          + (HI(framebuf[5]) * 10) 
          + (LO(framebuf[6]) * 100);
      wmr->wind.avrg.fval = (float)wmr->wind.avrg.val / 10;
      wmr->wind.avrg.over = BIT(framebuf[1], 5);

      wmr->wind.chill.val = (HI(framebuf[7]) * 10) 
          + LO(framebuf[7]);    
	  wmr->wind.chill.fval = (float)wmr->wind.chill.val;
      if (BIT(framebuf[6], 7)) wmr->wind.chill.val *= -1;
      wmr->wind.chill.over = BIT(framebuf[6], 6);
      wmr->wind.chill_nodata = BIT(framebuf[6], 5);
      
      wmr->wind.unit.framelen = datalen[frametype];
      memcpy(&(wmr->wind.unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_RAIN:
      *unit = &(wmr->rain.unit);
      wmr->rain.unit.sys_tm = wmr->sys_tm;
	  wmr->rain.unit.clock_valid = wmr->clock.unit.clock_valid;
      // add time passed since last clock frame
	  if(wmr->clock.unit.clock_valid)
	  {
		  wmr->rain.unit.clock_tm = wmr->clock.unit.clock_tm
			  + (long)difftime(wmr->rain.unit.sys_tm, wmr->clock.unit.sys_tm);
	  }      
      wmr->rain.unit.lobat = BIT(framebuf[1], 6);
      wmr->rain.curr.val = (HI(framebuf[2]) * 10) 
          + LO(framebuf[2])
          + (LO(framebuf[3]) * 100);
      wmr->rain.curr.over = BIT(framebuf[1], 4);

      wmr->rain.total.val = HI(framebuf[3]) 
          + (LO(framebuf[4]) * 10)
          + (HI(framebuf[4]) * 100)
          + (LO(framebuf[5]) * 1000)
          + (HI(framebuf[5]) * 10000) - 5;
      wmr->rain.total.fval = (float)wmr->rain.total.val / 10;
      wmr->rain.total.over = BIT(framebuf[1], 5);
      
      wmr->rain.yesterday.val = LO(framebuf[6])
          + (HI(framebuf[6]) * 10)
          + (LO(framebuf[7]) * 100)
          + (HI(framebuf[7]) * 1000);
      wmr->rain.yesterday.over = BIT(framebuf[1], 7);

      wmr->rain.startdate.tm_year = (HI(framebuf[12]) * 10)
          + LO(framebuf[12]) + 100;
      wmr->rain.startdate.tm_mon = (HI(framebuf[11]) * 10) 
          + LO(framebuf[11]) - 1;
      wmr->rain.startdate.tm_mday = (HI(framebuf[10]) * 10) 
          + LO(framebuf[10]);
      wmr->rain.startdate.tm_hour = (HI(framebuf[9]) * 10) 
          + LO(framebuf[9]);
      wmr->rain.startdate.tm_min = (HI(framebuf[8]) * 10) 
          + LO(framebuf[8]);
      
      wmr->rain.unit.framelen = datalen[frametype];
      memcpy(&(wmr->rain.unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_TH:
      chan = LO(framebuf[1]);
      switch (chan) {
        case 1: chan = 1; break;
        case 2: chan = 2; break;
        case 4: chan = 3; break;
        default: chan = 0xf; break;
      }
      if (chan <= 3) {
        *unit = &(wmr->th[chan].unit);
        wmr->th[chan].unit.sys_tm = wmr->sys_tm;
		wmr->th[chan].unit.clock_valid = wmr->clock.unit.clock_valid;
        // add time passed since last clock frame
		if(wmr->clock.unit.clock_valid)
		{
			wmr->th[chan].unit.clock_tm = wmr->clock.unit.clock_tm
				+ (long)difftime(wmr->th[chan].unit.sys_tm, wmr->clock.unit.sys_tm);
		}
        wmr->th[chan].unit.lobat = BIT(framebuf[1], 6);
        wmr->th[chan].chan_num = LO(framebuf[1]);

        wmr->th[chan].temp.val = LO(framebuf[2])
            + (HI(framebuf[2]) * 10)
            + (LO(framebuf[3]) * 100)
            + (((framebuf[3] & 0x30) >> 4) * 1000);
        if (BIT(framebuf[3], 7)) wmr->th[chan].temp.val *= -1;
        wmr->th[chan].temp.fval = (float)wmr->th[chan].temp.val / 10;

        wmr->th[chan].temp.over = BIT(framebuf[3], 6);

        wmr->th[chan].hum.val = LO(framebuf[4])
            + (HI(framebuf[4]) * 10);

        wmr->th[chan].dew.val = LO(framebuf[5])
            + (HI(framebuf[5]) * 10);
        wmr->th[chan].dew.over = BIT(framebuf[1], 4);
		wmr->th[chan].dew.fval = (float)wmr->th[chan].dew.val;

        wmr->th[chan].unit.framelen = datalen[frametype];
        memcpy(&(wmr->th[chan].unit.frame), framebuf, datalen[frametype]);
        *channel = chan;
      } else {
        return -1;
      }
      break;
    case FT_MUSHR:
	  *unit = &(wmr->th[0].unit);
	  *channel = 0;
      wmr->th[0].unit.sys_tm = wmr->sys_tm;
	  wmr->th[0].unit.clock_valid = wmr->clock.unit.clock_valid;
	  if(wmr->clock.unit.clock_valid)
	  {
		  // add time passed since last clock frame
		  wmr->th[0].unit.clock_tm = wmr->clock.unit.clock_tm
			  + (long)difftime(wmr->th[0].unit.sys_tm, wmr->clock.unit.sys_tm);
	  }      
      wmr->th[0].unit.lobat = BIT(framebuf[1], 6);
      wmr->th[0].temp.val = LO(framebuf[2])
          + (HI(framebuf[2]) * 10)
          + (LO(framebuf[3]) * 100)
          + (((framebuf[3] & 0x30) >> 4) * 1000);
      if (BIT(framebuf[3], 7)) wmr->th[0].temp.val *= -1;
      wmr->th[0].temp.fval = (float)wmr->th[0].temp.val / 10;
      
      wmr->th[0].temp.over = BIT(framebuf[3], 6);
      
      wmr->th[0].hum.val = LO(framebuf[4])
          + (HI(framebuf[4]) * 10);
      
      wmr->th[0].dew.val = LO(framebuf[5])
          + (HI(framebuf[5]) * 10);
      wmr->th[0].dew.over = BIT(framebuf[1], 4);
	  wmr->th[0].dew.fval = (float)wmr->th[0].dew.val;

      wmr->th[0].unit.framelen = datalen[frametype];
      memcpy(&(wmr->th[0].unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_TEMP:
      chan = LO(framebuf[1]);
      switch (chan) {
        case 1: chan = 0; break;
        case 2: chan = 1; break;
        case 4: chan = 2; break;
        default: chan = 0xf; break;
      }
      if (chan <= 2) {
        *unit = &(wmr->th[chan].unit);
        wmr->temp[chan].unit.sys_tm = wmr->sys_tm;
		wmr->temp[chan].unit.clock_valid = wmr->clock.unit.clock_valid;
		if(wmr->clock.unit.clock_valid)
		{
			// add time passed since last clock frame
			wmr->temp[chan].unit.clock_tm = wmr->clock.unit.clock_tm
				+ (long)difftime(wmr->temp[chan].unit.sys_tm, wmr->clock.unit.sys_tm);
		}
        wmr->temp[chan].unit.lobat = BIT(framebuf[1], 6);
        wmr->temp[chan].chan_num = LO(framebuf[1]);

        wmr->temp[chan].temp.val = LO(framebuf[2])
            + (HI(framebuf[2]) * 10)
            + (LO(framebuf[3]) * 100)
            + (((framebuf[3] & 0x30) >> 4) * 1000);
        if (BIT(framebuf[3], 7)) wmr->temp[chan].temp.val *= -1;
        wmr->temp[chan].temp.fval = (float)wmr->temp[chan].temp.val / 10;
        wmr->temp[chan].temp.over = BIT(framebuf[3], 6);

        wmr->temp[chan].unit.framelen = datalen[frametype];
        memcpy(&(wmr->temp[chan].unit.frame), framebuf, datalen[frametype]);
        *channel = chan;
      } else {
        return -1;
      }
      break;
    case FT_BTH:
      *unit = &(wmr->bth.unit);
	  wmr->bth.version = 1;
      wmr->bth.unit.sys_tm = wmr->sys_tm;
	  wmr->bth.unit.clock_valid = wmr->clock.unit.clock_valid;
	  if(wmr->clock.unit.clock_valid)
	  {
		  // add time passed since last clock frame
		  wmr->bth.unit.clock_tm = wmr->clock.unit.clock_tm
			  + (long)difftime(wmr->bth.unit.sys_tm, wmr->clock.unit.sys_tm);
	  }
      wmr->bth.unit.lobat = BIT(framebuf[1], 6);
      wmr->bth.temp.val = LO(framebuf[2])
          + (HI(framebuf[2]) * 10)
          + (LO(framebuf[3]) * 100)
          + (((framebuf[3] & 0x30) >> 4) * 1000);
      if (BIT(framebuf[3], 7)) wmr->bth.temp.val *= -1;
      wmr->bth.temp.fval = (float)wmr->bth.temp.val / 10;

      wmr->bth.temp.over = BIT(framebuf[3], 6);
      
      wmr->bth.hum.val = LO(framebuf[4])
          + (HI(framebuf[4]) * 10);
      
      wmr->bth.dew.val = LO(framebuf[5])
          + (HI(framebuf[5]) * 10);
      wmr->bth.dew.over = BIT(framebuf[1], 4);
	  wmr->bth.dew.fval = (float)wmr->bth.dew.val;
      wmr->bth.press.val = (framebuf[6] + 795);
      wmr->bth.w_status = LO(framebuf[7]);
      wmr->bth.sealevel.val = LO(framebuf[8])
          + (HI(framebuf[8]) * 10)
          + (LO(framebuf[9]) * 100)
          + (HI(framebuf[9]) * 1000);
      if (wmr->bth.sealevel.val < 4000) wmr->bth.sealevel.val += 10000;

	  wmr->bth.sealevel.fval = (float)wmr->bth.sealevel.val / 10;

      wmr->bth.unit.framelen = datalen[frametype];
      memcpy(&(wmr->bth.unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_EXTBTH:
      *unit = &(wmr->bth.unit);
	  wmr->bth.version = 2;
      wmr->bth.unit.sys_tm = wmr->sys_tm;
	  wmr->bth.unit.clock_valid = wmr->clock.unit.clock_valid;
	  // add time passed since last clock frame
       	  if(wmr->clock.unit.clock_valid)
      	  {
		  wmr->bth.unit.clock_tm = wmr->clock.unit.clock_tm
			  + (long)difftime(wmr->bth.unit.sys_tm, wmr->clock.unit.sys_tm);
       	  }
      wmr->bth.unit.lobat = BIT(framebuf[1], 6);
      wmr->bth.temp.val = LO(framebuf[2])
          + (HI(framebuf[2]) * 10)
          + (LO(framebuf[3]) * 100)
          + (((framebuf[3] & 0x30) >> 4) * 1000);
      if (BIT(framebuf[3], 7)) wmr->bth.temp.val *= -1;
      wmr->bth.temp.fval = (float)wmr->bth.temp.val / 10;

      wmr->bth.temp.over = BIT(framebuf[3], 6);
      
      wmr->bth.hum.val = LO(framebuf[4])
          + (HI(framebuf[4]) * 10);
      
      wmr->bth.dew.val = LO(framebuf[5])
          + (HI(framebuf[5]) * 10);
      wmr->bth.dew.over = BIT(framebuf[1], 4);
	  wmr->bth.dew.fval = (float)wmr->bth.dew.val;

      wmr->bth.press.val = (framebuf[6] + ((framebuf[7] & 0x1) * 256) + 600);
      
      wmr->bth.w_status = HI(framebuf[7]);
      
      wmr->bth.sealevel.val = HI(framebuf[8])
          + (LO(framebuf[9]) * 10)
          + (HI(framebuf[9]) * 100)
          + (LO(framebuf[10]) * 1000)
          + (HI(framebuf[10]) * 10000)
		  + (framebuf[6] + (framebuf[7] & 0x1) * 256)*10;
	  wmr->bth.sealevel.fval = (float)wmr->bth.sealevel.val / 10 ;

      wmr->bth.unit.framelen = datalen[frametype];
      memcpy(&(wmr->bth.unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_MINUTE:
      *unit = &(wmr->clock.unit);
      wmr->clock.unit.sys_tm = wmr->sys_tm;
      wmr->clock.unit.lobat = BIT(framebuf[1], 7);
      // if no complete clock data had been received up to now,
      // use system time.
      if (!(wmr->clock.unit.clock_valid)) {
        wmr->clock.date_tm = *localtime(&(wmr->clock.unit.sys_tm));
      } else {
        wmr->clock.date_tm.tm_min = 
          (((framebuf[1] & 0x70) >> 4) * 10) + LO(framebuf[1]);
        wmr->clock.unit.clock_tm = mktime(&(wmr->clock.date_tm));
        
        clock_diff = (long)difftime(wmr->clock.unit.clock_tm, wmr->clock.unit.sys_tm);
        if ((labs(clock_diff - wmr->clock.clock_diff) > MAX_CLOCK_CHANGE)) {
          // if difference of system and WMR918 clock times had changed
          // too much since last clock frame, regard WMR918 clock as bad.
          if (!(wmr->use_sys_tm)) {
            wmr->use_sys_tm = 1;
          }
        } else {
          if (wmr->use_sys_tm) {
//            syslog(LOG_USER + LOG_PID, "WMR918 clock seems to be ok, again");
            wmr->use_sys_tm = 0;
          }
          wmr->clock.clock_diff = clock_diff;
        }
      }
      
      wmr->clock.unit.framelen = datalen[frametype];
      memcpy(&(wmr->clock.unit.frame), framebuf, datalen[frametype]);
      break;
    case FT_CLOCK:
      *unit = &(wmr->clock.unit);
      wmr->clock.unit.sys_tm = wmr->sys_tm;
      wmr->clock.unit.lobat = BIT(framebuf[1], 7);

      wmr->clock.date_tm.tm_year = (HI(framebuf[5]) * 10) + LO(framebuf[5]) + 100;
      wmr->clock.date_tm.tm_mon = (HI(framebuf[4]) * 10) + LO(framebuf[4]) - 1;
      wmr->clock.date_tm.tm_mday = (HI(framebuf[3]) * 10) + LO(framebuf[3]);
      wmr->clock.date_tm.tm_hour = (HI(framebuf[2]) * 10) + LO(framebuf[2]);
      wmr->clock.date_tm.tm_min = ((framebuf[1] & 0x70) >> 4) * 10
           + LO(framebuf[1]);
      wmr->clock.date_tm.tm_sec = 0;
      wmr->clock.unit.clock_tm = mktime(&(wmr->clock.date_tm));
      if (wmr->clock.unit.clock_tm == (time_t)(-1)) {
        // if received WMR918 clock time isn't valid date/time
        wmr->clock.date_tm = *localtime(&(wmr->clock.unit.sys_tm));
        wmr->clock.unit.clock_tm = wmr->clock.unit.sys_tm;
        wmr->use_sys_tm = 1;
      } else {
        wmr->clock.unit.clock_valid = 1;
        clock_diff = (long)difftime(wmr->clock.unit.clock_tm, wmr->clock.unit.sys_tm);
        if ((labs(clock_diff - wmr->clock.clock_diff) > MAX_CLOCK_CHANGE)) {
          // if difference of system and WMR918 clock times had changed
          // too much since last clock frame, regard WMR918 clock as bad.
          if (!(wmr->use_sys_tm)) {
            //syslog(LOG_USER + LOG_PID, "bad WMR918 clock");
            wmr->use_sys_tm = 1;
          }
        } else {
          if (wmr->use_sys_tm) {
            //syslog(LOG_USER + LOG_PID, "WMR918 clock seems to be ok, again");
            wmr->use_sys_tm = 0;
          }
          wmr->clock.clock_diff = clock_diff;
        }
      }
      
      wmr->clock.unit.framelen = datalen[frametype];
      memcpy(&(wmr->clock.unit.frame), framebuf, datalen[frametype]);
      break;
    default:
      return -1;
  }
  
  return frametype;
}
