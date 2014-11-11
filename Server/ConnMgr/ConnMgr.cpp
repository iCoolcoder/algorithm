/*
����һ�������õķ�������ֻ���������ܣ�
1������ÿ�������ӿͻ��ˣ�ÿ10�����䷢��һ��hello, world
2����ͻ�����������������ݣ��������յ����ٽ����ݻط����ͻ���
*/
//test.cpp
#include "TcpEventServer.h"
#include <set>
#include <vector>
using namespace std;

//����ʾ��
class TestServer : public TcpEventServer
{
private:
	vector<Conn*> vec;
protected:
	//���ظ�������ҵ����麯��
	void ReadEvent(Conn *conn);
	void WriteEvent(Conn *conn);
	void ConnectionEvent(Conn *conn);
	void CloseEvent(Conn *conn, short events);
public:
	TestServer(int count) : TcpEventServer(count) { }
	~TestServer() { }

	//�˳��¼�����ӦCtrl+C
	static void QuitCb(int sig, short events, void *data);
	//��ʱ���¼���ÿ10�������пͻ��˷�һ��hello, world
	static void TimeOutCb(int id, int short events, void *data);
};

void TestServer::ReadEvent(Conn *conn)
{
	conn->MoveBufferData();
}

void TestServer::WriteEvent(Conn *conn)
{

}

void TestServer::ConnectionEvent(Conn *conn)
{
	TestServer *me = (TestServer*)conn->GetThread()->tcpConnect;
	printf("new connection: %d\n", conn->GetFd());
	me->vec.push_back(conn);
}

void TestServer::CloseEvent(Conn *conn, short events)
{
	printf("connection closed: %d\n", conn->GetFd());
}

void TestServer::QuitCb(int sig, short events, void *data)
{
	printf("Catch the SIGINT signal, quit in one second\n");
	TestServer *me = (TestServer*)data;
	timeval tv = { 1, 0 };
	me->StopRun(&tv);
}

void TestServer::TimeOutCb(int id, short events, void *data)
{
	TestServer *me = (TestServer*)data;
	char temp[33] = "hello, world\n";
	for (int i = 0; i<me->vec.size(); i++)
		me->vec[i]->AddToWriteBuffer(temp, strlen(temp));
}

int main()
{
	printf("pid: %d\n", getpid());
	TestServer server(3);
	server.AddSignalEvent(SIGINT, TestServer::QuitCb);
	timeval tv = { 10, 0 };
	server.AddTimerEvent(TestServer::TimeOutCb, tv, false);
	server.SetPort(2111);
	server.StartRun();
	printf("done\n");

	return 0;
}
