#ifndef OS_DETECT_HPP
#define OS_DETECT_HPP

	#if defined _WIN32 || defined __WIN32__
		#define WOS_WINDOWS
	#elif defined __linux__ || defined __linux || defined linux
		#define WOS_LINUX
	#elif defined __APPLE__ && TARGET_OS_MAC
		#define WOS_OSX
	#else
		#error Unsupported operating system.
	#endif

#endif
