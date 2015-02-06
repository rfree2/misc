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
#include <time.h> // for localtime_r that is not in std::

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined (WIN64)
	#define OS_TYPE_WINDOWS
#elif defined(__unix__) || defined(__posix) || defined(__linux) || defined(__darwin) || defined(__APPLE__) || defined(__clang__)
	#define OS_TYPE_POSIX
#else
	#warning "Compiler/OS platform is not recognized. Just assuming it will work as POSIX then"
	#define OS_TYPE_POSIX
#endif

std::string get_current_time() {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  time_t time_now = std::chrono::system_clock::to_time_t(now);
  std::chrono::high_resolution_clock::duration duration = now.time_since_epoch();
  int64_t micro = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

  // std::localtime() - This function may not be thread-safe. 
	#if OS_TYPE_WINDOWS
  	struct tm * tm_pointer = std::localtime( &time_now ); // thread-safe on mingw-w64 (thread local variable) and on MSVC btw
		// http://stackoverflow.com/questions/18551409/localtime-r-support-on-mingw
		// tm_pointer points to thread-local data, memory is owned/managed by the system/library
	#else
		// linux, freebsd, have this
		struct tm tm_object; // automatic storage duration http://en.cppreference.com/w/cpp/language/storage_duration
		struct tm * tm_pointer = & tm_object; // just point to our data
		auto x = localtime_r( &time_now , tm_pointer	); // modifies our own (this thread) data in tm_object, this is safe http://linux.die.net/man/3/localtime_r
		if (x != tm_pointer) return "(internal error in get_current_time)"; // redundant check in case of broken implementation of localtime_r
	#endif
	// tm_pointer now points to proper time data, and that memory is automatically managed
	if (!tm_pointer) return "(internal error in get_current_time - NULL)"; // redundant check in case of broken implementation of localtime_r

  std::stringstream stream;
	stream << std::setfill('0') 
		    << std::setw(2) << tm_pointer->tm_year+1900
		<< '-' << std::setw(2) << tm_pointer->tm_mon+1
		<< '-' << std::setw(2) << tm_pointer->tm_mday
		<< ' ' << std::setw(2) << tm_pointer->tm_hour
		<< ':' << std::setw(2) << tm_pointer->tm_min
		<< ':' << std::setw(2) << tm_pointer->tm_sec 
		<< '.' << std::setw(6) << (micro%1000000); // 6 because microseconds
  //if (date) std::strftime(date_buff, sizeof(date_buff), "%d-%b-%Y %H:%M:%S.", date); else date_buff[0]='\0';
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

