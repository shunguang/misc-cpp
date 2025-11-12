#ifndef __ERROR_MSG_H__
#define __ERROR_MSG_H__

#include "Msg.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT ErrorMsg : public Msg{

	public:
		ErrorMsg( const std::string &s="", const uint32_t clientIp_=0 );
		~ErrorMsg()=default;
		ErrorMsg(const ErrorMsg &x);
		ErrorMsg& operator = (const ErrorMsg &x);

		virtual void read( Uint8Stream &is);
		virtual void write( Uint8Stream &os);
		virtual std::string toString() const;

	public:
		char buf[MSG_ERR_MSG_LEN];
		uint32_t	clientIp;
	};

	typedef boost::shared_ptr<ErrorMsg>		ErrorMsgPtr;

}
#endif
