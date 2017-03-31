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
#  define LOG(level,format,...) \
    do{ \
        fprintf(stderr, "[%d]["#level"] "#format"\n", __LINE__, ##__VA_ARGS__); \
    }while(0)
# else
#  define LOG(level,format,...)
# endif // LOG

#endif // DEBUG

#endif // __COMM_H__
