/*******************************************************************************
* FileName:  Server.h
* Author: LiDai
* Date: 2020/03/09 14:28:55�����ļ�
* Version: A01
* Description:�������������
*  ��Ŀ��:IMServer
*  History:
*  <author>     <time>        <version>    <desc>
*   LiDai			 2020/03/09			A001        �����ļ������ó��ú���
*******************************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<string>
#include<vector>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include<sys/epoll.h>
#include"DataBaseManage.h"
#define MAX_CLNT 256
#define LOGIN 0
#define REG 1
#define MSG 2
using namespace std;
class CServer {
public:

	CServer();
	~CServer();
	int startServer();
	int handleRequest(const char* msg, vector<string>& splitMsg);
private:
	void errorHandling(const char* msg);//��������
	void handleClnt();     //����ͻ�������
	void sendMsg(const char* msg,long int len);
	void logIn(User user,int socket);
	void reg(User user,int socket);

	int initialServer();						//��ʼ�����������󶨵�ַ����ʼlisten
	int addCnnect();
	void Split(const string& src, const string& separator, vector<string>& dest);
private:
	int servSock, clntSock;					//�׽���
	int clntSocks[MAX_CLNT];
	int clntCnt;
	sockaddr_in servAddr, clntAddr;      //��ַ
	socklen_t clntAddrLen;

	epoll_event epEvent;
	epoll_event* epEvents;
	int epFd;

	mutex socketsMutx;

	CDataBaseManage* database;
};