#include "DataBaseManage.h"
#include<iostream>
#include<string.h>
using namespace std;
CDataBaseManage::CDataBaseManage()
{
}

CDataBaseManage::~CDataBaseManage()
{
}

bool CDataBaseManage::ConnectDatabase(const char* database)
{
	//初始化mysql
	mysql_init(&mysql);  //连接mysql，数据库

	//返回false则连接失败，返回true则连接成功
	if (!(mysql_real_connect(&mysql, "localhost", "root", "123456", database, 3306, NULL, 0))) //主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等
	{
		printf("Error connecting to database:%s\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Connect success...\n");
		return true;
	}
}

void CDataBaseManage::FreeConnect()
{
	//释放资源
	mysql_free_result(res);
	mysql_close(&mysql);
}

bool CDataBaseManage::QueryDatabase(const char* table)
{
	sprintf(query, "select * from %s;",table); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
	mysql_query(&mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码
	//返回0 查询成功，返回1查询失败
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	//获取结果集
	if (!(res = mysql_store_result(&mysql)))    //获得sql语句结束后返回的结果集
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}

	//打印数据行数
	printf("Number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	int  num_fields = mysql_num_fields(res);   //获取结果集中总共的字段数，即列数
	printf("Number of data column returned: %d\n", num_fields);
	//获取字段的信息
	char* str_field[32];  //定义一个字符串数组存储字段信息
	for (int i = 0; i < num_fields; i++)   //在已知字段数量的情况下获取字段名
	{
		str_field[i] = mysql_fetch_field(res)->name;
	}
	for (int i = 0; i < num_fields; i++)   //打印字段
		printf("%10s\t", str_field[i]);
	printf("\n");
	//打印获取的数据
	while ((column = mysql_fetch_row(res)))   //在已知字段数量情况下，获取并打印下一行
	{
		printf("%10s\t%10s\n", column[0], column[1]);  //column是列数组
	}
	return true;
}


bool CDataBaseManage::InsertData(const User &user)
{
	sprintf(query, "insert into accounts (id,password,nickname) values ('%s', '%s','%s');", user.id, user.password, user.nickname);  //可以想办法实现手动在控制台手动输入指令
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("insert failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}

bool CDataBaseManage::SerchUser(const char* table, User& user)
{
	sprintf(query, "select * from %s WHERE BINARY id='%s';", table, user.id); //执行查询语句，这里是查询所有，user是表名，不用加引号，用strcpy也可以
	mysql_query(&mysql, "set names gbk"); //设置编码格式（SET NAMES GBK也行），否则cmd下中文乱码
	//返回0 查询成功，返回1查询失败
	if (mysql_query(&mysql, query))        //执行SQL语句
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Query success\n");
	}
	//获取结果集
	if (!(res = mysql_store_result(&mysql)))    //获得sql语句结束后返回的结果集
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}

	//打印数据行数
	//printf("Number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	int  num_fields = mysql_num_fields(res);   //获取结果集中总共的字段数，即列数
	//printf("Number of data column returned: %d\n", num_fields);
	//获取字段的信息
	//char* str_field[32];  //定义一个字符串数组存储字段信息
	//for (int i = 0; i < num_fields; i++)   //在已知字段数量的情况下获取字段名
	//{
	//	str_field[i] = mysql_fetch_field(res)->name;
	//}
	//for (int i = 0; i < num_fields; i++)   //打印字段
	//	printf("%10s\t", str_field[i]);
	//printf("\n");
	//打印获取的数据
	column = mysql_fetch_row(res);
	//while ((column = mysql_fetch_row(res)))   //在已知字段数量情况下，获取并打印下一行
	//{
	//	printf("%10s\t%10s\n", column[0], column[1]);  //column是列数组
	//}
	if (column != NULL)
	{
		string dbPassword = column[1];
		string urPassWord = user.password;
		if (dbPassword == urPassWord)
		{
			user.setNickname(column[2]);
			return true;      //查询密码是否正确
		}
	}
	return false;
}
