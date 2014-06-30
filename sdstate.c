//Jens Troest (C) 2007
//State machine handling the serial data stream
//It takes one byte at a time and returns it updated state
#include "sdstate.h"
#include "wmr.h"
// static variables holding state of the statemachine

static int sd_state = SDS_NULL;
static int framelen, framecnt;
static unsigned int run_chksum;

//Output:framebuf is a pointer to the buffer containing the current frame
//Input:ch is the byte the caller wishes to add to the buffer
int wmr_statemachine(unsigned char *framebuf, unsigned char ch)
{
  switch (sd_state)
  {
    case SDS_NULL:
    case SDS_ERR_CHKSUM:
    case SDS_ERR_ILL_STATE:
    case SDS_ERR_SYNC:
    case SDS_ERR_FRAMETYPE:
    case SDS_ERR_READ:
      if (ch == 0xff)
        sd_state = SDS_RESYNCING2;
      else
        sd_state = SDS_RESYNCING;
      break;
    case SDS_RESYNCING:
      if (ch == 0xff)
        sd_state = SDS_RESYNCING2;
      break;
    case SDS_RESYNCING2:
      if (ch == 0xff)
        sd_state = SDS_START2;
      else
        sd_state = SDS_RESYNCING;
      break;
    case SDS_COMPLETE:
//      fprintf(stderr," SDS_COMPLETE");
      if (ch == 0xff)
        sd_state = SDS_START1;
      else
        sd_state = SDS_ERR_SYNC;
      break;
    case SDS_START1:
//      fprintf(stderr," SDS_START1 ");
      if (ch == 0xff)
        sd_state = SDS_START2;
      else
        sd_state = SDS_ERR_SYNC;
      break;
    case SDS_START2:
//      fprintf(stderr," SDS_START2 ");
      sd_state = SDS_DATA;
      if (ch >= FT_MAX) {
        sd_state = SDS_ERR_FRAMETYPE;
        break;
      }
      framelen = datalen[ch];
      if (framelen == 0) {
        sd_state = SDS_ERR_FRAMETYPE;
        break;
      }
      run_chksum = 0xff + 0xff + ch;
      framebuf[0] = ch;
      framecnt=1;
      break;
    case SDS_DATA:
      if (framecnt < framelen) {
        framebuf[framecnt] = ch;
        run_chksum += ch;

//        fprintf(stderr," SDS_DATA(cnt=%0d data=%02x chksum=%02x) ",framecnt,
//          ch,run_chksum);

        framecnt++;
      } else {
        run_chksum &= 0xff;
        if (run_chksum == ch) {
          sd_state = SDS_COMPLETE;
          framebuf[framecnt] = 0;

//          fprintf(stderr," SDS_DATA(cnt=%0d data=%02x chksum=%02x) (checksum ok)",framecnt,
//            ch,run_chksum);
        } else {
//          fprintf(stderr," SDS_DATA(cnt=%0d data=%02x chksum=%02x) ",framecnt,
//            ch,run_chksum);

//          fprintf(stderr, "checksum error: calculated %02x, received %02x !", run_chksum, ch);
          sd_state = SDS_ERR_CHKSUM;
        }

      }
//      fprintf(stderr,"\n");      
      break;
    default:
//      fprintf(stderr," SDS_ILLEGAL!!! ");        	
      sd_state = SDS_ERR_ILL_STATE;
  }
  
  return sd_state;
}
