#include "TcpEventServer.h"

Conn::Conn(int fd) : m_fd(fd)
{
	m_Prev = NULL;
	m_Next = NULL;
}

Conn::~Conn()
{

}

ConnQueue::ConnQueue()
{
	//����ͷβ��㣬��������ָ��
	m_head = new Conn(0);
	m_tail = new Conn(0);
	m_head->m_Prev = m_tail->m_Next = NULL;
	m_head->m_Next = m_tail;
	m_tail->m_Prev = m_head;
}

ConnQueue::~ConnQueue()
{
	Conn *tcur, *tnext;
	tcur = m_head;
	//ѭ��ɾ�������еĸ������
	while (tcur != NULL)
	{
		tnext = tcur->m_Next;
		delete tcur;
		tcur = tnext;
	}
}

Conn *ConnQueue::InsertConn(int fd, LibeventThread *t)
{
	Conn *c = new Conn(fd);
	c->m_Thread = t;
	Conn *next = m_head->m_Next;

	c->m_Prev = m_head;
	c->m_Next = m_head->m_Next;
	m_head->m_Next = c;
	next->m_Prev = c;
	return c;
}

void ConnQueue::DeleteConn(Conn *c)
{
	c->m_Prev->m_Next = c->m_Next;
	c->m_Next->m_Prev = c->m_Prev;
	delete c;
}

/*
void ConnQueue::PrintQueue()
{
Conn *cur = m_head->m_Next;
while( cur->m_Next != NULL )
{
printf("%d ", cur->m_fd);
cur = cur->m_Next;
}
printf("\n");
}
*/

TcpEventServer::TcpEventServer(int count)
{
	//��ʼ����������
	m_ThreadCount = count;
	m_Port = -1;
	m_MainBase = new LibeventThread;
	m_Threads = new LibeventThread[m_ThreadCount];
	m_MainBase->tid = pthread_self();
	m_MainBase->base = event_base_new();

	//��ʼ���������̵߳Ľṹ��
	for (int i = 0; i<m_ThreadCount; i++)
	{
		SetupThread(&m_Threads[i]);
	}

}

TcpEventServer::~TcpEventServer()
{
	//ֹͣ�¼�ѭ��������¼�ѭ��û��ʼ����ûЧ����
	StopRun(NULL);

	//�ͷ��ڴ�
	event_base_free(m_MainBase->base);
	for (int i = 0; i<m_ThreadCount; i++)
		event_base_free(m_Threads[i].base);

	delete m_MainBase;
	delete[] m_Threads;
}

void TcpEventServer::ErrorQuit(const char *str)
{
	//���������Ϣ���˳�����
	fprintf(stderr, "%s", str);
	if (errno != 0)
		fprintf(stderr, " : %s", strerror(errno));
	fprintf(stderr, "\n");
	exit(1);
}

void TcpEventServer::SetupThread(LibeventThread *me)
{
	//����libevent�¼��������
	me->tcpConnect = this;
	me->base = event_base_new();
	if (NULL == me->base)
		ErrorQuit("event base new error");

	//�����̺߳����߳�֮�佨���ܵ�
	int fds[2];
	if (pipe(fds))
		ErrorQuit("create pipe error");
	me->notifyReceiveFd = fds[0];
	me->notifySendFd = fds[1];

	//�����̵߳�״̬�������ܵ�
	event_set(&me->notifyEvent, me->notifyReceiveFd,
		EV_READ | EV_PERSIST, ThreadProcess, me);
	event_base_set(me->base, &me->notifyEvent);
	if (event_add(&me->notifyEvent, 0) == -1)
		ErrorQuit("Can't monitor libevent notify pipe\n");
}

void *TcpEventServer::WorkerLibevent(void *arg)
{
	//����libevent���¼�ѭ����׼������ҵ��
	LibeventThread *me = (LibeventThread*)arg;
	//printf("thread %u started\n", (unsigned int)me->tid);
	event_base_dispatch(me->base);
	//printf("subthread done\n");
}

bool TcpEventServer::StartRun()
{
	evconnlistener *listener;

	//����˿ںŲ���EXIT_CODE���ͼ����ö˿ں�
	if (m_Port != EXIT_CODE)
	{
		sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(m_Port);
		listener = evconnlistener_new_bind(m_MainBase->base,
			ListenerEventCb, (void*)this,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
			(sockaddr*)&sin, sizeof(sockaddr_in));
		if (NULL == listener)
			ErrorQuit("TCP listen error");
	}

	//����������߳�
	for (int i = 0; i<m_ThreadCount; i++)
	{
		pthread_create(&m_Threads[i].tid, NULL,
			WorkerLibevent, (void*)&m_Threads[i]);
	}

	//�������̵߳��¼�ѭ��
	event_base_dispatch(m_MainBase->base);

	//�¼�ѭ��������ͷż����ߵ��ڴ�
	if (m_Port != EXIT_CODE)
	{
		//printf("free listen\n");
		evconnlistener_free(listener);
	}
}

