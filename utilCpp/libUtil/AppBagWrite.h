/*
 *------------------------------------------------------------------------
 * abc.h
 *
 * No  any Corporation or client funds were used to develop this code.
 * But the numerical receip's LU decomposition algorithm is adopted.
 *
 * Copyright (c) 2022 Shunguang Wu
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *-------------------------------------------------------------------------
 */

#ifndef __APP_BAG_WRITE_H__
#define __APP_BAG_WRITE_H__

#include "AppDefs.h"
#include "AppLog.h"
#include "UtilFuncs.h"
#include "HealthyMonitor.h"
namespace app {
	/*
	*****************************************
	*  class AppBagWrite
	*****************************************
	*/
	class AppBagWrite {
	public:
		//constructor for write into a file by checking  file size, and automatic create multiple files
		AppBagWrite(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB = 1, const bool isBinary = true);

		//constructor for write into a file without checking file size, only create one file! 
		AppBagWrite(const std::string& filepath, const bool isBinary);

		AppBagWrite(const AppBagWrite& x) = delete;
		AppBagWrite& operator = (const AppBagWrite& x) = delete;
		~AppBagWrite();

		template <class T>
		void writeMsg(const T& msg) {}

		std::string getBagFilePath() const { return _bagFilePath; }

		bool close();
	protected:
		bool open();

		inline bool isBagBigEnough() const {
			return (_currFileSize >= _maxBagFileSize_B);
		}
		void wrtMsgWrapUp(const APP_TIME_MS tBeg);

	protected:
		std::string				_bagDir{ "" };
		const std::string _bagFilepathBaseName{ "" };
		const uint64_t		_maxBagFileSize_B{ (uint64_t)(2 * APP_DISK_GB - 1 * APP_DISK_MB) };   //todo: move to config, FAT16 2048MB 
		const uint64_t    _minFreeSpace_B{ (uint64_t)APP_DISK_GB };   //leav 1G in disk

		uint32_t		_bagCnt{ 0 };				//count the # of bag files, start from 0
		uint64_t    _currFileSize{ 0 }; //count current bag file sz in byte

		std::ofstream    _ofs{};

		uint64_t    _msgCnt{ 0 };    //the block count for all serial of bag files, strat from 0
		std::string _bagFilePath{""};
		bool				_isBinary{ true };
		HealthyMonitor  _hmWrtBagCupTime_ms{};
		bool						_checkFileSize{ false };
		bool						_isSuccefullyOpen{ false };
	};
	typedef std::shared_ptr<AppBagWrite>  AppBagWritePtr;

	/*
	*****************************************
	*  class AppBagWriteTxt
	*****************************************
	*/
	class AppBagWriteTxt : public AppBagWrite {
	public:
		//constructor for write into a file by checking  file size, and automatic create multiple files
		AppBagWriteTxt(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB = 1);
		//constructor for write into a file without checking file size, only create one file! 
		AppBagWriteTxt(const std::string& filepath);

		AppBagWriteTxt(const AppBagWriteTxt& x) = delete;
		AppBagWriteTxt& operator = (const AppBagWriteTxt& x) = delete;
		~AppBagWriteTxt();

		void write(const	std::string& s, const bool isAddNewLine=true);
		void write(const char* fmt, ...);

		template <class T>
		void writeMsg(const T& msg) {
			std::string s = msg.toString();
			write(s);
		}
	};
	typedef std::shared_ptr<AppBagWriteTxt>  AppBagWriteTxtPtr;

	/*
	*****************************************
	*  class AppBagWriteBin
	*****************************************
	*/
	class AppBagWriteBin : public AppBagWrite {
	public:
		//constructor for write into a file by checking  file size, and automatic create multiple files
		AppBagWriteBin(const std::string& dir, const std::string& tag, const int maxBagFileSize_GB, const int minFreeSpace_GB = 1);
		//constructor for write into a file without checking file size, only create one file! 
		AppBagWriteBin(const std::string& filepath);

		AppBagWriteBin(const AppBagWriteBin& x) = delete;
		AppBagWriteBin& operator = (const AppBagWriteBin& x) = delete;
		~AppBagWriteBin();

		template <class T>
		void writeMsg(const T& msg) {
		}
	};
	typedef std::shared_ptr<AppBagWriteBin>  AppBagWriteBinPtr;
}
#endif

