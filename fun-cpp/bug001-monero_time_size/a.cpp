#include <ctime>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <limits>
#include <algorithm>

typedef std::lock_guard<std::mutex> t_lg; // lock guard // TODO move to library
std::mutex g_using_localtime; // TODO move to library, to have all thread see it
std::string get_current_time() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  time_t time_now = std::chrono::system_clock::to_time_t(now);
  std::chrono::high_resolution_clock::duration duration = now.time_since_epoch();
  int64_t micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  struct tm * date=NULL;
  { t_lg lock_guard(g_using_localtime); // std::localtime() - This function may not be thread-safe. 
    date = std::localtime(& time_now);
  }
  char date_buff[32];
  if (date) std::strftime(date_buff, sizeof(date_buff), "%d-%b-%Y %H:%M:%S.", date); else date_buff[0]='\0';
  std::stringstream stream;
  stream << date_buff << std::setfill('0') << std::setw(6) << (micro%1000000); // 6 because microseconds !
	// --> string D-M-Y hh:mm:ss.micro
  return stream.str();
}
int main() {

	typedef long long signed int t_safe;
	double factor=0.5;
	t_safe chunksize_good = (t_safe)( 1024 * std::max(1.0,factor) );
	size_t cb = 8192;
	t_safe all = cb;
	t_safe pos = 0;
	t_safe lenall = all-pos;
	t_safe len = std::min( chunksize_good , lenall);
	int v=0;
	if (len>0) v=1;
	if (len < std::numeric_limits<size_t>::max()) v=2;
	std::cout << get_current_time() << ":" << v << std::endl ;
}

