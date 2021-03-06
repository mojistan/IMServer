#include "Server.h"

#define BUF_SIZE 1024
#define MAX_CLNT 256

void CServer::handleClnt(int sock)
{
    int clnt_sock = sock;
    long int str_len = 0, i;
    char msg[BUF_SIZE];

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    {
        if (str_len < 0)continue;
        vector<string> splitMsg;
        int type =  handleRequest(msg,splitMsg);     //获取消息类型
        switch (type)    //根据消息类型分类处理
        {
        case MSG:
            sendMsg(msg, strlen(msg));
            break;
        case REG:
            reg(User(splitMsg[1].data(), splitMsg[2].data(), splitMsg[3].data()),sock);
            break;
        case LOGIN:
            logIn(User(splitMsg[1].data(), splitMsg[2].data()),sock);
            break;
        default:
            break;
        }
   // sendMsg(msg, str_len);
    }
    //接收到消息为0时，代表当前客户端已经断开连接
    socketsMutx.lock();
    for (i = 0; i < clntCnt; i++) //删除没有连接的客户端
    {
        if (clnt_sock == clntSocks[i])
        {
            while (i++ < clntCnt - 1)
                clntSocks[i] = clntSocks[i + 1];
            break;
        }
    }
    clntCnt--;
    socketsMutx.unlock();
    close(clnt_sock);
}
void CServer::sendMsg(const char* msg, long int len) //向连接的所有客户端发送消息
{
    int i;
    socketsMutx.lock();
    for (i = 0; i < clntCnt; i++)
        write(clntSocks[i], msg, len);
    socketsMutx.unlock();
}
void CServer::logIn(User user,int socket)
{
    if (database->SerchUser("accounts", user))       //添加判断是否成功登陆
    {
        string rep = "LOGIN|SUCCESS|";
        rep.append(user.nickname);
        const char* reply = rep.data();
            write(socket, reply, strlen(reply));
    }
    else
    {
            const char* reply = "LOGIN|FAILED|";
            write(socket, reply, strlen(reply));
    }
}
void CServer::reg(User user,int socket)
{
    database->InsertData(user);        //添加判断是否成功加入
    char* reply = "REG|SUCCESS|";
    write(socket, reply, strlen(reply));
}
int CServer::handleRequest(const char* msg, vector<string> &splitMsg)
{
    string rawmMsg = msg;
    Split(msg, "|", splitMsg);

//对消息进行分类
    if (splitMsg[0] == "MSG") return MSG;
    else if (splitMsg[0] == "REG") return REG;
    else if ((splitMsg[0] == "LOGIN"))return LOGIN;

}
void CServer::errorHandling(const char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    printf("errno=%d\n", errno);
    char* mesg = strerror(errno);
    printf("Mesg:%s\n", mesg);
    exit(1);
}

CServer::CServer()
{
    initialServer();
}

CServer::~CServer()
{
    delete database;
}

int CServer::initialServer()
{
    servSock = socket(PF_INET, SOCK_STREAM, 0);     //分配一个套接字

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(1234);

    if (bind(servSock, (struct sockaddr*) & servAddr, sizeof(servAddr)) == -1)
        errorHandling("bind() error");

    if (listen(servSock, 5) == -1)
        errorHandling("listen() error");

    clntCnt = 0;
    database = new CDataBaseManage();

    return 0;
}

int CServer::startServer()
{
    database->ConnectDatabase("imusers");
    for( ; ; )
    {
        clntAddrLen = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*) & clntAddr, &clntAddrLen);

        socketsMutx.lock();
            clntSocks[clntCnt++] = clntSock; //写入新socket连接
        socketsMutx.unlock();

        thread th(&CServer::handleClnt, this, clntSock);  //创建线程为新客户端服务，并且把clnt_sock作为参数传递
        th.detach();


        printf("Connected client IP: %s \n", inet_ntoa(clntAddr.sin_addr)); //客户端连接的ip地址
    }
}

int CServer::addCnnect()
{
    return 0;
}
void CServer::Split(const string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    dest.clear();
    index = str.find_first_of(separator, start);
    do
    {
        if (index != string::npos)
        {
            substring = str.substr(start, index - start);
            dest.push_back(substring);
            start = index + separator.size();
            index = str.find(separator, start);
            if (start == string::npos) break;
        }
    } while (index != string::npos);

    //the last part
    substring = str.substr(start);
    dest.push_back(substring);
}