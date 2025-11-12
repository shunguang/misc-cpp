/*
* test.cpp
*
* test driver for mixing algorithm, we try to apply Test-Driven-Development (TDD) process 
*
*/
#include "mixAlg/appUtil.h"
#include "mixAlg/AppSignal.h"
#include "mixAlg/MixAlg.h"

void testUtil(const mix::MixCfg& cfg);
void testAppSignal(const mix::MixCfg& cfg);
void testMixAlg(const mix::MixCfg& cfg);
void generateBaseband(BasebandSignalPtr& x, const mix::MixCfg& cfg, const int nTones);
void generateCarrier(CarrierSignalPtr &y, const mix::MixCfg& cfg);

using namespace std;
using namespace mix;
int main(const int argc, const char* argv []) {

	MixCfg cfg(
		1.0,						/* double basebandTimeDuration_sec_*/
		100,						/* basebandSamplingFreq_Hz_*/
		AppRng<double>(10,50),	/* AppRng<double>&basebandFreqRng_Hz_ = AppRng<double>(10, 50) */
		true,						/* bool basebandComplex_ = true*/
		1000,						/* double carrierFreq_Hz_ = 1000000 */
		AppRng<double>(500, 2000)	/*AppRng<double> &carrierFreqRng_Hz_*/
		);
	cfg.isDump = false;

	//testUtil(argc, argv);
	//testAppSignal( cfg );
	testMixAlg( cfg );
}

void testMixAlg(const MixCfg& cfg)
{
		//create shared pointer objects from <cfg>
		MixAlgPtr mix = std::make_shared<MixAlg>(cfg);
		BasebandSignalPtr bs = std::make_shared< BasebandSignal<APP_SIG_TYPE> >(cfg);
		CarrierSignalPtr cs = std::make_shared< CarrierSignal<APP_SIG_TYPE> >(cfg);

		//generate baseband and carrier signals
		int nTones = 3;
		generateBaseband(bs, cfg, nTones);
		generateCarrier(cs, cfg);

		bs->dump2Txt("c:/temp/baseband.txt");
		cs->dump2Txt("c:/temp/carrier.txt");

		//set carrier signal for mixing algorith
		mix->setCarrierSignal(cs);

		//run a few loops to test mix() 
		for (int i = 0; i < 1; ++i) {
			bs->_uid = i;				//set different <_uid> for debugging purpose
			mix->mix( bs );			//call mix()

			//dump the mixed signal into text, then use 3rd party tools such as matlab to plot it
			const MixedSignal<APP_SIG_TYPE>& ms = mix->getMixedSignal();
			std::string ff = "c:/temp/mixed_" + std::to_string(i) + ".txt";
			ms.dump2Txt(ff);
		}

}

//Generate baseband and carrier signals from <cfg>, and dump them into a txt files for matlab analysis and visulization
void testAppSignal(const MixCfg& cfg)
{
	BasebandSignalPtr x = std::make_shared< BasebandSignal<APP_SIG_TYPE> >(cfg);
	CarrierSignalPtr y = std::make_shared< CarrierSignal<APP_SIG_TYPE> >(cfg);

	generateBaseband(x, cfg, 3);
	
	generateCarrier(y, cfg);

	x->dump2Txt("c:/temp/baseband1.txt");
	y->dump2Txt("c:/temp/carrier1.txt");
}

void testUtil(const MixCfg& cfg) {

	AppRng<double>  r1(0.1, 1.0, "Hz");
	AppRng<double>  r2(100, 1000, "MHz");

	bool c1 = r1.isInside(0.5);
	bool c2 = r1.isInside(0);
	bool c3 = r1.isInside(0.1-0.001);
	bool c4 = r1.isInside(1.0+0.001);

	cout << r1.toString("DoubleRng=") << endl;


	c1 = r2.isInside(100);
	c2 = r2.isInside(1000);
	cout << r2.toString("IntRng=") << endl;
	
	cout << cfg.toString() << endl;
}


void generateBaseband(BasebandSignalPtr& x, const mix::MixCfg& cfg, const int nTones)
{
	std::vector<double> f = { 
		cfg.basebandFreqRng_Hz.left,
		0.5 * (cfg.basebandFreqRng_Hz.left + cfg.basebandFreqRng_Hz.right),
		cfg.basebandFreqRng_Hz.right 
	};
	std::vector<double> phi = {0, M_PI/6, M_PI/3};

	int m = f.size();  //# of freq  components in baseband
	std::vector<double> w(m);
	for (int i = 0; i < m; ++i) {
		w[i] = 2 * M_PI * f[i];
	}

	APP_SIG_TYPE eI;    //real element
	APP_SIG_TYPE eQ;    //imag element
	APP_SIG_TYPE T = 1.0 /cfg.basebandSamplingFreq_Hz;      //sampling intreval
	APP_SIG_TYPE t = 0;

	int actualTones = min(3, nTones);
	const uint64_t& n = cfg.basebandSamplePts;
	for (uint64_t i = 0; i < n; ++i) {
		eI = 0;
		eQ = 0;
		for (int j = 0; j < m; j++) {
			eI += cos(w[j] * t + phi[j] );
			eQ += sin(w[j] * t + phi[j] );
		}

		x->_xI[i] = eI;
		x->_xQ[i] = eQ;

		//for next iteration
		t += T; //todo: if T is too small
	}

	x->normalization();
}

void generateCarrier(CarrierSignalPtr& y, const mix::MixCfg& cfg)
{
	const APP_SIG_TYPE phi = M_PI / 6;
	const APP_SIG_TYPE w = 2*M_PI*cfg.carrierFreq_Hz;         //2*pi*f
	const APP_SIG_TYPE T = 1.0 / cfg.carrierSamplingFreq_Hz;  //sampling time interval
	const uint64_t& n = cfg.carrierSamplePts;
	APP_SIG_TYPE t = 0;
	for (uint64_t i = 0; i < n; ++i) {
		y->_xI[i] = cos(w * t + phi);
		y->_xQ[i] = sin(w * t + phi);

		//for next iteration
		t += T; //todo: if T is too small
  }
	y->normalization();
}
