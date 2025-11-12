#ifndef _FIFO_Q_H_
#define _FIFO_Q_H_

#include "libUtil/DataTypes.h"
#include "UtilDefs.h"
namespace app {
	template <class T>
	class UTIL_EXPORT FifoQ
	{
	private:
		int					qsize_;
		std::string			qname_;                 //for debug purpose
		std::queue<T>		q_;
		mutable boost::mutex	qmutex_;
		boost::condition_variable qcv_;
	public:
		FifoQ(const int qSize_ = 10, const std::string &qname_ = "FifoQ") : qsize_(qSize_), qname_(qname_)
		{
		}

		bool isFull() {
			int s = size();
			return ( s >= qsize_);
		}

		int size() {
			int s = 0;
			{
				boost::mutex::scoped_lock lock(qmutex_);
				s = q_.size();
			}
			return s;
		}

		bool empty() const
		{
			bool f = false;
			{
				boost::mutex::scoped_lock lock(qmutex_);
				f = q_.empty();
			}
			return f;
		}

		void clear()
		{
			boost::mutex::scoped_lock lock(qmutex_);
			while(!q_.empty()){
				q_.pop();
			}
		}

		void push( const T &it)
		{
			boost::mutex::scoped_lock lock(qmutex_);
			q_.push(it);
			qcv_.notify_one();
		}

		bool try_pop(T& popped_value)
		{
			boost::mutex::scoped_lock lock(qmutex_);
			if (q_.empty()){
				return false;
			}

			popped_value = q_.front();
			q_.pop();
			return true;
		}

		void wait_and_pop(T& popped_value)
		{
			boost::mutex::scoped_lock lock(qmutex_);
			while (q_.empty())
			{
				qcv_.wait(lock);
			}

			popped_value = q_.front();
			q_.pop();
		}
	};
}
#endif
