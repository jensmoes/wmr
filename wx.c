//Jens Troest 2014
/* Main executable made for Raspbian but probably works for other linux dists as well.
   
   It will open the given serial port for read only and lock the thread until data is received
   
   If you do not want to lock the thread set VMIN to 0, this will cause read() (see man page) to return immediately regardless of whether there is data. I recommend the locking as it will avoid any silly runtime loops for polling.
   
   When data is available it will be fed into the state machine byte by byte. It assembles bytes until a full WMR918 frame is detected. This frame is then decoded and output is generated.
   
   Currently output is to an XML file obeying the wmr.dtd but other methods can be added to the WMR class to extend output capabilities.
   The current sensor (unit) XML is sent to stdout
*/

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
  //  printf("Opening %s result: %i\n",argv[1], uartFileDescriptor);
  if(uartFileDescriptor != -1)
    {
      //printf("Success opening serial %s\n", argv[1]);
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
		      //Now decode the frame. 
		      //It is added to wmr, channel will hold the current channel if it is a sensor with channel support.
		      //unit will point to the current sensor that was detected in the frame
		      //framebuf is the binary data from the statemachine
		      int frametype = wmr_decode(&wmr, &channel, &unit,framebuf);
		      if(FT_ERROR != frametype)
			{
			  //Do whatever needs to be done with the data in here
			  //Create a string containing this one element for stdout
			  std::string out;
			  switch(frametype)
			    {
			    case FT_WIND:
			      out = wind2xml(&(wmr.wind));
			      break;
			    case FT_RAIN:
			      out = rain2xml(&wmr.rain);
			      break;
			    case FT_BTH:
			    case FT_EXTBTH:
			      out = bth2xml(&wmr.bth);
			      break;
			    case FT_MUSHR:
			    case FT_TH:
			      out = th2xml(&(wmr.th[channel]), channel);
			      break;
			    case FT_TEMP:
			      out = t2xml(&(wmr.temp[channel]), channel);
			      break;
			    case FT_MINUTE://TBD
			      break;
			    case FT_CLOCK:
			      out = clock2xml(wmr.clock);
			      break;
			    default:
			      break;
			    }
			  //Standard out: Output the current element
  			  if(out.c_str())
			    printf(out.c_str());
			  //Output entire XML to file
			    std::string str = wmr.printXML();
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
