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
	//��ʼ��mysql
	mysql_init(&mysql);  //����mysql�����ݿ�

	//����false������ʧ�ܣ�����true�����ӳɹ�
	if (!(mysql_real_connect(&mysql, "localhost", "root", "123456", database, 3306, NULL, 0))) //�������û��������룬���ݿ������˿ںţ�����дĬ��0����3306��
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
	//�ͷ���Դ
	mysql_free_result(res);
	mysql_close(&mysql);
}

bool CDataBaseManage::QueryDatabase(const char* table)
{
	sprintf(query, "select * from %s;",table); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
	mysql_query(&mysql, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
	//����0 ��ѯ�ɹ�������1��ѯʧ��
	if (mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("query success\n");
	}
	//��ȡ�����
	if (!(res = mysql_store_result(&mysql)))    //���sql�������󷵻صĽ����
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}

	//��ӡ��������
	printf("Number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	int  num_fields = mysql_num_fields(res);   //��ȡ��������ܹ����ֶ�����������
	printf("Number of data column returned: %d\n", num_fields);
	//��ȡ�ֶε���Ϣ
	char* str_field[32];  //����һ���ַ�������洢�ֶ���Ϣ
	for (int i = 0; i < num_fields; i++)   //����֪�ֶ�����������»�ȡ�ֶ���
	{
		str_field[i] = mysql_fetch_field(res)->name;
	}
	for (int i = 0; i < num_fields; i++)   //��ӡ�ֶ�
		printf("%10s\t", str_field[i]);
	printf("\n");
	//��ӡ��ȡ������
	while ((column = mysql_fetch_row(res)))   //����֪�ֶ���������£���ȡ����ӡ��һ��
	{
		printf("%10s\t%10s\n", column[0], column[1]);  //column��������
	}
	return true;
}


bool CDataBaseManage::InsertData(const User &user)
{
	sprintf(query, "insert into accounts (id,password,nickname) values ('%s', '%s','%s');", user.id, user.password, user.nickname);  //������취ʵ���ֶ��ڿ���̨�ֶ�����ָ��
	if (mysql_query(&mysql, query))        //ִ��SQL���
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
	sprintf(query, "select * from %s WHERE BINARY id='%s';", table, user.id); //ִ�в�ѯ��䣬�����ǲ�ѯ���У�user�Ǳ��������ü����ţ���strcpyҲ����
	mysql_query(&mysql, "set names gbk"); //���ñ����ʽ��SET NAMES GBKҲ�У�������cmd����������
	//����0 ��ѯ�ɹ�������1��ѯʧ��
	if (mysql_query(&mysql, query))        //ִ��SQL���
	{
		printf("Query failed (%s)\n", mysql_error(&mysql));
		return false;
	}
	else
	{
		printf("Query success\n");
	}
	//��ȡ�����
	if (!(res = mysql_store_result(&mysql)))    //���sql�������󷵻صĽ����
	{
		printf("Couldn't get result from %s\n", mysql_error(&mysql));
		return false;
	}

	//��ӡ��������
	//printf("Number of dataline returned: %d\n", mysql_affected_rows(&mysql));
	int  num_fields = mysql_num_fields(res);   //��ȡ��������ܹ����ֶ�����������
	//printf("Number of data column returned: %d\n", num_fields);
	//��ȡ�ֶε���Ϣ
	//char* str_field[32];  //����һ���ַ�������洢�ֶ���Ϣ
	//for (int i = 0; i < num_fields; i++)   //����֪�ֶ�����������»�ȡ�ֶ���
	//{
	//	str_field[i] = mysql_fetch_field(res)->name;
	//}
	//for (int i = 0; i < num_fields; i++)   //��ӡ�ֶ�
	//	printf("%10s\t", str_field[i]);
	//printf("\n");
	//��ӡ��ȡ������
	column = mysql_fetch_row(res);
	//while ((column = mysql_fetch_row(res)))   //����֪�ֶ���������£���ȡ����ӡ��һ��
	//{
	//	printf("%10s\t%10s\n", column[0], column[1]);  //column��������
	//}
	if (column != NULL)
	{
		string dbPassword = column[1];
		string urPassWord = user.password;
		if (dbPassword == urPassWord)
		{
			user.setNickname(column[2]);
			return true;      //��ѯ�����Ƿ���ȷ
		}
	}
	return false;
}
