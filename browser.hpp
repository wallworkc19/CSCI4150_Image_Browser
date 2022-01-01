#pragma once

// OpenCV command line parser functions
// Vartable keys accepted by command line parser
//!< keys contains possible arguments and their default values
//!< rows defaults to 0 in Windows and 720 in Linux or Apple
//!< cols defaults to 0 in Windows and 1280 in Linux or Apple

const std::string keys =
{
	"{help h usage ?	|      | print this message								}"
#ifdef _WIN32
	"{rows r			|  0   | Max number of rows on screen					}"
	"{cols c			|  0   | Max number of columns on screen				}"
#else
	"{rows r			| 720  | Max number of rows on screen					}"
	"{cols c			|1280  | Max number of columns on screen				}"
#endif
	"{@directory		|<none>| Directory that contains the pictures to browse	}"
};