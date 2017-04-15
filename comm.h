/*
 * included:
 *    LOG v0.1 since 3/31/2017
 * author:
 *    wu
 * */
#ifndef __COMM_H__
#define __COMM_H__

#ifdef DEBUG

/*level of log*/
# define INFO  INFO
# define WARN  WARN
# define ERROR ERROR

# define _LOG(out, level, format, ...) \
    do{ \
        fprintf(out, "[%d]["#level"] "#format, __LINE__, ##__VA_ARGS__); \
    }while(0)

/*************************************/
# define LOG(level, format, ...) \
    do{ \
        _LOG(stdout, level,format\n, ##__VA_ARGS__); \
    }while(0)

/*************************************/
# define LOGI(format, ...) \
    do{ \
        _LOG(stdout, INFO, format\n, ##__VA_ARGS__); \
    }while(0)

/*************************************/
# define ELOG(format, ...) \
    do{ \
        _LOG(stderr, ERROR, format, ##__VA_ARGS__); \
        perror(" "); \
    }while(0)
# define LOGE(format, ...) \
    ELOG(format, ##__VA_ARGS__);


/*************************************/


# else

#  define LOG(level,format, ...)
#  define ELOG(format, ...)
#  define LOGE(format, ...)
#  define LOGI(format, ...)

#endif // DEBUG

#endif // __COMM_H__
