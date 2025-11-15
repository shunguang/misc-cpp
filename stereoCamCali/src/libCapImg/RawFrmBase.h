#ifndef _RAW_FRM_BASE_H_
#define _RAW_FRM_BASE_H_

#include "libutil/util.h"
#include "CapDefs.h"
namespace cali {
	class CAP_EXPORT RawFrmBase {
	public:
		RawFrmBase();
		RawFrmBase(const RawFrmBase &x );
		virtual ~RawFrmBase();
		virtual RawFrmBase& operator =(const RawFrmBase &x);
	public:
		uint64_t  	epoch_ms;
		uint32_t  	fn;
		SENSOR_T	sensorType;
	};

	typedef std::shared_ptr<RawFrmBase>		RawFrmBasePtr;
}

#endif
