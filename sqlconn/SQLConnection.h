#ifndef SQL_CONNECTION_H__ // #include guards
#define SQL_CONNECTION_H__

/* all necessary functionalities for mysql interaction */

#include <mysql/mysql.h>
#include <string>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>


class SQLConnection
{
public:
	SQLConnection(
		const std::string& server, int port, const std::string& user,
		const std::string& password, const std::string& database, int id=-1);

	virtual ~SQLConnection();

	bool connect(int retry=2); //连接,默认尝试2次
	bool close();              //关闭连接
	bool isValide();

	//检查query是否可以执行成功
	bool checkQuery(std::string query, std::string& error);

	//和checkQuery差不多 只是返回query结果
	std::vector<std::string> infoQuery(
		const std::string& query, std::string& error);

	//比infoQuery列进一步,拿到每行的每一列的结果
	std::vector<std::vector<std::string>> selectQuery(
		const std::string& query, std::string& error);

	std::string getServer();
	std::string getDatabase();
	std::string getUser();
    std::string escapeString(std::string_view _str);
	int getPoolId();

private:
	MYSQL* conn;
	MYSQL_RES* result;
	MYSQL_ROW row;
	std::string server;
	std::string user;
	std::string password;
	std::string database;
	int port;
	int index; //id in pool
};


SQLConnection::SQLConnection(
	const std::string& server, int port, const std::string& user,
	const std::string& password, const std::string& database, int id)
{
	this->server = server;
	this->user = user;
	this->password = password;
	this->database = database;
	this->port = port;
	this->index = id;
	conn = nullptr;
	result = nullptr;
}

SQLConnection::~SQLConnection()
{
	close();
}

bool SQLConnection::connect(int retry)
{
	bool success = false;
	if(retry <= 0 )
	{
		std::cout << "Failed to connect to host=" << server
				<< " db=" << database << " user=" << user << std::endl;
		return false;
	}

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_LOCAL_INFILE, 0);

	conn = mysql_real_connect(
			conn, server.c_str(), user.c_str(),
			password.c_str(), database.c_str(), port,
			NULL, CLIENT_MULTI_STATEMENTS);

	if (conn != nullptr)
		success = true;
	else
	{
		//cout << ". . Trying to reconnect after 1 second . ." << endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		success = connect(retry--);
	}
	return success;
}

bool SQLConnection::close()
{
	bool success = false;
	if (conn)
	{
		mysql_close(conn);
		conn = nullptr;
		success = true;
	}
	return success;
}

bool SQLConnection::isValide()
{
	if (conn && conn->server_status == MYSQL_STATUS_READY)
		return true;
	return false;
}


bool SQLConnection::checkQuery(std::string query, std::string& error)
{
	if (isValide())
	{
		int code = mysql_query(conn, query.c_str());
		if (code != 0)
		{
			error = std::string(mysql_error(conn));
			return false;
		}

		while(mysql_more_results(conn))
			mysql_next_result(conn);

		return true;
	}
	return false;
}


std::vector<std::string> SQLConnection::infoQuery(
	const std::string& query, std::string& error)
{
	std::vector<std::string> rows;
    if(conn)
    {
        int code = mysql_query(conn, query.c_str());
        if(code != 0)
			error = mysql_error(conn);
        else
        {
            MYSQL_ROW row;
            MYSQL_RES * result = mysql_store_result(conn);
            if(result)
            {
                while (((row=mysql_fetch_row(result)) !=NULL))
                {
                    rows.push_back(row[0]);
                }
                mysql_free_result(result);
            }
        }
    }
    else
        error = "ERROR: DB connection is not available !";
    return std::move(rows);
}

std::vector<std::vector<std::string>> SQLConnection::selectQuery(
	const std::string& query, std::string& error)
{
    std::vector<std::vector<std::string>> rows;

    if(conn)
    {
        int code = mysql_query(conn, query.c_str());
        if(code != 0)
			error = mysql_error(conn);
        else
        {
            MYSQL_ROW row;
            MYSQL_RES * result = mysql_store_result(conn);
            if(result)
            {
                while((row = mysql_fetch_row(result)))
                {
                    std::vector <std::string> temp;
                    for (int i=0 ; i < (int)mysql_num_fields(result); i++)
                    {
                        if(row[i]==NULL)
                            temp.push_back("NULL");
                        else
                            temp.push_back(row[i]);
                    }
                    if(!temp.empty())
                        rows.push_back(temp);
                }
                mysql_free_result(result);
            }
        }
    }
    else
        error = "ERROR: DB connection is not available !";
    return std::move(rows);
}

std::string SQLConnection::getServer()
{
	return this->server;
}

std::string SQLConnection::getDatabase()
{
	return this->database;
}

std::string SQLConnection::getUser()
{
	return this->user;
}

int SQLConnection::getPoolId()
{
	return this->index;
}

std::string SQLConnection::escapeString(std::string_view _str){
    std::string ret;
    ret.resize(_str.length() *2 + 2);
    mysql_real_escape_string(this->conn, ret.data(),_str.data(), _str.length());
    return ret;
}

#endif
