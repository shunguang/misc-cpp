/*
* AppSignal.h
* 
* It contains four template classes about the 1d signal: 
* class AppSignal{}		 - base class
* class BasebandSignal - a derived class (from AppSignal) for basband signal
	class CarrierSignal  - a derived class (from AppSignal) for carrier signal
	class MixedSignal    - a derived class (from AppSignal) for mixed signal
*
*/
#pragma once
#include "mixAlg/appUtil.h"
#include "mixAlg/MixCfg.h"

namespace mix {
	//A class to hold <numSamplePts> complex signal x(i) = xR(i) + j xI(i), i = 0, ...,  numSamplePts-1
	template <class T>
	class AppSignal {
	public:
		//constructor
		AppSignal(const double timeDuration_sec = 1, const double samplingFreq_Hz = 1, const bool isComplex = true, const uint64_t uid = 0, const std::string& name = "")
			: _timeDuration_sec(timeDuration_sec)
			, _numSamplePts(timeDuration_sec* samplingFreq_Hz)
			, _samplingFreq_Hz(samplingFreq_Hz)
			, _isComplex(isComplex)
			, _uid(uid)
			, _name(name)
		{
			_xI.resize(_numSamplePts);
			if (isComplex) {
				_xQ.resize(_numSamplePts);
			}
		}

		//copy constrcutor and assignment
		AppSignal(const AppSignal& x) = default;
		AppSignal& operator = (const AppSignal& x) = default;
		//todo: add move contructure

		//destructor 
		virtual ~AppSignal() = default;

		//set samples
		void normalization() {
			//find the maximum magnitude
			T tmp, maxMag = 0;
			if (_isComplex) {
				for (size_t i = 0; i < _numSamplePts; i++) {
					tmp = _xI[i] * _xI[i] + _xQ[i] * _xQ[i];
					if (tmp > maxMag) {
						maxMag = tmp;
					}
				}
			}
			else {
				for (const T& e : _xI) {
					tmp = e * e;
					if (tmp > maxMag) {
						maxMag = tmp;
					}
				}
			}

			// sqrt and check if it is a zeros vector
			maxMag = sqrt(maxMag);
			if (maxMag < std::numeric_limits<T>::epsilon()) {
				return;
			}
			
			//do normalization for both real and image part
			for (T& e : _xI) {
				e /= maxMag;
			}

			if (_isComplex) {
				for (T& e : _xQ) {
					e /= maxMag;
				}
			}
		}

		//read from txt files
		void readFromTxt(const std::string& txtFilePath) {
			std::ifstream ifs(txtFilePath, std::ifstream::in);

			readTxtHeader(ifs);
			readSignalFromTxt(ifs);
			ifs.close();
		}

		//read from binary files
		void readFromBinary(const std::string& txtFilePath) {
			//todo
		}

		//dump to txt for plot and/or analysis by 3rd party tools
		void dump2Txt(const std::string& txtFilePath) const {
			std::ofstream ofs(txtFilePath, std::ofstream::out);
			writeTxtHeader(ofs);
			writeSignalToTxt(ofs);
			ofs.close();
		}

		//dump to binary for efficency data exchange with 3rd party tools
		void dump2Binary(const std::string& txtFilePath) const {
			//todo
		}

	protected:
		//write txt file header
		virtual void writeTxtHeader(std::ofstream& ofs) const {
			ofs << "_name=" << _name << std::endl;
			ofs << "_uid=" << _uid << std::endl;
			ofs << "_timeDuration_sec=" << _timeDuration_sec << std::endl;
			ofs << "_numSamplePts=" << _numSamplePts << std::endl;
			ofs << "_samplingFreq_Hz=" << _samplingFreq_Hz << std::endl;
			ofs << "_isComplex=" << _isComplex << std::endl;
		}

		//write the real and imag of the signal
		void writeSignalToTxt(std::ofstream& ofs) const {
			if (_isComplex) {
				ofs << "# xI, xQ" << std::endl;
				for (int i = 0; i < _numSamplePts; ++i) {
					ofs << _xI[i] << "," << _xQ[i] << std::endl;
				}
			}
			else {
				ofs << "# xI " << std::endl;
				for (const T& e : _xI) {
					ofs << e << std::endl;
				}
			}
		}

