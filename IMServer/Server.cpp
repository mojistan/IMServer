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
        int type =  handleRequest(msg,splitMsg);     //��ȡ��Ϣ����
        switch (type)    //������Ϣ���ͷ��ദ��
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
    //���յ���ϢΪ0ʱ������ǰ�ͻ����Ѿ��Ͽ�����
    socketsMutx.lock();
    for (i = 0; i < clntCnt; i++) //ɾ��û�����ӵĿͻ���
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
void CServer::sendMsg(const char* msg, long int len) //�����ӵ����пͻ��˷�����Ϣ
{
    int i;
    socketsMutx.lock();
    for (i = 0; i < clntCnt; i++)
        write(clntSocks[i], msg, len);
    socketsMutx.unlock();
}
void CServer::logIn(User user,int socket)
{
    if (database->SerchUser("accounts", user))       //����ж��Ƿ�ɹ���½
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
    if (database->InsertData(user))        //����ж��Ƿ�ɹ�ע�����
    {
        char* reply = "REG|SUCCESS|";
        write(socket, reply, strlen(reply));
    }
}
int CServer::handleRequest(const char* msg, vector<string> &splitMsg)
{
    string rawmMsg = msg;
    Split(msg, "|", splitMsg);

//����Ϣ���з���
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
    servSock = socket(PF_INET, SOCK_STREAM, 0);     //����һ���׽���

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
//��������Ҫѭ���������������
int CServer::startServer()
{
    //ʹ��epollʵ�ָ���
    database->ConnectDatabase("imusers");
    for( ; ; )
    {
        //�޸�ʹ��epoll����
        clntAddrLen = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*) & clntAddr, &clntAddrLen);

        socketsMutx.lock();
            clntSocks[clntCnt++] = clntSock; //д����socket����
        socketsMutx.unlock();

        thread th(&CServer::handleClnt, this, clntSock);  //�����߳�Ϊ�¿ͻ��˷��񣬲��Ұ�clnt_sock��Ϊ��������
        th.detach();


        printf("Connected client IP: %s \n", inet_ntoa(clntAddr.sin_addr)); //�ͻ������ӵ�ip��ַ
    }
}

int CServer::addCnnect()
{
    return 0;
}

void CServer::Split(const string& src, const string& separator, vector<string>& dest)
{
    //����separator�ָ������ַ����ָ���
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