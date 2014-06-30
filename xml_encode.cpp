//Jens Troest (c) 2007

//Library for outputting WMR data in XML.
//The XML is formatted according to the DTD defined in wmr.dtd

#include <stdio.h>
#include <string>
using namespace std;

#include "xml_encode.h"
//XML elements
string header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
string header_dtd = "<!DOCTYPE station SYSTEM \"wmr.dtd\">\n";
//Station
string stationbegin = "<station name=\"WMR968\">\n";
string stationend = "</station>";
string stationempty = "<station />";
//Date time
string datetimebegin = "<datetime>";
string datetimeend   = "</datetime>";
string datetimeempty = "<datetime />";
//Station clock
string clockempty = "<clock />";
string clockbegin = "<clock>";
string clockend   = "</clock>";
//System
string systemempty = "<system />";
string systembegin = "<system>";
string systemend   = "</system>";
char dateSeperator = '-';
char timeSeperator = ':';
//Battery status
string batbegin = "<battery>";
string batend   = "</battery>";
//Sensor
string sensorbegin = "<sensor name=\"%s\" type=\"%s\">";
string sensorend   = "</sensor>\n";
string sensorempty = "<sensor />\n";
//Temperature
string tempbegin = "<temperature name=\"%s\" unit=\"&#x2103;\">";
string tempend   = "</temperature>";
//Humidity
string humbegin = "<humidity>";
string humend = "</humidity>";
//Rain
string rainbegin = "<rain name=\"%s\" unit=\"mm\">";
string rainend = "</rain>";
string rainratebegin = "<rain name=\"%s\" unit=\"mm/hr\">";
//Wind
string dirbegin = "<winddirection unit=\"degrees\">";
string dirend   = "</winddirection>";
string dirempty = "<winddirection />";
string velocitybegin = "<windvelocity name=\"%s\" unit=\"m/s\">";
string velocityend   = "</windvelocity>";
//Baro Thermo Hygro
string version = "<version>%u</version>";
string pressbegin = "<pressure name=\"%s\" unit=\"hPa\">";
string pressend   = "</pressure>";
string fcbegin = "<forecast>";
string fcend = "</forecast>";
string skc = "Sunny";
string few = "Partly Cloudy";
string bkn = "Cloudy";
string rain = "Rain";
//Over and under
string over = "&gt;%.1f";
string under = "&lt;%.1f";

