/*
* MixCfg.h
*
* configuration data structure of the mixing app
*
*/

#pragma once
#include "mixAlg/appUtil.h"
namespace mix {
	//cfguration for the Mix algorithm
	struct MixCfg {
		MixCfg(
			const double basebandTimeDuration_sec_=1, 
			const double  basebandSamplingFreq_Hz_=100, 
			const AppRng<double>& basebandFreqRng_Hz_ = AppRng<double>(10, 100),
			const bool basebandComplex_=true,
			const double carrierFreq_Hz_=1000,
			const AppRng<double> &carrierFreqRng_Hz_= AppRng<double>(500,2000) 
		) 
			: basebandTimeDuration_sec(basebandTimeDuration_sec_)
			, basebandSamplingFreq_Hz(basebandSamplingFreq_Hz_)
			, basebandSamplePts( ceil(basebandSamplingFreq_Hz_ * basebandTimeDuration_sec_) )
			, basebandFreqRng_Hz(basebandFreqRng_Hz_)
			, basebandComplex(basebandComplex_)
			, carrierFreq_Hz(carrierFreq_Hz_)
			, carrierSamplingFreq_Hz(2* carrierFreq_Hz_)
			, carrierSamplePts( 2*carrierFreq_Hz_ * basebandTimeDuration_sec_)
			, carrierFreqRng_Hz(carrierFreqRng_Hz_)
		{
		}

		int getExpansionZerosLength() {
			//todo: only works for  carrierSamplePts/ basebandSamplePts is an integer
			int L = (carrierSamplePts/ basebandSamplePts);
			return L;
		}

		double					basebandTimeDuration_sec{ 1 };
		double					basebandSamplingFreq_Hz={100};						//baseband signal sampling freqency in Hz
		uint64_t				basebandSamplePts={100};								//number of sampling points for baseband signal
		AppRng<double>	basebandFreqRng_Hz = { 10, 100, "Hz" };	        //the baseband freq range of desinged mixing algorithm
		bool            basebandComplex = { true };

		//we assume the carrier is a single tone
		double				carrierFreq_Hz = { 1000 };								//caaerier freqency in Hz
		double				carrierSamplingFreq_Hz = { 2000 };                     //carrier sampling freq
		uint64_t			carrierSamplePts = { 2000 };							//number of sampling points for baseband signal
		AppRng<double>	carrierFreqRng_Hz = { 500, 2000, "Hz" };      //the carrier freq range of desinged mixing algorithm

		bool				isTimeDomainSln = { true };
		bool				isDump = { false };

		//a fixed 31-pt gaussian kernel for fir
		//todo: need to improve
		std::vector<double> firKernel = { 0.0439,0.0657,0.0956,0.1353,0.1863,0.2494,0.3247,0.4111,0.5063,0.6065,0.7066,0.8007,0.8825,
			0.9460,0.9862,1.0000,0.9862,0.9460,0.8825,0.8007,0.7066,0.6065,0.5063,0.4111,0.3247,0.2494,0.1863,0.1353,0.0956,0.0657,0.0439 };


		std::string toString() const {
			std::ostringstream ss;

			ss << "basebandTimeDuration_sec=" << basebandTimeDuration_sec << std::endl
				<< "basebandSamplingFreq_Hz=" << basebandSamplingFreq_Hz << std::endl
				<< "basebandSamplePts=" << basebandSamplePts << std::endl
				<< basebandFreqRng_Hz.toString("basebandFreqRng_Hz=") << std::endl
				<< "carrierFreq_Hz=" << carrierFreq_Hz << std::endl
				<< "carrierSamplePts=" << carrierSamplePts << std::endl
				<< carrierFreqRng_Hz.toString("carrierFreqRng_Hz=") << std::endl
				<< "isTimeDomainSln=" << isTimeDomainSln << std::endl;

			return ss.str();
		}
	};
}
