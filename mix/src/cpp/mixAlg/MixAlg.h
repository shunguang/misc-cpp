/*
* MixAlg.h
*  mixing baseband signal with carrier signal from both time-domain and freq-domain algoriths
*  todo: implement freq. domain algorithms
*
*/

#pragma once

#include "mixAlg/appUtil.h"
#include "mixAlg/MixCfg.h"
#include "mixAlg/AppSignal.h"

#define APP_SIG_TYPE float

typedef std::shared_ptr<mix::BasebandSignal<APP_SIG_TYPE>> BasebandSignalPtr;
typedef std::shared_ptr<mix::CarrierSignal<APP_SIG_TYPE>> CarrierSignalPtr;
typedef std::shared_ptr<mix::MixedSignal<APP_SIG_TYPE>> MixedSignalPtr;
namespace mix {

	class MixAlg {
	public:
		MixAlg(const MixCfg& cfg);
		MixAlg(const MixAlg& rhs) = delete;
		MixAlg& operator = (const MixAlg& rhs) = delete;

		~MixAlg();

		//generate <_carrier> based on <_cfg>
		void setCarrierSignal( const CarrierSignalPtr &carrier );
		
		//do mix operation
		bool mix(const BasebandSignalPtr &x );

		//get the results
		const MixedSignal<APP_SIG_TYPE>& getMixedSignal() const {
			return *(_mixed.get());
		}
		
	protected:
		//upper sampling <x>, and the result are in in <this->_upperSampledBaseband>
		void upSamplingBasebandSignale(const BasebandSignalPtr& x);

		// multiple <this->_upperSampledBaseband> with <this->_carrier>, the result will be in <this->_mixed>
		void signalMultiplication();

		//check if sizes of related vectors have any conflicts
		bool validation();

	protected:
		CarrierSignalPtr		_upperSampledBaseband={nullptr};
		CarrierSignalPtr    _carrier = { nullptr };
		MixedSignalPtr			_mixed = {nullptr};
		MixCfg							_cfg = {};


		//temp variables
		std::vector<APP_SIG_TYPE>  _vTmp1;
		std::vector<APP_SIG_TYPE>  _vTmp2;
		std::vector<APP_SIG_TYPE>  _firKernel;

	};
	typedef std::shared_ptr<mix::MixAlg> MixAlgPtr;
}
