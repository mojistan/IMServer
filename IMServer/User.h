class  User
{
public:
	char* id;
	char* password;
	char* nickname;
public:
	User(const char* tid, const char* tpassword, const char* tnickname);
	User(const char* tid, const char* tpassword);      //只输入用户名密码
	~User();
	const char* getId();
	const char* getPassword();
	const char* getNickname();
	void setNickname(const char* name);
};