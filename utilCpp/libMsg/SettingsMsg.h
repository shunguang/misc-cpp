#ifndef __SETTINGS_MSG_H__
#define __SETTINGS_MSG_H__

#include "libUtil/Roi.h"
#include "Msg.h"
#include "MsgDefs.h"

#define NGV_SET_START				((uint16_t)(1UL << 0))		// 0000 0000 0000 0001, start connect
#define NGV_SET_PLAY  				((uint16_t)(1UL << 1))		// 0000 0000 0000 0010, start connect, play or pause
#define NGV_SET_CLOSE_PREV_STREAM  	((uint16_t)(1UL << 2))		// 0000 0000 0000 0011, client  request to close and quit
#define NGV_SET_EXIT				((uint16_t)(1UL << 3))		// 0000 0000 0000 1100
#define NGV_SET_PREM				((uint16_t)(1UL << 4))      //if it is a premier client 

#define NGV_SET_LFC_SAVE			((uint16_t)(1UL << 5))	  // 0000 0000 0001 0000
#define NGV_SET_LFC_DWLD			((uint16_t)(1UL << 6))    //download 
#define NGV_SET_TURRET_SNAPSHOT		((uint16_t)(1UL << 7))
#define NGV_SET_REC					((uint16_t)(1UL << 8))    //start recording
//#define NGV_SET_SHUTDOWN			((uint16_t)(1UL << 9))    //shutdown server 

//camera status flags
#define NGV_STAB_FLAG				((uint16_t)(1UL << 10))    //stab is on/off
#define NGV_AE_FLAG					((uint16_t)(1UL << 11))    //auto exposure

namespace app {
	class Settings {
	public:
		Settings( const uint16_t f=0, const uint32_t ip=0, const uint16_t port=0) : flags(f), rtspPort(port), clientIp(ip),
			ngvMode(NGV_MODE_SLEEP), wamiMode(BROWSE_MODE_BKG), chipL(0), chipX(0), chipY(0), chipW(0), chipH(0),
			bkgImgW(0), bkgImgH(0), bkgPyrL(0), fps(0), lfcFrmRateIdx(0), turretImgSzIdx(0), turretZoomIdx(0), errCode(0), nClients(0), rsvd(0) {}
		Settings(const Settings &rhs) = default;
		Settings(Settings  &&rhs) = default;
		~Settings() = default;

		Settings &operator=(const Settings &rhs) = default;
		Settings &operator=(Settings &&rhs) = default;

		bool operator == (const Settings &x) const {
			if (flags != x.flags) { return false; }
			if (rtspPort != x.rtspPort) { return false; }
			if (clientIp != x.clientIp) { return false; }
			if (ngvMode != x.ngvMode) { return false; }
			if (wamiMode != x.wamiMode) { return false; }
			if (chipL != x.chipL) { return false; }
			if (chipX != x.chipX) { return false; }
			if (chipY != x.chipY) { return false; }
			if (chipW != x.chipW) { return false; }
			if (chipH != x.chipH) { return false; }
			if (bkgImgW != x.bkgImgW) { return false; }
			if (bkgImgH != x.bkgImgH) { return false; }
			if (bkgPyrL != x.bkgPyrL) { return false; }
			if (fps != x.fps) { return false; }
			if (lfcFrmRateIdx != x.lfcFrmRateIdx) { return false; }
			if (turretImgSzIdx != x.turretImgSzIdx) { return false; }
			if (turretZoomIdx != x.turretZoomIdx) { return false; }
			return true;
		}
		bool operator != (const Settings &x) const {
			return !(operator == (x));
		}

		bool isStart() const { return (flags & NGV_SET_START); }
		bool isPlay()  const { return (flags & NGV_SET_PLAY); }
		bool isPause()  const { return !isPlay(); }

		bool isClosePrevStream() const	{ return (flags & NGV_SET_CLOSE_PREV_STREAM); }
		bool isExit()  const			{ return (flags & NGV_SET_EXIT); }
		bool isPrem()  const { return (flags & NGV_SET_PREM); }

		bool isLfcSave()  const			{ return (flags & NGV_SET_LFC_SAVE); }
		bool isLfcDownload()  const		{ return (flags & NGV_SET_LFC_DWLD); }
		bool isTurretSnapShot()  const 		{ return (flags & NGV_SET_TURRET_SNAPSHOT); }
		bool isWarmiStartVideoRec()  const 	{ return (flags & NGV_SET_REC); }
		//bool isShutDown()  const 				{ return (flags & NGV_SET_SHUTDOWN); }
		
		bool isStabOn()  const 				{ return (flags & NGV_STAB_FLAG); }
		bool isAutoExpOn()  const 			{ return (flags & NGV_AE_FLAG); }

		//set methods	
		void resetFlags()		{ flags = 0; }
		void setStart( bool f)	{ (f) ? (flags |= NGV_SET_START) : (flags &= ~(NGV_SET_START) ); }
		void setPlay()  { flags |= NGV_SET_PLAY; }
		void setPause() { flags &= ~(NGV_SET_PLAY); }

