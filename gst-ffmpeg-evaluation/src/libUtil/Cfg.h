#ifndef __CFG_H__
#define __CFG_H__

#include "DataTypes.h"

namespace ngv {
	enum GstVideoEncNameId{
		X264ENC=0,
		OMXH264ENC,
		NVH264ENC,
		NVV4L2H264ENC,		
		X265ENC,
		OMXH265ENC,
		NVH265ENC,
		NVV4L2H265ENC		
	};

	static char *g_gstEncName[]={"x264","omxh264enc", "nvh264enc","nvv4l2h264enc",		
		"x265","omxh265enc", "nvh265enc","nvv4l2h265enc"};

	class CfgBase {
	public:
		CfgBase();
		CfgBase(const CfgBase &x) = default;
		CfgBase& operator = (const CfgBase &x) = default;
		virtual ~CfgBase() = default;

		virtual boost::property_tree::ptree toPropertyTree() const = 0;
		virtual void fromPropertyTree(const boost::property_tree::ptree &pt) = 0;
		virtual std::string toString() const = 0;
		static std::string toString(const boost::property_tree::ptree &pt); 
	};
	typedef std::shared_ptr<CfgBase>		CfgBasePtr;

	class CfgEnc : public CfgBase
	{
	public:
		CfgEnc();
		CfgEnc(const CfgEnc &x)=default;
		CfgEnc &operator=(const CfgEnc &x)=default;
		virtual ~CfgEnc()=default;

		virtual boost::property_tree::ptree toPropertyTree() const;
		virtual void fromPropertyTree(const boost::property_tree::ptree &pt);
		virtual std::string toString() const;
        const char* getGstEncName() const {
			return g_gstEncName[(int)gstEncNameId];
		}
	public:
		int32_t imgW{1920};
		int32_t imgH{1080};
		//T0 = tbn/tbd(sec), fps = 1/T0 (Hz)
		int32_t tbn{1001};  //timebase numerorator
		int32_t tbd{30000}; //timebase denominator
		int32_t bitrate{10000000};

		GstVideoEncNameId  gstEncNameId{X264ENC};
		std::string ffmpegEncName{"mpeg4"};
		std::string inputKlvFile{"./klv.bin"};
		std::string inputYuvFile{"./vid.bin"};
		std::string ouputFilePath{"./out_muxEnc.ts"};
		bool useVidCapFilterAfterEncoder{false};
		uint32_t  nFrmsToPeekCpuTimeProfile{100};
	};
	typedef std::shared_ptr<CfgEnc> CfgEncPtr;


	class CfgDec : public CfgBase
	{
	public:
		CfgDec();
		CfgDec(const CfgDec &x) = default;
		CfgDec &operator=(const CfgDec &x) = default;
		virtual ~CfgDec() = default;

		virtual boost::property_tree::ptree toPropertyTree() const;
		virtual void fromPropertyTree(const boost::property_tree::ptree &pt);
		virtual std::string toString() const;

	public:
		int32_t imgW{1920};
		int32_t imgH{1080};
		uint32_t maxNumOfFrms{0xFFFFFFFF};
		uint32_t gstVidParser{264};
		std::string gstDecName{"omxh264dec"};
		std::string inputFilePath{"./Truck.ts"};
		std::string outputKlvFile{"./klv.bin"};
		std::string outputYuvFile{"./vid.bin"};
		uint32_t  nFrmsToPeekCpuTimeProfile{100};
	};
	typedef std::shared_ptr<CfgDec> CfgDecPtr;


	class  CfgApp {
	public:
		CfgApp();
		CfgApp(const CfgApp &x) = default;
		CfgApp& operator = (const CfgApp &x) = default;
		~CfgApp()=default;

		void readFromFile(const char *fname);
		void writeToFile(const char *fname);
		std::string toString();

	private:
		boost::property_tree::ptree toPropertyTree();
		void fromPropertyTree(const boost::property_tree::ptree &pt) ;

	public:
		CfgEncPtr       enc; 
		CfgDecPtr		dec; 
	};

	typedef std::shared_ptr<CfgApp> CfgAppPtr;

}

#endif




