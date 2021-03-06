//
// Copyright 2011 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDED_UHD_UTILS_LOG_HPP
#define INCLUDED_UHD_UTILS_LOG_HPP

#include <uhd/config.hpp>
#include <uhd/utils/pimpl.hpp>
#include <boost/current_function.hpp>
#include <boost/thread/thread.hpp>
#include <ostream>
#include <string>
#include <sstream>
#include <iostream>

/*! \file log.hpp
 * The UHD logging facility
 * =========================
 *
 * The logger enables UHD library code to easily log events into a file and display
 * messages above a certain level in the terminal.
 * Log entries are time-stamped and stored with file, line, and function.
 * Each call to the UHD_LOG macros is thread-safe. Each thread will aquire the lock
 * for the logger.
 *
 * To disable console logging completely at compile time specify
 * `-DUHD_LOG_CONSOLE_DISABLE` during configuration with CMake.
 *
 * By default no file logging will occur. Set a log file path:
 *  - at compile time by specifying `-DUHD_LOG_FILE=$file_path`
 *  - and/or override at runtime by setting the environment variable `UHD_LOG_FILE`
 *
 * Log levels
 * ----------
 *
 * All log messages with verbosity greater than or equal to the log level
 * (in other words, as often or less often than the current log level)
 * are recorded to std::clog and/or the log file.
 * Log levels can be specified using string or numeric values of uhd::log::severity_level.
 *
 * The default log level is "info", but can be overridden:
 *  - at compile time by setting the pre-processor define `-DUHD_LOG_MIN_LEVEL`.
 *  - at runtime by setting the environment variable `UHD_LOG_LEVEL`.
 *  - for console logging by setting `(-D)UHD_LOG_CONSOLE_LEVEL` at run-/compiletime
 *  - for file logging by setting `(-D)UHD_LOG_FILE_LEVEL` at run-/compiletime
 *
 * UHD_LOG_LEVEL can be the name of a verbosity enum or integer value:
 *   - Example pre-processor define: `-DUHD_LOG_MIN_LEVEL=3`
 *   - Example pre-processor define: `-DUHD_LOG_MIN_LEVEL=info`
 *   - Example environment variable: `export UHD_LOG_LEVEL=3`
 *   - Example environment variable: `export UHD_LOG_LEVEL=info`
 *
 * Log formatting
 * --------------
 *
 * The log format for messages going into a log file is CSV.
 * All log messages going into a logfile will contain following fields:
 * - timestamp
 * - thread-id
 * - source-file + line information
 * - severity level
 * - component/channel information which logged the information
 * - the actual log message
 *
 * The log format of log messages displayed on the terminal is plain text with space separated tags prepended.
 * For example:
 *    - `[INFO] [x300] This is a informational log message`
 *
 * The log format for log output on the console by using these preprocessor defines in CMake:
 * - `-DUHD_LOG_CONSOLE_TIME` adds a timestamp [2017-01-01 00:00:00.000000]
 * - `-DUHD_LOG_CONSOLE_THREAD` adds a thread-id `[0x001234]`
 * - `-DUHD_LOG_CONSOLE_SRC` adds a sourcefile and line tag `[src_file:line]`
 */

/*
 * Advanced logging macros
 * UHD_LOG_MIN_LEVEL definitions
 * trace: 0
 * debug: 1
 * info: 2
 * warning: 3
 * error: 4
 * fatal: 5
 */

namespace uhd {
    namespace log {
        /** logging severity levels
         * Either numeric value or string can be used to define loglevel in CMake and environment variables
         */
        enum severity_level {
            trace   = 0, /**< displays every available log message */
            debug   = 1, /**< displays most log messages necessary for debugging internals */
            info    = 2, /**< informational messages about setup and what is going on*/
            warning = 3, /**< something is not right but operation can continue */
            error   = 4, /**< something has gone wrong */
            fatal   = 5, /**< something has gone horribly wrong */
            off = 6, /**< logging is turned off */
        };

        // The operator is used when putting the severity level to log
        template <typename CharT, typename TraitsT>
        inline std::basic_ostream<CharT, TraitsT>&
        operator<<(std::basic_ostream<CharT, TraitsT>& strm, severity_level level)
        {
            static const char* strings[] = {"TRACE",   "DEBUG", "INFO",
                                            "WARNING", "ERROR", "FATAL"
            };

            if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings)) {
                strm << strings[level];
            } else {
                strm << static_cast<int>(level);
            }

            return strm;
        }


    }
}


// internal logging macro to be used in other macros
#define _UHD_LOG_INTERNAL(component, level) \
    uhd::_log::log(level, __FILE__, __LINE__, component, boost::this_thread::get_id())

