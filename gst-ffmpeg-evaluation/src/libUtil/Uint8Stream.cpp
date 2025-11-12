#include <assert.h>
#include <fstream>
#include <iostream>
#include "Uint8Stream.h"

using namespace ngv;

Uint8Stream::Uint8Stream( const uint32_t capacity )
	: m_size	( 0 )
	, m_capacity( capacity )
	, m_buf( NULL)
	, m_pWrt( NULL )
	, m_readCnt( 0 )
#if ENABLE_CRC
	, m_crc()
#endif
{
	creatBuf();
}

Uint8Stream::~Uint8Stream()
{
	deleteBuf();
	m_size		= 0;
	m_capacity	= 0;
}

void Uint8Stream::resetWrite()
{
	m_size = 0;
	m_pWrt = m_buf;
#if ENABLE_CRC
	m_crc.reset();
#endif
}

uint16_t  Uint8Stream::getChecksum() const
{
#if ENABLE_CRC
	return m_crc.getCrcValue();
#else
	return 0;
#endif
}

uint16_t  Uint8Stream::getCrcLength() const
{
#if ENABLE_CRC
	return m_crc.getLength();
#else
	return 0;
#endif
}

void Uint8Stream::getData( std::vector<uint8_t> &v ) const
{
	uint32_t n = size();
	const uint8_t *p = getData();
	v.assign(p, p+n);
}

const uint8_t*  Uint8Stream::getData( const uint32_t offset ) const
{
	assert( offset< m_size );

	return m_buf + offset;
}

uint32_t Uint8Stream::size() const
{
	return m_size;
}

uint32_t	Uint8Stream::capacity() const
{
	return m_capacity;
}

void Uint8Stream::writeAt( const uint32_t loc, const uint32_t value )
{
	assert( (loc+4) <= m_size );

	uint8_t *q = (uint8_t *)&value;
	uint8_t *p = m_buf + loc;
	for( uint32_t i=0; i<4; i++ ){
		*p = *q;

		++p;
		++q;
	}
}

void Uint8Stream::write( const int16_t x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p, 2, updateCrc  );
}

void Uint8Stream::write( const int32_t x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p,	4, updateCrc  );
}

void Uint8Stream::write( const uint16_t x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p,	2, updateCrc  );
}

void Uint8Stream::write( const uint32_t x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p,	4, updateCrc  );
}

void Uint8Stream::write( const uint64_t x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p, 8, updateCrc  );
}

void Uint8Stream::write( const float x, const bool updateCrc  )
{
	const uint8_t *p = ( uint8_t *)(&x);
	write( p,	4, updateCrc  );
}

void Uint8Stream::write( const std::string &x, const bool updateCrc  )
{
	//uint8_t nullChar = '\0';
	uint32_t n = x.size();

	write( n, updateCrc );
	for( uint32_t i=0; i<n; ++i ){
		write( (uint8_t)(x[i]), updateCrc );
	}
	//write( nullChar, updateCrc );
}

void Uint8Stream::write( const uint8_t *p, const uint32_t len, const bool updateCrc  )
{
	for( uint32_t i=0; i<len; ++i ){
		write( p[i], updateCrc );
	}
}

void Uint8Stream::write( const int8_t byte, const bool updateCrc  )
{
	const uint8_t *p = (uint8_t *)&byte;
	write( *p, updateCrc  );
}

void Uint8Stream::write( const uint8_t byte, const bool updateCrc  )
{
	if( m_size>=m_capacity ){
		extendBuf( m_capacity );
	}

	*m_pWrt = byte;
	++m_pWrt;
	++m_size;

#if ENABLE_CRC
	if( updateCrc ){
		m_crc.update( byte );
	}
#if _DEBUG
	assert( m_size > m_crc.getLength() );
#endif
	//std::cout << "size=" << m_size << "; len=" << m_crc.getLength() << std::endl;
#endif
}

void Uint8Stream::resetRead()
{
	m_readCnt = 0;
}

bool Uint8Stream::read( int8_t &byte)
{
	if( m_readCnt <=  m_size ){
		byte = (int8_t)(m_buf[m_readCnt]);
		++m_readCnt;
		return true;
	}
	return false;
}

bool Uint8Stream::read( uint8_t &byte)
{
	if( m_readCnt <=  m_size ){
		byte = m_buf[m_readCnt];
		++m_readCnt;
		return true;
	}
	return false;
}

bool Uint8Stream::read( uint8_t *p, const uint32_t len)
{
	if( m_readCnt+len <=  m_size ){
		memcpy( p, m_buf+m_readCnt, len );
		m_readCnt += len;
		return true;
	}
	return false;
}

