#ifndef _UINT8_STREAM_H
#define _UINT8_STREAM_H 1

#include <math.h>
#include <stdio.h>
#include <memory.h>

#if _WINDOWS
#include <conio.h>
#endif

#include <string>
#include <vector>

#define ENABLE_CRC 0

#if ENABLE_CRC
	#include "CRC.h"
#endif

namespace ngv {
	class Uint8Stream {
	public:
		Uint8Stream(const uint32_t capacity = 1024);

		~Uint8Stream();

		uint16_t  getCrcLength() const;
		uint16_t  getChecksum() const;
		
		void getData( std::vector<uint8_t> &v ) const;
		const uint8_t*  getData(const uint32_t offset = 0) const ;
		uint32_t  size() const; //get the size # 
		uint32_t  capacity() const; //get the size # 

		void	resetWrite();
		//push all kind of data at the end of $m_buf$
		void write(const int8_t byte, const bool updateCrc = true);
		void write(const uint8_t byte, const bool updateCrc = true);
		void write(const uint8_t *p, const uint32_t len, const bool updateCrc = true);
		void write(const int16_t x, const bool updateCrc = true);
		void write(const int32_t x, const bool updateCrc = true);
		void write(const uint16_t x, const bool updateCrc = true);
		void write(const uint32_t x, const bool updateCrc = true);
		void write(const uint64_t x, const bool updateCrc = true);
		void write(const float x, const bool updateCrc = true);
		void write(const std::string &s, const bool updateCrc = true);

		//overitten 4 bytes starting at $m_buf[loc]$, do not change crc
		void writeAt(const uint32_t loc, const uint32_t value);

		void resetRead();
		bool read(int8_t &byte);
		bool read(uint8_t &byte);
		bool read(uint8_t *p, const uint32_t len);
		bool read(int16_t &x);
		bool read(int32_t &x);
		bool read(uint16_t &x);
		bool read(uint32_t &x);
		bool read(uint64_t &x);
		bool read(float &x);
		bool read(std::string &x);

		void loadBin(const char* fileName);
		void dumpBin(const char* fileName) const;

		void dumpTxt(const char* fileName=0) const;

	protected:
		void creatBuf();
		void deleteBuf();
		void extendBuf(const uint32_t nAddtionalBytes);

	protected:
		uint32_t m_size;		//size of <m_buf>
		uint32_t m_capacity;    //capacity of <m_buf>
		uint8_t* m_buf;			//the buffer holding data
		uint8_t* m_pWrt;		//the pointer points to the next unit to write 
		uint32_t m_readCnt;
#if ENABLE_CRC
		CRC16  m_crc;
#endif
	};
}
#endif