void TcpEventServer::StopRun(timeval *tv)
{
	int contant = EXIT_CODE;
	//��������̵߳Ĺ�����д��EXIT_CODE��֪ͨ�����˳�
	for (int i = 0; i<m_ThreadCount; i++)
	{
		write(m_Threads[i].notifySendFd, &contant, sizeof(int));
	}
	//������̵߳��¼�ѭ��
	event_base_loopexit(m_MainBase->base, tv);
}

void TcpEventServer::ListenerEventCb(struct evconnlistener *listener,
	evutil_socket_t fd,
struct sockaddr *sa,
	int socklen,
	void *user_data)
{
	TcpEventServer *server = (TcpEventServer*)user_data;

	//���ѡ��һ�����̣߳�ͨ���ܵ����䴫��socket������
	int num = rand() % server->m_ThreadCount;
	int sendfd = server->m_Threads[num].notifySendFd;
	write(sendfd, &fd, sizeof(evutil_socket_t));
}

void TcpEventServer::ThreadProcess(int fd, short which, void *arg)
{
	LibeventThread *me = (LibeventThread*)arg;

	//�ӹܵ��ж�ȡ���ݣ�socket��������������룩
	int pipefd = me->notifyReceiveFd;
	evutil_socket_t confd;
	read(pipefd, &confd, sizeof(evutil_socket_t));

	//�����������EXIT_CODE���������¼�ѭ��
	if (EXIT_CODE == confd)
	{
		event_base_loopbreak(me->base);
		return;
	}

	//�½�����
	struct bufferevent *bev;
	bev = bufferevent_socket_new(me->base, confd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(me->base);
		return;
	}

	//�������ӷ������
	Conn *conn = me->connectQueue.InsertConn(confd, me);

	//׼����socket�ж�д����
	bufferevent_setcb(bev, ReadEventCb, WriteEventCb, CloseEventCb, conn);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	//�����û��Զ���������¼��������
	me->tcpConnect->ConnectionEvent(conn);
}

void TcpEventServer::ReadEventCb(struct bufferevent *bev, void *data)
{
	Conn *conn = (Conn*)data;
	conn->m_ReadBuf = bufferevent_get_input(bev);
	conn->m_WriteBuf = bufferevent_get_output(bev);

	//�����û��Զ���Ķ�ȡ�¼��������
	conn->m_Thread->tcpConnect->ReadEvent(conn);
}

void TcpEventServer::WriteEventCb(struct bufferevent *bev, void *data)
{
	Conn *conn = (Conn*)data;
	conn->m_ReadBuf = bufferevent_get_input(bev);
	conn->m_WriteBuf = bufferevent_get_output(bev);

	//�����û��Զ����д���¼��������
	conn->m_Thread->tcpConnect->WriteEvent(conn);

}

void TcpEventServer::CloseEventCb(struct bufferevent *bev, short events, void *data)
{
	Conn *conn = (Conn*)data;
	//�����û��Զ���ĶϿ��¼��������
	conn->m_Thread->tcpConnect->CloseEvent(conn, events);
	conn->GetThread()->connectQueue.DeleteConn(conn);
	bufferevent_free(bev);
}

bool TcpEventServer::AddSignalEvent(int sig, void(*ptr)(int, short, void*))
{
	//�½�һ���ź��¼�
	event *ev = evsignal_new(m_MainBase->base, sig, ptr, (void*)this);
	if (!ev ||
		event_add(ev, NULL) < 0)
	{
		event_del(ev);
		return false;
	}

	//ɾ���ɵ��ź��¼���ͬһ���ź�ֻ����һ���ź��¼���
	DeleteSignalEvent(sig);
	m_SignalEvents[sig] = ev;

	return true;
}

bool TcpEventServer::DeleteSignalEvent(int sig)
{
	map<int, event*>::iterator iter = m_SignalEvents.find(sig);
	if (iter == m_SignalEvents.end())
		return false;

	event_del(iter->second);
	m_SignalEvents.erase(iter);
	return true;
}

event *TcpEventServer::AddTimerEvent(void(*ptr)(int, short, void *),
	timeval tv, bool once)
{
	int flag = 0;
	if (!once)
		flag = EV_PERSIST;

	//�½���ʱ���ź��¼�
	event *ev = new event;
	event_assign(ev, m_MainBase->base, -1, flag, ptr, (void*)this);
	if (event_add(ev, &tv) < 0)
	{
		event_del(ev);
		return NULL;
	}
	return ev;
}

bool TcpEventServer::DeleteTImerEvent(event *ev)
{
	int res = event_del(ev);
	return (0 == res);
}