string wmr2xml(WMR* wmr)
{
	//Build output xml code reflecting current conditions
  string out;
  out = header + header_dtd;
  bool good = false;
  if(wmr)
    {
      out += stationbegin;
      //Insert timestamp for last update of station data (battery and clock are in the station)
      //Instert system time. Clock frame is only sent once an hour by the station
      out += systime2xml(wmr->sys_tm);
      //Insert station clock
      /*      if(wmr->clock.unit.clock_valid && !(wmr->use_sys_tm))
	out += clock2xml(wmr->clock);
      else
	{
	  out += clockempty;//Should I use system time?
	}
      out += bat2xml(wmr->clock.unit.lobat);
      out += wind2xml(&(wmr->wind));
      out += rain2xml(&wmr->rain);
      out += bth2xml(&wmr->bth);
      for(int i=0; i<TH_AMOUNT;i++)
      {
	out += th2xml(&wmr->th[i],i);
      }
      for(int i=0; i<TEMP_AMOUNT;i++)
      {
	out += t2xml(&wmr->temp[i],i+1);//No built in so increment index for correct number
      }*/
      out += bat2xml(wmr->clock.unit.lobat);
      if(wmr->clock.unit.detected)
	{
	  out += clock2xml(wmr->clock);
	  good = true;
	  }
      if(wmr->wind.unit.detected)
	{
	  out += wind2xml(&wmr->wind);
	  good = true;
	}
      if(wmr->rain.unit.detected)
	{
	  out += rain2xml(&wmr->rain);
	  good = true;
	}
      for(int i=0;i<TH_AMOUNT;i++)
	{
	  if(wmr->th[i].unit.detected)
	    {
	      out += th2xml(&wmr->th[i],i);
	      good = true;
	    }
	}
      for(int i=0;i<TEMP_AMOUNT;i++)
	{
	  if(wmr->temp[i].unit.detected)
	    {
	      out += t2xml(&wmr->temp[i],i+1);
	      good = true;
	    }
	}
      if(wmr->bth.unit.detected)
	{
	  out += bth2xml(&wmr->bth);
	  good = true;
	  }
      out += stationend;
    }
  else
    out = stationempty;
  return out;
}
//Convert time into XML in UTC
string time2xml(time_t time)
{
	string dtstr;
	tm* dt = gmtime(&time);
	if(dt && time)//If valid time and not zero time
	{
		char tmp[MAX_TEMP_STRING_LENGTH];
		dtstr = datetimebegin;
		sprintf(tmp, "%i", dt->tm_year+1900);
		dtstr += tmp;
		dtstr += dateSeperator;
		sprintf(tmp, "%02u",dt->tm_mon+1);
		dtstr += tmp;
		dtstr += dateSeperator;
		sprintf(tmp, "%02u",dt->tm_mday);
		dtstr += tmp;
		dtstr += 'T';
		sprintf(tmp, "%02u",dt->tm_hour);
		dtstr += tmp;
		dtstr += timeSeperator;
		sprintf(tmp, "%02u",dt->tm_min);
		dtstr += tmp;
		dtstr += timeSeperator;
		sprintf(tmp, "%02u",dt->tm_sec);
		dtstr += tmp;
		dtstr += 'Z';

		dtstr += datetimeend;
	}
	else
		dtstr = datetimeempty;
	return dtstr;
}
//Convert time into XML in UTC and put it under system tags
string systime2xml(time_t time)
{
	string tmp;
	tmp += systembegin;
	tmp+= time2xml(time);
	tmp+= systemend;
	return tmp;
}
//Convert station time into xml
string clock2xml(WMR_CLOCK clock)
{
	string clockstring = clockbegin;
	clockstring += time2xml(mktime(&clock.date_tm));
//Test	clockstring += time2xml(time(NULL));
	clockstring += clockend;
	return clockstring;
}
//Convert battery status into xml
string bat2xml(int bat)
{
	string batstr = batbegin;
	batstr += bat?"LOW":"OK";
	batstr += batend;
	return batstr;
}
//Convert relative humidity into xml
string hum2xml(WMR_SENS* hum)
{
	char tmp[MAX_TEMP_STRING_LENGTH];
	string humstr = humbegin;
	if(hum)
	{
		sprintf(tmp, "%u", hum->val);
		humstr += tmp;
	}
	humstr += humend;
	return humstr;
}
//Converts temperature into xml. Puts name in the name attribute. If name is NULL default is temperature
//If temp is NULL empty element will be created
string temp2xml(WMR_SENS* temp, const char* name)
{
	string tempstr;
	char tmp[MAX_TEMP_STRING_LENGTH];
	if(name)
		sprintf(tmp, tempbegin.c_str(), name);
	else
		sprintf(tmp, tempbegin.c_str(), "temperature");
	tempstr = tmp;
	if(temp)
	{
		if(temp->over)
		{
			string sign;
			if(temp->fval > 0)
				sprintf(tmp, over.c_str(), temp->fval);
			else
				sprintf(tmp, under.c_str(), temp->fval);
		}
		else
		{
			sprintf(tmp, "%.1f", temp->fval);
		}
		tempstr += tmp;
	}
	tempstr += tempend;
	return tempstr;
}

//Sensor element creation functions below:

//Convert wind sensor data to xml sensor element
string wind2xml(WMR_WIND* wind)
{
	char tmp[MAX_TEMP_STRING_LENGTH];
	string windstr;
	sprintf(tmp,sensorbegin.c_str(), "wind", "outdoor");
	windstr = tmp;
	if(wind->unit.framelen)//If there is a wind sensor connected create the data. Otherwise make an empty element to indicate the sensor is supported but not present
	{
		//Time of observation
		windstr += systime2xml(wind->unit.sys_tm);
		windstr += time2xml(wind->unit.clock_tm);
		//Battery status in sensor
		windstr += bat2xml(wind->unit.lobat);
		//Wind direction
		windstr += dirbegin;
		sprintf(tmp,"%03u", wind->dir.val);
		windstr += tmp;
		windstr += dirend;
		sprintf(tmp, velocitybegin.c_str(), "gust");
		windstr += tmp;
		if(wind->gust.over)
		{
			if(wind->gust.fval > 0)
				sprintf(tmp, over.c_str(), wind->gust.fval);
			else
				sprintf(tmp, under.c_str(), wind->gust.fval);
		}
		else
		{
			sprintf(tmp, "%.1f", wind->gust.fval);
		}
		windstr += tmp;
		windstr += velocityend;
		sprintf(tmp, velocitybegin.c_str(), "average");
		windstr += tmp;
		if(wind->avrg.over)
		{
			if(wind->avrg.fval > 0)
				sprintf(tmp, over.c_str(), wind->avrg.fval);
			else
				sprintf(tmp, under.c_str(), wind->avrg.fval);
		}
		else
		{
			sprintf(tmp, "%.1f", wind->avrg.fval);
		}
		windstr += tmp;
		windstr += velocityend;

		if(wind->chill_nodata)
		{
			//Put in an emptry windchill element to indicate it present but not available
			windstr += temp2xml(NULL, "windchill");
		}
		else
		{
			windstr += temp2xml(&wind->chill, "windchill");
		}
	}
	windstr += sensorend;
	return windstr;
}
//Convert rain sensor data into xml sensor element
string rain2xml(WMR_RAIN *rain)
{
	string rainstr;
	char tmp[MAX_TEMP_STRING_LENGTH];
	sprintf(tmp,sensorbegin.c_str(), "rain", "outdoor");
	rainstr = tmp;
	if(rain->unit.framelen)//If sensor is present
	{
		//Time of observation
		rainstr += systime2xml(rain->unit.sys_tm);
		rainstr += time2xml(rain->unit.clock_tm);
		//Battery status
		rainstr += bat2xml(rain->unit.lobat);
		//Current rate
		sprintf(tmp, rainratebegin.c_str(), "current");
		rainstr += tmp;
		sprintf(tmp, "%u" , rain->curr.val);
		rainstr += tmp;
		rainstr += rainend;

		//Total since date
		sprintf(tmp, rainbegin.c_str(), "total");
		rainstr += tmp;
		rainstr += time2xml(mktime(&rain->startdate));
		sprintf(tmp, "%.1f" , rain->total.fval);
		rainstr += tmp;
		rainstr += rainend;

		//Yesterdays rainfall
		sprintf(tmp, rainbegin.c_str(), "yesterday");
		rainstr += tmp;
		sprintf(tmp, "%u" , rain->yesterday.val);
		rainstr += tmp;
		rainstr += rainend;
	}
	rainstr += sensorend;
	return rainstr;
}

