#ifndef __STAB_MSG_H__
#define __STAB_MSG_H__

#include "Msg.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT StabMsg : public Msg{

	public:
		StabMsg(const uint8_t stabOn_=0, const uint32_t	clientIp_=0);
		~StabMsg();
		StabMsg(const StabMsg &x);
		StabMsg& operator = (const StabMsg &x);

		virtual void read( Uint8Stream &is);
		virtual void write( Uint8Stream &os);
		virtual std::string toString() const;

	public:
		uint8_t		stabOn;		            //0-stab off, 1-stab on
		uint8_t		authorization; 			//0-no, 1-has             
		uint16_t	reserved16;             //0--status send to client
		uint32_t	clientIp;
	};

	typedef std::shared_ptr<StabMsg>		StabMsgPtr;

}
#endif