		void setClosePrevStream(bool f) { (f) ? (flags |= NGV_SET_CLOSE_PREV_STREAM) : (flags &= ~(NGV_SET_CLOSE_PREV_STREAM)); }
		void setExit( bool f)	{ (f) ? (flags |= NGV_SET_EXIT)  : (flags &= ~(NGV_SET_EXIT));   }
		void setPrem(bool f) { (f) ? (flags |= NGV_SET_PREM) : (flags &= ~(NGV_SET_PREM)); }

		void setLfcSave(bool f) { (f) ? (flags |= NGV_SET_LFC_SAVE) : (flags &= ~(NGV_SET_LFC_SAVE)); }
		void setLfcDownload(bool f) { (f) ? (flags |= NGV_SET_LFC_DWLD) : (flags &= ~(NGV_SET_LFC_DWLD)); }
		void setTurretSnapShot(bool f) { (f) ? (flags |= NGV_SET_TURRET_SNAPSHOT) : (flags &= ~(NGV_SET_TURRET_SNAPSHOT)); }
		void setWamiStartVideoRec(bool f) { (f) ? (flags |= NGV_SET_REC) : (flags &= ~(NGV_SET_REC)); }
		//void setShutdown(bool f) { (f) ? (flags |= NGV_SET_SHUTDOWN) : (flags &= ~(NGV_SET_SHUTDOWN)); }

		void setStabFlag(bool f) 	{ (f) ? (flags |= NGV_STAB_FLAG) : (flags &= ~(NGV_STAB_FLAG)); }
		void setAutoExpFlag(bool f) { (f) ? (flags |= NGV_AE_FLAG) : (flags &= ~(NGV_AE_FLAG)); }

		
		void setLfcFrmRateIdx(uint8_t idx) { lfcFrmRateIdx = idx; }
		void setTurretImgSzIdx(uint8_t idx) { turretImgSzIdx = idx; }
		void setTurretZoomIdx(uint8_t idx) { turretZoomIdx = idx; }

		//original image size from camera
		ImgSize getYuvImgSize() const{
			ImgSize sz;
			if (wamiMode == (uint8_t)BROWSE_MODE_BKG) {
				sz.w = bkgImgW;
				sz.h = bkgImgH;
			}
			else if (wamiMode == (uint8_t)BROWSE_MODE_CHIP) {
				sz.w = chipW;
				sz.h = chipH;
			}
			else if (wamiMode == (uint8_t)BROWSE_MODE_PIC_IN_PIC) {
				sz.w = bkgImgW + chipW;
				sz.h = APP_MAX(bkgImgH, chipH);
			}
			else if (wamiMode == (uint8_t)BROWSE_MODE_REC) {
				sz.w = bkgImgW;
				sz.h = bkgImgH;
			}
			else {
				appAssert(false, "getYuvImgSize(): wrong wamiMode flag!");
			}
			return sz;
		}

		int getLfcTimeInterval_ms() const {
			//lfcFrmRateIdx=0 <----> 1Hz
			//lfcFrmRateIdx=1 <----> 1/2 Hz
			//lfcFrmRateIdx=2 <----> 1/3 Hz
			//...
			return (lfcFrmRateIdx + 1) * 1000;
		}


		//regular case: chipL>=0, chip rect(x,y,W,H) at level <chipL>; 
		//digital zoom: chipL<0,  (chipW, chipH) is the destination size, (chipX,chipY) is the top left corner the ROI_L0, 
		//              and  (ROI_L0.width, ROI_L0.height) = (chipW/k, chipH/k), where k = 2^( |chipL| )  
		bool isDigitalZoom() const{
			return  ( chipL < 0 );
		}
		void getDigitalZoomRoi_L0( Roi &chipRoi_L0 ) const{
			appAssert( chipL<0, "getDigitalZoomRoi_L0(): chipL must < 0!");
			const int L = -chipL;
			//corressponding roi size at L0 to be digitial
			const int w_L0 = chipW>>L;      
			const int h_L0 = chipH>>L;
			const int xc_L0 = chipX + w_L0/2 - 1;
			const int yc_L0 = chipY + h_L0/2 - 1;

			chipRoi_L0.setByIntCenter(xc_L0, yc_L0, w_L0, h_L0);
			chipRoi_L0.trimLocation( NGV_RGB_W0, NGV_RGB_H0 );
		}

