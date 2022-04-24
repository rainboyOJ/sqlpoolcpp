
code discard,please use[rainboyOJ/morden_cppdb](https://github.com/rainboyOJ/morden_cppdb)

# ##########################################################

    MYSQL CONNECTION POOL FOR REALTIME DATA

    AUTHOR: DOM SILVA

    COPYRIGHT: ENCE20 Inc.

    WEBSITE: https://ence20.com

# ##########################################################

![ARCHITECTURE](Pool_Architecture.png)


# Install Dependencies CentOS

- yum -y install mysql-community-libs mysql-devel

# INCLUDE IN PROJECT

``` C++

#include "./sqlconn/ConnectionPool.h"

std::string host = "127.0.0.1";
std::string username = "root";
std::string password = "password";
std::string database = "mydb";
size_t NUM_CONNS = 3;

std::shared_ptr<ConnectionPool> connPool;
connPool.reset(new ConnectionPool(
                host, port, username, password, database, NUM_CONNS));

// get connection
auto sqlPtr = connPool->GetConnecion();

// Use connection



// release connection
connPool->ReleaseConnecion(sqlPtr);

```

# RUN EXAMPLE

- update database credentials by editing config.txt 

- on command line run:

``` bash 
cd example

./compile.sh

./multiConn

```

- press Ctrl+C to exit