		//Interface function with other tools such as matlab, python etc.
		virtual void readTxtHeader(std::ifstream& ifs) {
			//todo:
		}
		void readSignalFromTxt(std::ifstream& ifs) {
			//todo:
		}

	public:
		//for simplisity, we use std::vector to store the real and imag parts of the complex signal <x>
		//it can also be extented to 3rd party vecor or matrices in the future
		std::vector<T> _xI = {};						//the real part of the signal
		std::vector<T> _xQ = {};						//the imag part of the signal 

		//other propoerties of the signal such as length, sampling freq etc
		double		_timeDuration_sec = { 0 };
		uint64_t	_numSamplePts = { 0 };			//# of sample points, numSamplePts==xI.size()
		double		_samplingFreq_Hz = { 0 };	//sampling rate of <x> in Hz

		//if <_isComplex> is true, both <_xI> and <_xQ> are valid with same length, otherwise the signal is real and  0==_xQ.length()
		bool		_isComplex = { true };		//if it is a complex signal

		//for debug purpose
		uint64_t	 _uid = { 0 };               //the uid of this signal
		std::string  _name = { "" };
	};


	template <class T>
	class BasebandSignal : public AppSignal<T> {
	public:
		BasebandSignal(const MixCfg& cfg, const uint64_t uid = 0)
			: AppSignal<T>(cfg.basebandTimeDuration_sec, cfg.basebandSamplingFreq_Hz, cfg.basebandComplex, uid, "Baseband")
			, _freqRng_Hz(cfg.basebandFreqRng_Hz)
		{}


		BasebandSignal(const BasebandSignal& x) = default;
		BasebandSignal& operator = (const BasebandSignal& x) = default;
		//todo: add move contructure

		virtual ~BasebandSignal() override = default;

	protected:
		virtual void readTxtHeader(std::ifstream& ifs) override {
			//todo:
		}
		virtual void writeTxtHeader(std::ofstream& ofs) const override {
			AppSignal<T>::writeTxtHeader(ofs);
			ofs << _freqRng_Hz.toString("freqRng_Hz =") << std::endl;
		}

	protected:
		AppRng<double>	_freqRng_Hz = { 0,0 };
		//todo: add freq bands of the original baseband signal or model its freq behavior
		//std::vector < mix::AppRng > _vFreqBands;
	};

	template <class T>
	class CarrierSignal : public AppSignal<T> {
	public:
		CarrierSignal(const MixCfg& cfg, const uint64_t uid = 0, const std::string &name="Carrier")
			: AppSignal<T>(cfg.basebandTimeDuration_sec, cfg.carrierSamplingFreq_Hz, true, uid, name)
			, _carrierFreq_Hz(cfg.carrierFreq_Hz)
		{}

		CarrierSignal(const CarrierSignal& x) = default;
		CarrierSignal& operator = (const CarrierSignal& x) = default;
		//todo: add move contructure

		virtual ~CarrierSignal() override = default;

	protected:
		virtual void readTxtHeader(std::ifstream& ifs) override {
			//todo:
		}
		virtual void writeTxtHeader(std::ofstream& ofs) const override {
			AppSignal<T>::writeTxtHeader(ofs);
			ofs << "_carrierFreq_Hz =" << _carrierFreq_Hz << std::endl;
		}

	protected:
		double _carrierFreq_Hz = { 0f };
		//todo: add more about carrier signal
	};

	template <class T>
	class MixedSignal : public AppSignal<T> {
	public:
		MixedSignal(const MixCfg& cfg, const uint64_t uid = 0)
			: AppSignal<T>(cfg.basebandTimeDuration_sec, cfg.carrierSamplingFreq_Hz, true, uid, "mixed")
			, _cfg(cfg)
		{}

		MixedSignal(const MixedSignal& x) = default;
		MixedSignal& operator = (const MixedSignal& x) = default;

		virtual ~MixedSignal() override = default;

	protected:
		virtual void readTxtHeader(std::ifstream& ifs) override {
			//todo:
		}
		virtual void writeTxtHeader(std::ofstream& ofs) const override {
			AppSignal<T>::writeTxtHeader(ofs);
			ofs << _cfg.toString() << std::endl;
		}

	public:
		MixCfg _cfg;
	};
}