		void read(Uint8Stream &is) {
			is.read(flags);
			is.read(rtspPort);
			is.read(clientIp);
			is.read(ngvMode);
			is.read(wamiMode);
			is.read(chipL);
			is.read(chipX);
			is.read(chipY);
			is.read(chipW);
			is.read(chipH);
			is.read(bkgImgW);
			is.read(bkgImgH);
			is.read(bkgPyrL);
			is.read(fps);
			is.read(lfcFrmRateIdx);
			is.read(turretImgSzIdx);
			is.read(turretZoomIdx);
			is.read(errCode);
			is.read(nClients);
			is.read(rsvd);
		}
		void write(Uint8Stream &os) {
			os.write(flags);
			os.write(rtspPort);
			os.write(clientIp);
			os.write(ngvMode);
			os.write(wamiMode);
			os.write(chipL);
			os.write(chipX);
			os.write(chipY);
			os.write(chipW);
			os.write(chipH);
			os.write(bkgImgW);
			os.write(bkgImgH);
			os.write(bkgPyrL);
			os.write(fps);
			os.write(lfcFrmRateIdx);
			os.write(turretImgSzIdx);
			os.write(turretZoomIdx);
			os.write(errCode);
			os.write(nClients);
			os.write(rsvd);
		}
		std::string toString() const {
			char buf[512];
#if 0			
			snprintf(buf, 512, "flags=%d,Rtsp(Port=%u,ip=%lu(%s)), ngvMode=%d\n wami: [mode=%d, chip(L=%d,x=%u,y=%u,w=%u,h=%u), bkgImg(w=%d,h=%d,L=%d),fps=%d]\n Lfc: [save=%d,download=%d,frmRateIdx=%d]\n Turret[imgSzIdx=%d, zoomIdx=%d, snapshot=%d]",
				flags, rtspPort, clientIp, ipConvertNum2Str(clientIp).c_str(),  ngvMode, wamiMode, chipL, chipX, chipY, chipW, chipH, bkgImgW, bkgImgH, bkgPyrL, fps, isLfcSave(), isLfcDownload(), lfcFrmRateIdx, turretImgSzIdx, turretZoomIdx, isTurretSnapShot());
#else
			snprintf(buf, 512, "flags=%d,Rtsp(Port=%u,ip=%lu(%s)), ngvMode=%d\n wami: [mode=%d, chip(L=%d,x=%u,y=%u,w=%u,h=%u), bkgImg(w=%d,h=%d,L=%d),fps=%d]\n",
				flags, rtspPort, clientIp, ipConvertNum2Str(clientIp).c_str(),  ngvMode, wamiMode, chipL, chipX, chipY, chipW, chipH, bkgImgW, bkgImgH, bkgPyrL, fps);
#endif				
			return std::string(buf);
		}

	public:
		uint16_t  flags;
		
		uint16_t  rtspPort;		//used for RTSP port,  camera(server) sends it back to client via msg 
								//to generate TCP url: "rtsp://10.10.10.100:8558/app"; where "10.10.10.100" is camera ip

		uint32_t  clientIp;		//used for client management

		uint8_t   ngvMode;        //WAMI, LFC, or TURRENT mode
		uint8_t   wamiMode;       //picture in picture or simple mode
		int16_t   chipL;           //chip pyramid level, [...,-2,-1,0,1,2,3,4, ...], chipL<0, means digital zoom from L=0 to factor of 2^|chipL|
		
		//regular case: chipL>=0, chip rect(x,y,W,H) at level <chipL>; 
		//digital zoom: chipL<0,  (chipW, chipH) is the destination size, (chipX,chipY) is Left top point ROI_L0 at L0 image, 
		//              where (ROI_L0.width, ROI_L0.height) = (chipW/k, chipH/k), where k = 2^( |chipL| )  
		uint16_t  chipX;          
		uint16_t  chipY;
		uint16_t  chipW;
		uint16_t  chipH;
		
		//(bkgImgW, bkgImgH):  for wami mode it is the  background imge w and h
		//                     for Turret mode it is the HDMI Img size
		uint16_t  bkgImgW;     //bkg img width and height
		uint16_t  bkgImgH;
		
		uint8_t   bkgPyrL;       //bkg img to disp or record
		uint8_t   fps;           //wami mode video frms/sec
		uint8_t   lfcFrmRateIdx; //lfc saving frame rate index
		uint8_t   turretImgSzIdx;

		uint8_t		turretZoomIdx;
		uint8_t		errCode;
		uint8_t		nClients;
		uint8_t		rsvd;        //at the conection period, it flags if the stablization is on (1)/off(0)
	};

	class MSG_EXPORT SettingsMsg : public Msg{
	public:
		SettingsMsg();
		SettingsMsg( const Settings &s );
		~SettingsMsg();
		SettingsMsg(const SettingsMsg &x);
		SettingsMsg& operator = (const SettingsMsg &x);
		bool operator == (const SettingsMsg &x);

		void setChipInfo( int x, int y, int w, int h);
		void setBkgImgSize(int w, int h);
		void setSaveInRemote(bool flag);

		//virtual methods 
		virtual void read( Uint8Stream &is);
		virtual void write( Uint8Stream &os);
		virtual std::string toString() const;
	public:
		Settings  settings;
	};
	typedef std::shared_ptr<Settings>		SettingsPtr;
	typedef std::shared_ptr<SettingsMsg>	SettingsMsgPtr;
}
#endif
