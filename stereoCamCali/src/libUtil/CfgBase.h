#ifndef __CFG_BASE_H__
#define __CFG_BASE_H__

#include "DataTypes.h"
#include "AppEnums.h"
#include "AppLog.h"
#include "UtilFuncs.h"
namespace cali {
	class CfgBase {

	public:
		CfgBase();
		CfgBase(const CfgBase& x) = default;
		CfgBase& operator = (const CfgBase& x) = default;
		virtual ~CfgBase() = default;

		virtual boost::property_tree::ptree toPropertyTree() const = 0;
		virtual void fromPropertyTree(const boost::property_tree::ptree& pt) = 0;

		virtual std::string toString() const;
		static std::string  toString(const boost::property_tree::ptree& pt);
		void dumpToLog(const std::string& msg = "");
	};
	typedef std::shared_ptr<CfgBase>		CfgBasePtr;
}

#endif




