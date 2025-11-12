#ifndef _CIRCULAR_Q_H_
#define _CIRCULAR_Q_H_

#include "libUtil/AppDefs.h"
#include "libUtil/AppLog.h"
namespace app
{
    template <class T>
    class CircularQ
    {
    public:
        // assignment constructor
        CircularQ(const uint32_t nTotItems, const std::string &name)
            : m_v(), m_q(), m_name(name), m_wrtDropCnt(0)
        {
            allocQ(nTotItems);
        }

        // destructor
        ~CircularQ()
        {
            freeQ();
        }

        void allocQ(const uint32_t nTotItems)
        {
            boost::mutex::scoped_lock lock(m_mutexRW);
            
            m_items = nTotItems;
            m_q.clear();
            for (uint32_t i = 0; i < m_items; i++)
            {
                T p;
                m_q.push_back(p);
            }
            m_v.resize(m_items, 0);
            m_headW = 0;
	        m_headR = 0;
        }

        void freeQ()
        {
            boost::mutex::scoped_lock lock(m_mutexRW);
            m_q.clear();
            m_v.clear();
            m_headR = 0;
            m_headW = 0;
        }

        void resetName(const std::string &name)
        {
            boost::mutex::scoped_lock lock(m_mutexRW);	            
            m_name = name;
        }
        void resetSize(const uint32_t nTotItems)
        {
            freeQ();
            allocQ(nTotItems);
        }

        bool wrt(const T *src)
        {
            bool sucWrt = false;
            {
                boost::mutex::scoped_lock lock(m_mutexRW);                
                uint32_t &idx = m_headW;
                int &cnt = m_v[idx];
                if (cnt == 0)
                {
                    m_q[idx] = *src;
                    cnt++;

                    //move head to the next slot
                    ++idx;
                    if (idx >= m_items)
                    {
                        idx = 0;
                    }
                    sucWrt = true;
                }
            }

            if (!sucWrt)
            {
                m_wrtDropCnt++;
                if (m_wrtDropCnt > 999)
                {
                    dumpLog("CircularQ::wrt(): writen is too fast, %d frames droped--%s", m_wrtDropCnt, m_name.c_str() );
                    m_wrtDropCnt = 0;
                }
            }
            return sucWrt;
        }

        bool read(T *dst)
        {
            bool hasData = false;
            {
                boost::mutex::scoped_lock lock(m_mutexRW);                
                uint32_t &idx = m_headR;
                int &cnt = m_v[idx];
                if (cnt > 0)
                {
                    *dst = m_q[idx];
                    cnt = 0;
                    hasData = true;
                    ++idx;
                    if (idx >= m_items)
                    {
                        idx = 0;
                    }
                }
            }
            return hasData;
        }

    public:
        uint32_t m_items; // predefined # of elements in queue
        uint32_t m_headW; // write index
        uint32_t m_headR; // read index

        std::vector<T> m_q;   // queue represented as vector
        std::vector<int> m_v; // count the wrt (++) / read(--) activities in m_q[i] index

        boost::mutex m_mutexRW;        
        std::string m_name;   // qname for debugging
        uint32_t    m_wrtDropCnt;
    };
}
#endif