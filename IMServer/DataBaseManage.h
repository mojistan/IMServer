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

	//��������
	bool InsertData(const User &user);
	//��ѯ�û�
	bool SerchUser(const char* table,User& user);
private:
	MYSQL mysql; //mysql����
	MYSQL_FIELD* fd;  //�ֶ�������
	MYSQL_RES* res; //����ṹ�������е�һ����ѯ�����
	MYSQL_ROW column; //һ�������ݵ����Ͱ�ȫ(type-safe)�ı�ʾ����ʾ�����е���
	char query[150]; //��ѯ���
};