bool Uint8Stream::read( int16_t &x )
{
	if( m_readCnt+2 <=  m_size ){
		int16_t *p = ( int16_t *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 2;
		return true;
	}
	return false;
}

bool Uint8Stream::read( int32_t &x )
{
	if( m_readCnt+4 <=  m_size ){
		int32_t *p = ( int32_t *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 4;
		return true;
	}
	return false;
}

bool Uint8Stream::read( uint16_t &x )
{
	if( m_readCnt+2 <=  m_size ){
		uint16_t *p = ( uint16_t *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 2;
		return true;
	}
	return false;
}

bool Uint8Stream::read( uint32_t &x )
{
	if( m_readCnt+4 <=  m_size ){
		uint32_t *p = ( uint32_t *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 4;
		return true;
	}
	return false;
}

bool Uint8Stream::read( uint64_t &x )
{
	if( m_readCnt+8 <=  m_size ){
		uint64_t *p = ( uint64_t *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 8;
		return true;
	}
	return false;
}

bool Uint8Stream::read( float &x )
{
	if( m_readCnt+4 <=  m_size ){
		float *p = ( float *)(m_buf + m_readCnt);
		x = *p;
		m_readCnt += 4;
		return true;
	}
	return false;
}

bool Uint8Stream::read( std::string &x )
{
	uint32_t len;
	if( !read(len) ){
		x="";
		return false;
	}

	uint8_t ch;
	for( uint32_t i=0; i<len; ++i ){
		if( read( ch ) ){
			if ( ch != '\0' ){
				x.push_back(ch);
			}
		}
	}
	return true;
}

void Uint8Stream::dumpTxt( const char* fileName ) const
{
	if ( fileName == 0 ){
		printf( "index, byte value(dec), value(hex), char\n" );
		for (uint32_t i = 0; i < m_size; ++i){
			printf( "%d,%d,%0x,%c\n", i, m_buf[i], m_buf[i], m_buf[i] );
		}
	}
	else{
		FILE *fp = fopen(fileName, "w");
		if (fp){
#if 1
			for (uint32_t i = 0; i < m_size; ++i){
				fprintf(fp, "%d,",  m_buf[i]);
			}
#if ENABLE_CRC			
			fprintf(fp, "crc=%d\n", m_crc.getCrcValue() );
#endif
			
#else
			fprintf(fp, "index, byte value(dec), value(hex), char\n");
			for (uint32_t i = 0; i < m_size; ++i){
				fprintf(fp, "%d,%d,0x%2X\n", i, m_buf[i], m_buf[i]);
			}
#endif
		}
		fclose(fp);
	}
}

void Uint8Stream::loadBin( const char* fileName )
{
	std::ifstream fs( fileName, std::ios::in | std::ios::binary);
	if( fs.is_open() ){
		fs.seekg(0, std::ios::end);
		uint32_t size = fs.tellg() ;
		uint8_t *buf = new uint8_t [size];
		
		fs.seekg(0, std::ios::beg);
		fs.read( (char*)buf, size );
		fs.close();

		resetWrite();
		write( buf, size, false );

		delete [] buf;
	}
	fs.close();
}

void Uint8Stream::dumpBin( const char* fileName ) const
{
	std::ofstream fs( fileName, std::ios::out | std::ios::binary | std::ios::trunc);
	if( fs.is_open() ){
		fs.write( (char *)m_buf, m_size );
	}
	fs.close();
}

void Uint8Stream :: extendBuf( const uint32_t nAddtionalBytes )
{
	//save $m_buf$ into $tmpBuf$;
	uint32_t oldCapacity = m_capacity;
	uint8_t *tmpBuf = new uint8_t[oldCapacity];
	memcpy( tmpBuf, m_buf, oldCapacity );

	//extend $m_buf$
	m_capacity += nAddtionalBytes;
	deleteBuf();
	creatBuf();

	//copy the saved data into $m_buf$
	memcpy( m_buf, tmpBuf, oldCapacity );
	m_size = oldCapacity;
	m_pWrt = m_buf + m_size;

	//delete tmporal data
	delete [] tmpBuf;
}

void Uint8Stream::creatBuf()
{
	if ( m_capacity > 0 ){
		if (m_buf){
			deleteBuf();
		}

		m_buf = new uint8_t[m_capacity];
		if ( !m_buf ){
			printf( "Uint8Stream::creatBuf(): cannot allocate memory!" );
			exit(-1);
		}
		m_size = 0;
		m_pWrt = m_buf;
		m_readCnt = 0;
	}
	else{
		m_size = 0;
		m_capacity = 0;
		m_buf = NULL;
	}
}
void Uint8Stream::deleteBuf()
{
	if ( m_buf ){
		delete [] m_buf;
		m_buf = NULL;
		m_pWrt = NULL;
	}
}