// macro-style logging (compile-time determined)
#if UHD_LOG_MIN_LEVEL < 1
#define UHD_LOG_TRACE(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::trace) << message;
#else
#define UHD_LOG_TRACE(component, message)
#endif

#if UHD_LOG_MIN_LEVEL < 2
#define UHD_LOG_DEBUG(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::debug) << message;
#else
#define UHD_LOG_DEBUG(component, message)
#endif

#if UHD_LOG_MIN_LEVEL < 3
#define UHD_LOG_INFO(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::info) << message;
#else
#define UHD_LOG_INFO(component, message)
#endif

#if UHD_LOG_MIN_LEVEL < 4
#define UHD_LOG_WARNING(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::warning) << message;
#else
#define UHD_LOG_WARNING(component, message)
#endif

#if UHD_LOG_MIN_LEVEL < 5
#define UHD_LOG_ERROR(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::error) << message;
#else
#define UHD_LOG_ERROR(component, message)
#endif

#if UHD_LOG_MIN_LEVEL < 6
#define UHD_LOG_FATAL(component, message) \
    _UHD_LOG_INTERNAL(component, uhd::log::fatal) << message;
#else
#define UHD_LOG_FATAL(component, message)
#endif

#ifndef UHD_LOG_FASTPATH_DISABLE
#define UHD_LOG_FASTPATH(message)               \
    std::cerr << message << std::flush;
#else
#define UHD_LOG_FASTPATH(message)
#endif

// iostream-style logging
#define UHD_LOGGER_TRACE(component) _UHD_LOG_INTERNAL(component, uhd::log::trace)
#define UHD_LOGGER_DEBUG(component) _UHD_LOG_INTERNAL(component, uhd::log::debug)
#define UHD_LOGGER_INFO(component) _UHD_LOG_INTERNAL(component, uhd::log::info)
#define UHD_LOGGER_WARNING(component) _UHD_LOG_INTERNAL(component, uhd::log::warning)
#define UHD_LOGGER_ERROR(component) _UHD_LOG_INTERNAL(component, uhd::log::error)
#define UHD_LOGGER_FATAL(component) _UHD_LOG_INTERNAL(component, uhd::log::fatal)


//! Helpful debug tool to print site info
#define UHD_HERE()                                              \
    UHD_LOGGER_DEBUG("DEBUG") << __FILE__ << ":" << __LINE__;

//! Helpful debug tool to print a variable
#define UHD_VAR(var)                                        \
    UHD_LOGGER_DEBUG("DEBUG") << #var << " = " << var;

//! Helpful debug tool to print a variable in hex
#define UHD_HEX(var)                                                    \
    UHD_LOGGER_DEBUG("DEBUG") << #var << " = 0x" << std::hex << std::setfill('0') << std::setw(8) << var << std::dec;



namespace uhd{ namespace _log{

    //! Internal logging object (called by UHD_LOG macros)
    class UHD_API log {
    public:
        log(
            const uhd::log::severity_level verbosity,
            const std::string &file,
            const unsigned int line,
            const std::string &component,
            const boost::thread::id id
        );

        ~log(void);

        static void set_log_level(uhd::log::severity_level level);
        static void set_console_level(uhd::log::severity_level level);
        static void set_file_level(uhd::log::severity_level level);

        // Macro for overloading insertion operators to avoid costly
        // conversion of types if not logging.
        #define INSERTION_OVERLOAD(x)   log& operator<< (x)             \
                                        {                               \
                                            if(_log_it) {               \
                                                _ss << val ;            \
                                            }                           \
                                            return *this;               \
                                        }

        // General insertion overload
        template <typename T>
        INSERTION_OVERLOAD(T val)

        // Insertion overloads for std::ostream manipulators
        INSERTION_OVERLOAD(std::ostream& (*val)(std::ostream&))
        INSERTION_OVERLOAD(std::ios& (*val)(std::ios&))
        INSERTION_OVERLOAD(std::ios_base& (*val)(std::ios_base&))

    private:
        std::ostringstream _ss;
        std::ostringstream _file;
        std::ostringstream _console;
        bool _log_it;
        bool _log_file;
        bool _log_console;
    };

    } //namespace uhd::_log
    namespace log{
        inline void
        set_console_level(severity_level level){
            ::uhd::_log::log::set_console_level(level);
        }

        inline void
        set_log_level(severity_level level){
            ::uhd::_log::log::set_log_level(level);
        }

        inline void
        set_file_level(severity_level level){
            ::uhd::_log::log::set_file_level(level);
        }
    }
}

#endif /* INCLUDED_UHD_UTILS_LOG_HPP */
