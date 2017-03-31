/*
 * included:
 *    LOG since 3/31/2017
 * */
#ifndef __COMM_H__
#define __COMM_H__

#ifdef DEBUG
# undef DEBUG
#endif

#ifndef DEBUG
# define DEBUG 1

/*level of log*/
# define INFO  INFO
# define WARN  WARN
# define ERROR ERROR

# if DEBUG == 1

#  define _LOG(out, level, format, ...) \
    do{ \
        fprintf(out, "[%d]["#level"] "#format, __LINE__, ##__VA_ARGS__); \
    }while(0)

#  define LOG(level, format, ...) \
    do{ \
        _LOG(stdout, level,format\n, ##__VA_ARGS__); \
    }while(0)

#  define ELOG(format, ...) \
    do{ \
        _LOG(stderr, ERROR, format, ##__VA_ARGS__); \
        perror(" "); \
    }while(0)

# else

#  define LOG(level,format, ...)
#  define ELOG(format, ...)

# endif // LOG

#endif // DEBUG

#endif // __COMM_H__
