#include "User.h"
#include<cstring>
User::User(const char* tid, const char* tpassword, const char* tnickname)
{
	id = new char[strlen(tid) + 1];
	password = new char[strlen(tpassword) + 1];
	nickname = new char[strlen(tnickname) + 1];
	memcpy(id, tid, strlen(tid) + 1);										//应该检查长度
	memcpy(password, tpassword, strlen(tpassword) + 1);
	memcpy(nickname, tnickname, strlen(tnickname) + 1);
}
User::User(const char* tid, const char* tpassword)       //只输入用户名密码
{
	id = new char[strlen(tid) + 1];
	password = new char[strlen(tpassword) + 1];
	memcpy(id, tid, strlen(tid) + 1);										//应该检查长度
	memcpy(password, tpassword, strlen(tpassword) + 1);
	nickname = NULL;
}
User::~User()
{
	if(id){
		delete []id;
		id = NULL;
	}
	if (password) {
		delete []password;
		password = NULL;
	}
	if (nickname) {
		delete []nickname;
		nickname = NULL;
	}

}
const char* User::getId()
{
	return id;
}
const char* User::getPassword()
{
	return password;
}
const char* User::getNickname()
{
	return nickname;
}
void User::setNickname(const char* name)
{
	nickname = new char[strlen(name) + 1];
	memcpy(nickname, name, strlen(name) + 1);
}