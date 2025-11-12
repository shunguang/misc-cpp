#ifndef __CAM_CTRL_MSG_H__
#define __CAM_CTRL_MSG_H__

#include "Msg.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT CamCtrlMsg  : public Msg{

	public:
		CamCtrlMsg();
		~CamCtrlMsg();
		CamCtrlMsg(const CamCtrlMsg &x);
		CamCtrlMsg& operator = (const CamCtrlMsg &x);

		virtual void read( Uint8Stream &is);
		virtual void write( Uint8Stream &os);
		virtual std::string toString() const;

		bool isShutdownServer() { return flag1==1;}	
	public:
		uint8_t		apertureFlag;	//0-no chnage, 1-decrease index by one, 2--increase index by one
		uint8_t		focusFlag;      //0-no change, 1-focus out, 2--focus in
		uint8_t		exposureFlag;   //0-no change, 1-decrease exp time, 2--increasse exp time by 100 macros sec
		uint8_t	    flag1;          //1-shutdown,  2-todo   
		uint8_t	    authorization;  //autherization, 0-has, 1-on autherization
		uint32_t	clientIp;
	};

	typedef std::shared_ptr<CamCtrlMsg>		CamCtrlMsgPtr;

}
#endif
