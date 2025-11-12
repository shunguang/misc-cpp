#ifndef __MSG_H__
#define __MSG_H__

#include "libUtil/AppEnums.h"
#include "libUtil/UtilFuncs.h"
#include "libUtil/UtilIo.h"

#include "Uint8Stream.h"
#include "MsgEnums.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT MsgHd{
	public:
		MsgHd(uint32_t syncWord_ = 0, uint32_t type_ = 0, uint32_t length_ = 0) : syncWord(syncWord_), type(type_), length(length_){}
		bool operator == (const MsgHd &x) const
		{
			//only compare the items shown in GUI
			if (x.syncWord != syncWord) { return false; }
			if (x.type != type) { return false; }
			if (x.length != length) { return false; }
			return true;
		}

		std::string toString() const
		{
			std::ostringstream ss;   // stream used for the conversion
			ss << "syncWord=" << syncWord;
			ss << ", type=" << type << "("<< g_msgTypeLabel[type]<<")";
			ss << ", length=" << length << ",";
			return ss.str();
		}

	public:
		uint32_t syncWord;    //4
		uint32_t type;        //4, msg type
		uint32_t length;      //4, the bytes of msg body part
	};

	//pure virtual class for all msg kind of data structures
	class MSG_EXPORT Msg{
	public:
		Msg();
		virtual ~Msg();
		Msg( const uint32_t syncWord_, const uint32_t type_ );
		Msg( const Msg &x );
		Msg& operator = (  const Msg &x );
		
		boost::posix_time::ptime getTimestamp() const { 
			return ts; 
		}
		void setTimestamp( const boost::posix_time::ptime &ts_= MSG_LOCAL_TIMESTAMP_NOW){ 
			ts = ts_; 
		}
		void activateAcknFlag()    { 
			reserved = MSG_ACK; 
		}
		void deactivateAcknFlag() { 
			reserved = MSG_ORI; 
		}

		bool isSameTimestamp( const boost::posix_time::ptime &ts_ ) const { 
			return (ts == ts_); 
		}
		std::string getTypeName() const { 
			return getMsgTypeLabel(hd.type); 
		}
		MsgHd    getHead() const		{ 
			return hd; 
		}

		bool verifyLengthAndChecksum();
		//read msg from raw string
		//return true if pass crc check, false otherwise 
		bool readFromStr( const std::string &s );

		//virtual methods
		virtual void read(Uint8Stream &is) = 0;  //read from a stream to parse
		virtual void write(Uint8Stream &os) = 0; //write to a stream to send
		virtual void dumpTxt(const std::string &fileName, const uint16_t crc = 0) const;
		//dump/load from binary
		virtual void dumpBinary( const std::string &path );
		virtual void loadBinary( const std::string &fileName );

		//dump to txt
		virtual std::string toString() const;
		std::string getHeadStr() const;
		std::string getTailStr() const;

	public:
		//Msg header
		MsgHd hd;  //12 bytes
		
		//Msg body defined inside each msg
		// ....
		uint16_t reserved;   //0 - original msg, 1-acknowledgement messege w/ same header and body  if need

		//Msg tail
		uint16_t checksum;
	protected:
	    boost::posix_time::ptime ts; //server timestamp used as a Msg id
	};
	typedef std::shared_ptr<Msg>		MsgPtr;

	//return true if msg.size()>=16, false otherwise
	uint32_t MSG_EXPORT getMsgType(const std::string& msg);
	bool MSG_EXPORT getMsgInfo(const std::string& msg, uint32_t& synWord, uint32_t& type, uint32_t& len, uint16_t& crc);
	bool MSG_EXPORT getMsgFromRawStr(std::string& raw, std::string& msg);
}
#endif
