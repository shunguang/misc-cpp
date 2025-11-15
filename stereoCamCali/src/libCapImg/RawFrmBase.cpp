#include "RawFrmBase.h"

using namespace std;
using namespace cali;

RawFrmBase::RawFrmBase()
	: epoch_ms (0)
	, fn(0)
	, sensorType(SENSOR_T_UNKN)
{
}

RawFrmBase::RawFrmBase(const RawFrmBase &x)
	: epoch_ms(x.epoch_ms)
	, fn(x.fn)
	, sensorType(x.sensorType)
{
}

RawFrmBase::~RawFrmBase()
{
}

//hard copy
RawFrmBase& RawFrmBase::operator = (const RawFrmBase &x)
{
	if (this != &x) {
		epoch_ms	= x.epoch_ms;
		fn = x.fn;
		sensorType	= x.sensorType;
	}
	return *this;
}
