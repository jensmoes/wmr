//Jens Troest 2014
//Main executable made for Raspbian but probably works for other linux dists as well.
//It will open the given serial port for read only and lock the thread until data is received
//If you do not want to lock the thread set VMIN to 0, this will cause read()3 to return immediately regardless of whether there is data. I recommend the locking as it will avoid any silly runtime loops for polling.
//When data is available it will be fed into the state machine which assembles bytes until a full WMR918 frame is detected. This frame is then decoded and output is generated.
//Currently output is to an XML file obeying the wmr.dtd but other methods can be added to the WMR class to extend output capabilities


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "wmr.h"
#include "sdstate.h"
#include "decode.h"
#include "xml_encode.h"

int main(int argc, char* argv[])
{
  if(argc == 1)
    {
      printf("Usage: %s <serial port> <out file>\n I.e. wmr /dev/tty1 ./out.xml\nDefault out file is ./current.xml\n",argv[0]);
      return 0;
    }

  int uartFileDescriptor = open(argv[1],O_RDONLY|O_RSYNC);
  printf("Opening %s result: %i",argv[1], uartFileDescriptor);
  if(uartFileDescriptor != -1)
    {
      printf("Success opening serial %s\n", argv[1]);
      struct termios options;
      tcgetattr(uartFileDescriptor, &options);
      options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;//<Set baud rate
      options.c_iflag = IGNPAR;
      options.c_oflag = 0;
      options.c_lflag = 0;
      options.c_cc[VMIN] = 1;
      tcflush(uartFileDescriptor, TCIFLUSH);
      tcsetattr(uartFileDescriptor, TCSANOW, &options);
      int read_len = 0;
      int state;
      unsigned char framebuf[20];
      WMR wmr;
      memset(&wmr,0,sizeof(WMR));

      while(read_len >= 0)
	{
	  char* read_buf = new(char[20]);
	  read_len = read(uartFileDescriptor, (void*)read_buf,20);
	  if(read_len < 0)
	    {
	      printf("Error reading serial %s\n",argv[1]);
	    }
	  else if(read_len == 0)

	    {
	      printf("Waiting, no data\n");
	    }
	  else
	    {
	      int state;
	      int i;
	      for(i=0;i<read_len;i++)
		{
		  state = wmr_statemachine(framebuf, read_buf[i]);
		  if(state == SDS_COMPLETE)//We have a whole frame
		    {
		      int channel=0;
		      unitptr_t unit;
		      int frametype = wmr_decode(&wmr, &channel, &unit,framebuf);
		      //Now translate the weather data into the generic format to be passed to the owner
		      if(FT_ERROR != frametype)
			{
			  //Do whatever needs to be done with the data in here
      			  std::string str = wmr2xml(&wmr);
			  //Send the XML for current framedata to the owner
			  std::string out;
			  switch(frametype)
			    {
			    case FT_WIND:
			      wmr.wind.unit.detected = true;
			      out = wind2xml(&(wmr.wind));
			      break;
			    case FT_RAIN:
			      wmr.rain.unit.detected = true;
			      out = rain2xml(&wmr.rain);
			      break;
			    case FT_BTH:
			    case FT_EXTBTH:
			      wmr.bth.unit.detected = true;
			      out = bth2xml(&wmr.bth);
			      break;
			    case FT_MUSHR:
			    case FT_TH:
			      wmr.th[channel].unit.detected = true;
			      out = th2xml(&(wmr.th[channel]), channel);
			      break;
			    case FT_TEMP:
			      wmr.temp[channel].unit.detected = true;
			      out = t2xml(&(wmr.temp[channel]), channel);
			      break;
			    case FT_MINUTE://TBD
			      break;
			    case FT_CLOCK:
			      wmr.clock.unit.detected = true;
			      out = clock2xml(wmr.clock);
			      break;
			    default:
			      break;
			    }
			  //			  			  if(out.c_str())
			  // Here we want to trigger an event that new data has been received or output the whole object
			    //Create methods in WMR that can print in XML and JSON			((CWeatherStationBase*) this->owner)->callback(out.c_str(), str->c_str());

			  //			    printf(out.c_str());
			  //			  std::string xml = wmr.printXML();
			  //			  printf(xml.c_str());
			  if(str.c_str())
			    {
			      FILE* file;
			      if(argc > 2)
				file = fopen(argv[2],"w+");
			      else
				file = fopen("./current.xml","w+");
			      if(file)
				{
				  fprintf(file,str.c_str());
				  fclose(file);
				}
			      else
				printf("Error opening file");
			    }
			  //Add current frame to the log
			}
		    }
		}
	    }
	  read_len=0;
	  delete read_buf;
	}
    }


  else
    {
      printf("Error opening %s\n",argv[1]);
    }


}
