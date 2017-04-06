#include "../../comm.h"
#include <stdio.h>
#include <string.h>

#include <mysql/mysql.h>


#define HOST "localhost"
#define USER "test"
#define PWD  "123456"
#define DB1  "test"


int main()
{
    MYSQL conn;
    char buf[] = "insert into test(id, name,sex,qq) value(5,'five','1',987593)";

    // initialize connect
    mysql_init(&conn);

    //connect to mysql
    if(mysql_real_connect(&conn, HOST, USER, PWD, DB1, 3306, NULL, 0) == NULL)
    {
        ELOG(mysql_real_connect: %s, mysql_error(&conn));
        return -1;
    }

    if(mysql_real_query(&conn, buf, strlen(buf))!= 0)
    {
        ELOG(2: %s, mysql_error(&conn));
        mysql_close(&conn);
        return -1;
    }

    //close connect
    mysql_close(&conn);

    return 0;
}
