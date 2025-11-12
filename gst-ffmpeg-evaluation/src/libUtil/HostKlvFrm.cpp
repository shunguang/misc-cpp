#include "HostKlvFrm.h"

using namespace ngv;

HostKlvFrm::HostKlvFrm(uint32_t fn, uint16_t sz, uint16_t cap)
    : fn_(fn), sz_(sz), cap_(cap)
{
    creatBuf();
}


HostKlvFrm::~HostKlvFrm()
{
    deleteBuf();
}

void HostKlvFrm::setToZeros()
{
    memset(buf_, 0, sz_);
}

void HostKlvFrm::readFromBuf( const uint8_t *buf, const uint16_t bufLength)
{  
    //printf("bufLength=%u\n", bufLength);
    Uint8Stream os;
    os.write(buf, bufLength, false);
    deSerial(os);
}


bool HostKlvFrm::readFromBinFile(FILE *fp)
{
    if (feof(fp))
    {
        return false;
    }

#if 0
    uint8_t b4[4];
    //read four bytes for fn_;
    fread(b4, 1, 4, fp);
    fn_ = *((uint32_t *)b4);

    //read two bytes for sz_
    fread(b4, 1, 2, fp);
    sz_ = *((uint16_t *)b4);
#else
    int i;
    uint8_t r;
    fn_ = 0;
    for (i = 0; i < 4; ++i) {
        fread(&r, 1, 1, fp);
        fn_ |= ((uint32_t)r << (24 - (i * 8)));
    }

    sz_ = 0;
    for (i = 0; i < 2; ++i) {
        fread(&r, 1, 1, fp);
        sz_ |= ((uint16_t)r << (8 - (i * 8)));
    }
#endif

    //check if need to resize buffer
    if (sz_ > cap_)
    {
        resize(2 * sz_);
    }

    //read data into buf_
    fread(buf_, 1, sz_, fp);

    //printf("read klv frame n:%u, size:%u\n", fn_, sz_);
    return true;
}

std::string HostKlvFrm::toString() const
{
    std::ostringstream ss;
    ss << "fn=" << fn_ << ",sz=" << sz_ << ",buf=[";
    if( sz_ > 0 ){
        for (int i = 0; i < sz_ - 1; ++i)
        {
            ss << (int)buf_[i] << ",";
        }
        ss << (int)buf_[sz_ - 1];
    }
    ss << "]";
    return ss.str();
}

void HostKlvFrm::resize(uint16_t newcap_)
{
    if (newcap_ > cap_)
    {
        deleteBuf();
        cap_ = newcap_;
        creatBuf();
    }
}

void HostKlvFrm::creatBuf()
{
    if (cap_ > 0)
    {
        buf_ = new uint8_t[cap_];
    }
}

void HostKlvFrm::deleteBuf()
{
    delete[] buf_;
    buf_ = NULL;
}


//read msg from stream <is>
void HostKlvFrm::deSerial( Uint8Stream &is )
{
	//----------------------------------------------
	//read message header
	//----------------------------------------------
	is.read(fn_);
	is.read(sz_);

    //printf("is.size()=%u, is.capacity()=%u, fn_=%lu, sz_=%d, cap_=%d\n", is.size(), is.capacity(), fn_, sz_, cap_);
    if( sz_ > cap_){
        resize( 2*sz_);
    }
	is.read(buf_, sz_);				//class member
}

//write a msg into stream for sending
void HostKlvFrm::enSerial( Uint8Stream &os ) const
{
	os.resetWrite();
	//----------------------------------------------
	//write message header
	//----------------------------------------------
	os.write(fn_, false);
	os.write(sz_, false);
	os.write(buf_, sz_, false);
}
