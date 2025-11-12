/*
brief 16-bit cyclic redundancy check.
16-bit cyclic redundancy check for blocks of 8-bit data of arbitrary length.
*/

/*
 * This file provided by:        
 * Cloud Cap Technology, Inc.    
 * PO Box 1500                   
 * No. 8 Fourth St.              
 * Hood River, OR, 97031         
 * +1-541-387-2120    (voice)    
 * +1-541-387-2030    (fax)      
 * http: www.cloudcaptech.com   
*/
#ifndef _CRC_H_
#define _CRC_H_

#include <iostream>
#include <string>

#include "MsgDefs.h"

#define CRC_CPP			1
namespace app {
#if CRC_CPP
	#define CRC_CCITT_POLYNOMIAL	0x1021
	class MSG_EXPORT CRC16{
		public:
			CRC16( const uint16_t seed_ = 0xFFFF );
			~CRC16();

			//return the CRC value
			uint16_t getCrcValue() const;

			//return total # of bytes in the crc stream
			uint16_t getLength() const;
			/**
			* Updates the CRC with the next byte.
			* @param b the byte to add to the CRC
			*/
			void update(const uint8_t byte);

			/*! Calculates a 16-bit cyclic redundancy check on blocks of 8-bit data of arbitrary length.
            \param pBuf points to a buffer of data.
            \param len is the size of the buffer in bytes.
            \return The 16-bit crc of the buffer.
			*/
			void update( const uint8_t *p, uint32_t len );

			void reset();

			std::string toString() const;

		private:
			uint16_t CRC_LUT[256];
			uint16_t seed;
			uint16_t crc;
			uint16_t length;
	};
#else
		unsigned short UTIL_EXPORT CRC16(const unsigned char* pBuf, int len);
		unsigned short UTIL_EXPORT CRC16OneByte(unsigned char Byte, unsigned short crc);
#endif
}
#endif
