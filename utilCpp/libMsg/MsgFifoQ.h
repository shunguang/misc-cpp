#ifndef _MSG_FIFO_Q_H_
#define _MSG_FIFO_Q_H_

#include "Msg.h"
#include "MsgDefs.h"
namespace app {
	class MSG_EXPORT MsgFifoQ
	{
	private:
		int						qsize;
		std::string				name;                 //for debug purpose
		std::queue<MsgPtr>		the_queue;
		mutable boost::mutex	the_mutex;
		boost::condition_variable the_condition_variable;
	public:
		MsgFifoQ(const int qSize_ = 10, const std::string &name_ = "Unknown") : qsize(qSize_), name(name_)
		{
		}

		bool isFull() {
			int s = size();
			if (s >= qsize) {
				return true;
			}
			else {
				return false;
			}
		}

		int size() {
			int s = 0;
			{
				boost::mutex::scoped_lock lock(the_mutex);
				s = the_queue.size();
			}
			return s;
		}

		bool empty() const
		{
			boost::mutex::scoped_lock lock(the_mutex);
			return the_queue.empty();
		}

		void clear()
		{
			boost::mutex::scoped_lock lock(the_mutex);
			while(!the_queue.empty()){
				the_queue.pop();
			}
		}

		void push( const MsgPtr &it)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			the_queue.push(it);
			lock.unlock();
			the_condition_variable.notify_one();
		}

		bool try_pop(MsgPtr& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			if (the_queue.empty()){
				return false;
			}

			popped_value = the_queue.front();
			the_queue.pop();
			return true;
		}

		void wait_and_pop(MsgPtr& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			while (the_queue.empty())
			{
				the_condition_variable.wait(lock);
			}

			popped_value = the_queue.front();
			the_queue.pop();
		}
	};
}
#endif
