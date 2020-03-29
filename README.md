# IMServer
## 简介
服务器基于linux，数据库使用MySQL，客户端采用Qt开发，在IMClient
***
### 2020/3/28
当前master采用每个链接一个线程的方法处理，拟引入epoll
### 2020/3/29
在分支epoll中测试修改为epoll测试成功，
