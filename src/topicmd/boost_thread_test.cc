#include "gtest/gtest.h"

#include <boost/thread.hpp>
#include <iostream>

using namespace std;

void ThreadFunction()
{
  int counter = 0;

  for(;;)
    {
      counter++;
      // cout << "thread iteration " << counter << " Press Enter to stop" << endl;

      try
	    {
        // Sleep and check for interrupt.
        // To check for interrupt without sleep,
        // use boost::this_thread::interruption_point()
        // which also throws boost::thread_interrupted
	      boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	    }
      catch(boost::thread_interrupted&)
	    {
        // cout << "Thread is stopped" << endl;
        return;
	    }
    }
}

TEST(Boost, Thread) 
{
  // Start thread
  boost::thread t(&ThreadFunction);

  EXPECT_EQ(t.joinable(), 1);

  // Ask thread to stop
  t.interrupt();

  // Join - wait when thread actually exits
  t.join();

  EXPECT_EQ(t.joinable(), 0);
}