string bth2xml(WMR_BTH *bth)
{
	string bthstr;
	char tmp[MAX_TEMP_STRING_LENGTH];
	sprintf(tmp,sensorbegin.c_str(), "barothermohygro", "indoor");
	bthstr = tmp;
	if((bth->unit.framelen))
	{
		sprintf(tmp,version.c_str(), bth->version);
		bthstr += tmp;
		//Time of observation
		bthstr += systime2xml(bth->unit.sys_tm);
		bthstr += time2xml(bth->unit.clock_tm);
		//Battery status
		bthstr += bat2xml(bth->unit.lobat);
		bthstr += temp2xml(&bth->temp, NULL);
		bthstr += temp2xml(&bth->dew, "dewpoint");
		bthstr += hum2xml(&bth->hum);
		sprintf(tmp,pressbegin.c_str(), "station");
		bthstr += tmp;
		sprintf(tmp,"%u", bth->press.val);
		bthstr += tmp;
		bthstr += pressend;
		sprintf(tmp,pressbegin.c_str(), "sealevel");
		bthstr += tmp;
		sprintf(tmp,"%.01f", bth->sealevel.fval);
		bthstr += tmp;
		bthstr += pressend;
		bthstr += fcbegin;
		switch(bth->w_status)
		{
		case WST_SUNNY:
			bthstr += skc;
			break;
		case WST_HLFCLDY:
			bthstr += few;
			break;
		case WST_CLDY:
			bthstr += bkn;
			break;
		case WST_RAINY:
			bthstr += rain;
			break;
		default:
			break;
		}
		bthstr += fcend;
	}
	bthstr += sensorend;
	return bthstr;
}

string th2xml(WMR_TH* th, int number)
{
	string thstr;
	char tmp[MAX_TEMP_STRING_LENGTH];
	char type[MAX_TEMP_STRING_LENGTH];
	//Create location name for sensor
	if(number)
	{
		sprintf(type,"aux%u", number);
	}
	else
	{
		sprintf(type,"outdoor");
	}
	sprintf(tmp,sensorbegin.c_str(), "thermohygro", type);
	thstr = tmp;
	if(th->unit.framelen)
	{
		//Time of observation
		thstr += systime2xml(th->unit.sys_tm);
		thstr += time2xml(th->unit.clock_tm);
		//Battery status
		thstr += bat2xml(th->unit.lobat);
		thstr += temp2xml(&th->temp, NULL);
		thstr += temp2xml(&th->dew, "dewpoint");
		thstr += hum2xml(&th->hum);
	}
	thstr += sensorend;
	return thstr;
}

string t2xml(WMR_T* t, int number)
{
	string tstr;
	char tmp[MAX_TEMP_STRING_LENGTH];
	char type[MAX_TEMP_STRING_LENGTH];
	//Create location name for sensor
	if(number)
	{
		sprintf(type,"aux%u", number);
	}
	else
	{
		sprintf(type,"outdoor");
	}
	sprintf(tmp,sensorbegin.c_str(), "thermo", type);
	tstr = tmp;
	if(t->unit.framelen)
	{
		//Time of observation
		tstr += systime2xml(t->unit.sys_tm);
		tstr += time2xml(t->unit.clock_tm);
		//Battery status
		tstr += bat2xml(t->unit.lobat);
		tstr += temp2xml(&t->temp, NULL);
		printf("\n\nA T SENSOR DETECTED, temp = %f over=%i\n\n",t->temp.fval,t->temp.over);
	}
	tstr += sensorend;
	return tstr;
}
