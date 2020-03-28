// main.cpp : Ö÷º¯Êý
//
//
#include "Server.h"
#include<stdio.h>
#include"DataBaseManage.h"
#include <iostream>
#include <signal.h>     /* signal, raise, sig_atomic_t */
using namespace std;
int main(int argc, char* argv[])
{
	signal(SIGPIPE, SIG_IGN);
	CServer* server = new CServer();
	server->startServer();
}