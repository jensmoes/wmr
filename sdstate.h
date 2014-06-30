#ifndef __sdstate_h__
#define __sdstate_h__

#include "wmr.h"
// states of the serial data receiving state machine
#define SDS_NULL	0			// undefined state
#define SDS_START1	1			// first start byte received state
#define SDS_START2	2			// second start byte received state
#define SDS_DATA	3			// proper data frame type byte received state
#define SDS_COMPLETE	4		// proper checksum received state
#define SDS_RESYNCING	5		// while seeking for a first 0xff after error
#define SDS_RESYNCING2	6		// while seeking for a 2nd 0xff after error
// error indicating states
#define SDS_ERR_CHKSUM		-1	// bad checksum
#define SDS_ERR_ILL_STATE	-2	// should never happen (programming error)
#define SDS_ERR_SYNC		-3	// not a proper start frame sequence received
#define SDS_ERR_FRAMETYPE	-4	// non existent frame type byte received
#define SDS_ERR_READ            -5      // error when reading from serial port


/* This function incoming serial characters are handed over to one by one.
 * it detects data frames' start pattern and checks byte count of the frames by
 * their type byte and also checks checksum. 
 * Returns the state, SDS_COMPLETE means that a complete frame has arrived with
 * a good checksum.
 */
int wmr_statemachine(unsigned char *framebuf, unsigned char ch);

#endif	
	
