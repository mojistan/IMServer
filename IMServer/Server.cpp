#include "Server.h"

#define BUF_SIZE 1024
#define MAX_CLNT 256
#define EPOLL_SIZE 50
void CServer::handleClnt()
{
    long int str_len = 0;
    char msg[BUF_SIZE];
    int epCount = 0;
    while (1)
    {
        epCount = epoll_wait(epFd, epEvents, EPOLL_SIZE, NULL);
        for (int i = 0; i < epCount; i++)
        {
            int curSock = epEvents[i].data.fd;
            str_len = recv(curSock, msg, sizeof(msg),0);
            if (str_len == 0)           //���յ���ϢΪ0ʱ������ǰ�ͻ����Ѿ��Ͽ�����
            {
                socketsMutx.lock();
                epoll_ctl(epFd, EPOLL_CTL_DEL, curSock, NULL);
                for (i = 0; i < clntCnt; i++) //ɾ��û�����ӵĿͻ���
                {
                    if (curSock == clntSocks[i])
                    {
                        while (i++ < clntCnt - 1)
                            clntSocks[i] = clntSocks[i + 1];
                        break;
                    }
                }
                clntCnt--;
                socketsMutx.unlock();
                close(curSock);
                printf("socket : %d closed\n", curSock);
            }
            else if (str_len < 0)
            {
                errorHandling("recv() error");
            }
            else {
                vector<string> splitMsg;
                int type = handleRequest(msg, splitMsg);     //��ȡ��Ϣ����
                switch (type)    //������Ϣ���ͷ��ദ��
                {
                case MSG:
                    sendMsg(msg, strlen(msg));
                    break;
                case REG:
                    reg(User(splitMsg[1].data(), splitMsg[2].data(), splitMsg[3].data()), curSock);
                    break;
                case LOGIN:
                    logIn(User(splitMsg[1].data(), splitMsg[2].data()), curSock);
                    break;
                default:
                    break;
                }
            }
        }
    }
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
//��������ѭ���������������
int CServer::startServer()
{
    //ʹ��epollʵ�ָ���
    database->ConnectDatabase("imusers");
    epFd = epoll_create(MAX_CLNT);
    epEvents = new epoll_event[MAX_CLNT];
    thread th(&CServer::handleClnt, this);  //�����߳�Ϊ�¿ͻ��˷��񣬲��Ұ�clnt_sock��Ϊ��������
    th.detach();

    for( ; ; )
    {
        clntAddrLen = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*) & clntAddr, &clntAddrLen);


        socketsMutx.lock();
            epEvent.events = EPOLLIN;
            epEvent.data.fd = clntSock;
            epoll_ctl(epFd, EPOLL_CTL_ADD, clntSock, &epEvent);
            clntSocks[clntCnt++] = clntSock; //д����socket����
        socketsMutx.unlock();

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