#include "MuxEncUserData.h"
using namespace ngv;
MuxEncUserData::MuxEncUserData(const CfgEncPtr &cfg)
    : m_cfg(cfg), m_hostYuvFrm(new ngv::HostYuvFrm(cfg->imgW, cfg->imgH, 0)), m_hostKlvFrm(new ngv::HostKlvFrm(0, 0, 512)), m_hostKlvFrmDefault(new ngv::HostKlvFrm(0, 4, 4))
{
  dumpLog("MuxEncUserData()---AA");
  m_timeDuration_nanoSec = gst_util_uint64_scale_int(m_cfg->tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, m_cfg->tbd /*25*/);
  m_timeDurationKlv_nanoSec = gst_util_uint64_scale_int(m_cfg->tbn /*1*/, GST_SECOND /*1sec = 1e+9 nano sec*/, m_cfg->tbd /*25*/);
  dumpLog("m_timeDurationKlv_nanoSec=%ld", m_timeDurationKlv_nanoSec);

  //open data file
  m_fpYuv = fopen(m_cfg->inputYuvFile.c_str(), "rb");
  if (!m_fpYuv)
  {
    appExit("appQuit: cannot open file: %s", m_cfg->inputYuvFile.c_str());
  }

  m_fpKlv = fopen(m_cfg->inputKlvFile.c_str(), "rb");
  if (!m_fpKlv)
  {
    appExit("appQuit: cannot open file: %s", m_cfg->inputKlvFile.c_str());
  }

  //prepare KVLV, if the 1 video frm has KLV we use <m_hostKlvFrm>, otherwise <m_hostKlvFrmDefault> will be used as facked metadata
  m_hostKlvFrmDefault->setToZeros();
  m_hasKlv = m_hostKlvFrm->readFromBinFile(m_fpKlv);
  m_cpuProf.setThdCntToPrintOut(cfg->nFrmsToPeekCpuTimeProfile);

  dumpLog("MuxEncUserData()---BB");
}


MuxEncUserData::~MuxEncUserData()
{
  if (m_fpYuv)
    fclose(m_fpYuv);
  if (m_fpKlv)
    fclose(m_fpKlv);
}

bool MuxEncUserData::readNextYuvFrm()
{
  bool suc = m_hostYuvFrm->readFromBinFile(m_fpYuv, m_imgCnt);
  m_imgCnt++;
  return suc;
}

uint16_t MuxEncUserData::readNextKlvFrm(std::vector<uint8_t> &v)
{
  Uint8Stream os;

  if (m_hasKlv && m_hostKlvFrm->fn_ == m_klvCnt)
  {
    m_hostKlvFrm->enSerial(os);
    os.getData(v);
    //std::cout << "used metadat:" << m_hostKlvFrm->toString() << std::endl;

    //update <m_hosKlvFrm>
    m_hasKlv = m_hostKlvFrm->readFromBinFile(m_fpKlv);
  }
  else
  {
    m_hostKlvFrmDefault->fn_ = m_klvCnt;
    m_hostKlvFrmDefault->enSerial(os);
    os.getData(v);
    //std::cout << "used metadat:" << m_hostKlvFrmDefault->toString() << std::endl;
  }

  m_klvCnt++;
  return v.size();
}

