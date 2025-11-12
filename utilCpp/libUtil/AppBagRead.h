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

#ifndef __APP_BAG_READ_H__
#define __APP_BAG_READ_H__

#include "AppDefs.h"
#include "AppLog.h"
#include "UtilFuncs.h"
#include "HealthyMonitor.h"
namespace app {
	/*
	*****************************************
	*  class AppBagRead
	*****************************************
	*/
	class AppBagRead {
	public:
		AppBagRead(const std::string& bagFilePath, const bool isBinary);
		virtual ~AppBagRead();

		//return true -- if there has a line in file, 
		//       false --  otherwise or eof
		virtual bool readLine(std::string& line) = 0;
		virtual uint64_t readRawAll(std::vector<uint8_t>& msg) = 0;
		virtual uint64_t readRawSome(std::vector<uint8_t>& msg, const uint64_t  nBytes ) = 0;

		void close();

	protected:
		bool open();

	protected:
		const bool					_isBinary{ false };
		const std::string		_bagFilePath{};
		std::ifstream				_ifs{};
		bool								_isOpened{ false };
		uint64_t						_nTotBytes{ 0 };
	};

	/*
	*****************************************
	*  class AppBagReadTxt
	*****************************************
	*/

	class AppBagReadTxt : public AppBagRead {
	public:
		AppBagReadTxt(const std::string& bagFilePath);
		virtual ~AppBagReadTxt();

		virtual uint64_t readRawAll(std::vector<uint8_t>& msg);
		virtual uint64_t readRawSome(std::vector<uint8_t>& msg, const uint64_t  nBytes);

		//return true -- if there has a line in file, 
		//       false --  otherwise or eof
		virtual bool readLine(std::string& line);
	};

	/*
	*****************************************
	*  class AppBagReadBin
	*****************************************
	*/
	class AppBagReadBin : public AppBagRead {
	public:
		AppBagReadBin(const std::string& bagFilePath);
		virtual ~AppBagReadBin();

		//for binary bag, we read the raw message, the caller will parse it latter 
		virtual uint64_t readRawAll(std::vector<uint8_t>& msg);
		virtual uint64_t readRawSome(std::vector<uint8_t>& msg, const uint64_t  nBytes);
		virtual bool readLine(std::string& line);
	};
}
#endif

