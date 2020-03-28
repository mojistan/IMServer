#pragma once
#include"mysql/mysql.h"
#include "User.h"

class CDataBaseManage
{
public:
	CDataBaseManage();
	~CDataBaseManage();
	bool ConnectDatabase(const char* database);
	void FreeConnect();
	bool QueryDatabase(const char* table);

	//插入数据
	bool InsertData(const User &user);
	//查询用户
	bool SerchUser(const char* table,User& user);
private:
	MYSQL mysql; //mysql连接
	MYSQL_FIELD* fd;  //字段列数组
	MYSQL_RES* res; //这个结构代表返回行的一个查询结果集
	MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
	char query[150]; //查询语句
};