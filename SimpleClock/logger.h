#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif // _WIN32 || _WIN64

class logger 
{
private:
#if defined(_WIN32) || defined(_WIN64)
    using PATH_CHAR = wchar_t;
    using PATH_STRING = const std::wstring;
    using TITLE_STRING = std::wstring;
    using P_PATH_CHAR = LPCWSTR;
#define LOCAL_TIME(a, b) localtime_s(&a, &b);
#define _LOGGER_STR(a) L##a
#else
    using PATH_CHAR = char;
    using PATH_STRING = const std::string;
    using TITLE_STRING = std::string;
    using P_PATH_CHAR = const PATH_CHAR*;
#define LOCAL_TIME(a, b) localtime_r(&b, &a);
#define _LOGGER_STR(a) a
#endif // _WIN32 || _WIN64

	std::wfstream outputStream;
	TITLE_STRING title;

#if defined(_WIN32) || defined(_WIN64)
    void initializeOutputFile(){
        wchar_t fileName[FILENAME_MAX];
        GetEnvironmentVariable(L"USERPROFILE", fileName, FILENAME_MAX);
        ::lstrcatW(fileName, L"\\");
        ::lstrcatW(fileName, this->title.c_str());
        ::lstrcatW(fileName, L"_log_file.log");
        auto pid = ::GetCurrentProcessId();
        ::swprintf(fileName + ::lstrlen(fileName), FILENAME_MAX - ::lstrlen(fileName), L"%d", pid);
        outputStream = std::wfstream(fileName, std::ios_base::out | std::ios_base::app);
    }
#else
    void initializeOutputFile(){
        char default_log_file[PATH_MAX];
        pid_t pid = getpid();
        const char* home_path = ::getenv("HOME");
        ::strcpy(default_log_file, home_path);
        ::strcat(default_log_file, "/");
        ::strcat(default_log_file, this->title.c_str());
        ::strcat(default_log_file, "_log_file.log");
        char* append_ptr = default_log_file + ::strlen(default_log_file);
        ::sprintf(append_ptr, "%d", pid);

        outputStream = std::wfstream(default_log_file, std::ios_base::out | std::ios_base::app);
    }
#endif // _WIN32 || _WIN64

public:
    inline std::wfstream& ostream() { return this->outputStream; }
    inline void setTitle(const TITLE_STRING& t){ this->title = t;}

    // constructor
	logger(const TITLE_STRING& title, P_PATH_CHAR filename)
		: outputStream(filename, std::ios_base::out | std::ios_base::app),
		title(title) {}
	logger(const TITLE_STRING& title, PATH_STRING& filename)
		: outputStream(filename, std::ios_base::out | std::ios_base::app),
		title(title) {}
    logger(const TITLE_STRING& title) :title(title) {
        initializeOutputFile();
    }
    logger(){
        title = _LOGGER_STR("Default");
        initializeOutputFile();
    }

	void begin_log();
	~logger() {}
};

inline void logger::begin_log() {
	time_t t = ::time(NULL);
	struct tm _time_;
    LOCAL_TIME(_time_, t);

	this->outputStream << this->title.c_str() << " ";
	this->outputStream 
		<< std::setw(2) << std::right << _time_.tm_mon + 1 << L"-" 
		<< std::setfill(L'0') << std::setw(2) << std::right << _time_.tm_mday  << L" "
		<< std::setfill(L'0') << std::setw(2) << std::right << _time_.tm_hour << L":" 
		<< std::setfill(L'0') << std::setw(2) << std::right << _time_.tm_min << L":" 
		<< std::setfill(L'0') << std::setw(2) << std::right << _time_.tm_sec << L":  ";
}

#endif // _LOGGER_H
