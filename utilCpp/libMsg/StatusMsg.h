#ifndef __STATUS_MSG_H__
#define __STATUS_MSG_H__

#include "Msg.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT StatusMsg : public Msg{

	public:
		StatusMsg(const uint8_t perct_=0, const uint16_t errMsk_ = MSG_UINT16_RESERVED);
		~StatusMsg();
		StatusMsg(const StatusMsg &x);
		StatusMsg& operator = (const StatusMsg &x);

		virtual void read( Uint8Stream &is);
		virtual void write( Uint8Stream &os);
		virtual std::string toString() const;

	public:
		uint8_t		perct;		            //free disk percet [0,100] good and percentage, 255 for bad status
		uint8_t		reserved8;             
		uint16_t	reserved16;
	};

	typedef std::shared_ptr<StatusMsg>		StatusMsgPtr;

}
#endif
