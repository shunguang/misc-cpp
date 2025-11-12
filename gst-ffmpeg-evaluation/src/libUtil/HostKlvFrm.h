#ifndef __HOST_KLV_FRM_H__
#define __HOST_KLV_FRM_H__

#include "DataTypes.h"
#include "AppLog.h"
#include "Uint8Stream.h"

namespace ngv
{
    class HostKlvFrm
    {
    public:
        HostKlvFrm( uint32_t fn=0, uint16_t sz=0, uint16_t cap=512);
        HostKlvFrm(const HostKlvFrm &x) = delete;
        HostKlvFrm &operator=(const HostKlvFrm &x) = delete;

        ~HostKlvFrm();
        void setToZeros();

        void readFromBuf( const uint8_t *buf, const uint16_t bufLength);
        bool readFromBinFile(FILE *fp);
        std::string toString() const;

		void deSerial( Uint8Stream &is );
		void enSerial( Uint8Stream &os ) const;

    private:
        void resize(uint16_t newcap_);
        void creatBuf();
        void deleteBuf();

    public:
        uint32_t fn_{0};
        uint16_t sz_{0};
        uint16_t cap_{0}; //capacity of buf[]
        uint8_t *buf_{nullptr};
    };

    typedef std::shared_ptr<HostKlvFrm> HostKlvFrmPtr;
}
#endif
