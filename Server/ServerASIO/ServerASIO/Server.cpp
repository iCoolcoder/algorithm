#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "lruCache.h"

using boost::asio::ip::tcp;
#define max_len 260

typedef boost::unique_lock<boost::shared_mutex> mutex_lock;

std::string fn_(const std::string &str)
{
	return boost::move(str);
}

lru_cache_using_std<std::string, std::string> lru_cache(fn_, 10000);
class statistics
{
public:
	static void addone()
	{
		mutex_lock lock(m_mutex);
		m_count++;
		std::cout << m_count << std::endl;
	}
	static void minsone()
	{
		mutex_lock lock(m_mutex);
		m_count--;
		std::cout << m_count << std::endl;
	}
	static size_t m_count;
	static boost::shared_mutex m_mutex;
};
size_t statistics::m_count = 0;
boost::shared_mutex statistics::m_mutex;

class clientSession
	: public boost::enable_shared_from_this<clientSession>
{
public:
	clientSession(boost::asio::io_service& iosev)
		: m_socket(iosev)
	{
		memset(m_data, '\0', sizeof(m_data));
	}

	~clientSession() {}
	
	void start()
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer("link successfully"),
			boost::bind(&clientSession::handle_write,shared_from_this(),
			boost::asio::placeholders::error));

		m_socket.async_read_some(boost::asio::buffer(m_data, max_len),
			boost::bind(&clientSession::handle_read, shared_from_this(),
			boost::asio::placeholders::error));
	}
	tcp::socket& socket()
	{
		return m_socket;
	}
private:
	void handle_write(const boost::system::error_code& error)
	{
		if(error)
		{
			m_socket.close();
			statistics::minsone();
		}

	}
	void handle_read(const boost::system::error_code& error)
	{
		if(!error)
		{
			lru_cache.operator()(std::string(m_data));

			memset(m_data, '\0', sizeof(m_data));
			m_socket.async_read_some(boost::asio::buffer(m_data, max_len),
				boost::bind(&clientSession::handle_read, shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			m_socket.close();
			statistics::minsone();
		}
	}

private:
	tcp::socket m_socket;
	char		m_data[max_len];
};

class serverApp
{
	typedef boost::shared_ptr<clientSession> session_ptr;
public:
	serverApp(boost::asio::io_service& ioservice, tcp::endpoint& endpoint)
		: m_ioservice(ioservice),
		m_acceptor(ioservice,endpoint)
	{
		session_ptr new_session(new clientSession(ioservice));
		m_acceptor.async_accept(new_session->socket(),
			boost::bind(&serverApp::handle_accept, this, boost::asio::placeholders::error,
			new_session));
	}
	~serverApp() { }
	void handle_accept(const boost::system::error_code& error, session_ptr& session)
	{
		if(!error)
		{
			std::cout << "get a new client!" << std::endl;
			statistics::addone();

			// handle the data for each client
			session->start();

			// now you know why we will encapsulate session class for each client
			session_ptr new_session(new clientSession(m_ioservice));
			m_acceptor.async_accept(new_session->socket(),
				boost::bind(&serverApp::handle_accept,this,boost::asio::placeholders::error,
				new_session));
		}
	}
private:
	boost::asio::io_service& m_ioservice;
	tcp::acceptor m_acceptor;
};

int main()
{
	boost::asio::io_service myIoService;
	short port = 8100/*argv[1]*/;
	//inet4
	tcp::endpoint endPoint(tcp::v4(),port);
	serverApp sa(myIoService,endPoint);
	myIoService.run();
	return 0;
}

