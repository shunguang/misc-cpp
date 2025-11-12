#pragma once

#include "MixAlg.h"
using namespace std;
using namespace mix;
MixAlg::MixAlg(const MixCfg& cfg) 
: _cfg(cfg)
{
	//create shared poniters for signales
	_upperSampledBaseband = std::make_shared< CarrierSignal<APP_SIG_TYPE> >( cfg, 0, "upperSampledBaseband");
	_carrier = std::make_shared< CarrierSignal<APP_SIG_TYPE> >(cfg);
	_mixed = std::make_shared< MixedSignal<APP_SIG_TYPE> >(cfg);

	//copy kernal from cfg into <this->_firKernel>
	_firKernel.clear();
	for (const double& e : cfg.firKernel) {
		_firKernel.push_back((APP_SIG_TYPE)e);
	}

	//allocate sizes for class temporal vectors
	_vTmp1.resize(cfg.carrierSamplePts);
	_vTmp2.resize(cfg.carrierSamplePts);

	//check the consistance of vetcor sizes
	appAssert( validation(), "MixAlg::MixAlg(): conflict of vector sizes!");
}

MixAlg::~MixAlg() 
{
}

bool MixAlg::mix(const BasebandSignalPtr& x)
{
	//dump input baseband signal if needs
	if (_cfg.isDump) {
		x->dump2Txt("c:/temp/base.txt");
	}

	//go either time- ot freq- domain mixing algorithms
	if (_cfg.isTimeDomainSln){
		//upsampling baseband <x> into <_upperSampledBaseband>
		upSamplingBasebandSignale(x);

		//multiply <<_upperSampledBaseband> with <_carrier>, and the results are in <_mixed>
		signalMultiplication();

		if (_cfg.isDump) {
			_upperSampledBaseband->dump2Txt("c:/temp/upSampledBaseband.txt");
			_mixed->dump2Txt("c:/temp/mixed.txt");
		}
	}
	else {
		//freq. domain solution
		//todo
		return false;
	}
	return true;
}

//upsampling baseband <x> into <_upperSampledBaseband>
void MixAlg::upSamplingBasebandSignale(const BasebandSignalPtr& x)
{
	//upsampling <x> into <_upperSampledBaseband>
	size_t L = _cfg.getExpansionZerosLength();
	appInsertZeros<APP_SIG_TYPE>(_vTmp1, x->_xI, L);
	appFir<APP_SIG_TYPE>( _upperSampledBaseband->_xI, _vTmp1, _firKernel );
	
	if (x->_isComplex) {
		appInsertZeros< APP_SIG_TYPE>(_vTmp1, x->_xQ, L);
		appFir<APP_SIG_TYPE>(_upperSampledBaseband->_xQ, _vTmp1, _firKernel );
	}
}

//multiply <<_upperSampledBaseband> with <_carrier>, and the results are in <_mixed>
//exp(j*wc*t) = I1 + jQ1
//exp(j*ws*t) = I2 + jQ2
//exp(j*wc*t) * exp(j*ws*t) =  exp(j(wc+ws)*t) = (I1*I2 - Q1*Q2) + j(I2*Q1+I1*Q2)
void MixAlg::signalMultiplication()
{
	size_t i;

	const vector<APP_SIG_TYPE>& I1 = _carrier->_xI;
	const vector<APP_SIG_TYPE>& Q1 = _carrier->_xQ;
	const vector<APP_SIG_TYPE>& I2 = _upperSampledBaseband->_xI;
	const vector<APP_SIG_TYPE>& Q2 = _upperSampledBaseband->_xQ;

	if (_upperSampledBaseband->_isComplex) {
		//mixed->_xQ = I1* I2 - Q1 * Q2
		vecMul<APP_SIG_TYPE>(_vTmp1,  I1, I2);
		vecMul<APP_SIG_TYPE>(_vTmp2, Q1, Q2);
		vecSub<APP_SIG_TYPE>(_mixed->_xI, _vTmp1, _vTmp2);

		//mixed->_xQ = I2* Q1 + I1 * Q2
		vecMul<APP_SIG_TYPE>(_vTmp1, I2, Q1);
		vecMul<APP_SIG_TYPE>(_vTmp2, I1, Q2);
		vecAdd<APP_SIG_TYPE>(_mixed->_xQ, _vTmp1, _vTmp2);
	}
	else {
		vecMul<APP_SIG_TYPE>(_mixed->_xI, I1, I2);
	}
}

void MixAlg::setCarrierSignal(const CarrierSignalPtr& carrier)
{
	//hard copy
	*_carrier.get() = *carrier.get();
	//_carrier->dump2Txt("c:/temp/carrier2.txt");
}



//check the consistence of the sizes of the arrays/vectors preallocated in constructor
bool MixAlg::validation()
{
	//todo
	return true;
}


