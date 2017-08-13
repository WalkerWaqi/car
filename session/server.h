#ifndef ASERVER_H
#define ASERVER_H

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include "allocator.h"
#include "packet_session.h"
#include "modbus_session.h"
#include "logger.h"
#include "io_service_pool.hpp"
#include "car.h"
#include "common.h"

using boost::asio::ip::tcp;
using namespace agv;

class server
{
public:
	server(short port, std::size_t io_service_pool_size) 
		: io_service_pool_(io_service_pool_size) 
		, io_service_work_pool_(io_service_pool_size) 
		, acceptor_(io_service_pool_.get_io_service(), tcp::endpoint(tcp::v4(), port))
        , port_(port)
	{
        LOG_SESSION_INFO("begin port_ = " << port_ << " io_service_pool_size = " << io_service_pool_size);

		ptrSessions = PTRSESSION_CONTAINER(new SESSION_CONTAINER);

        session_ptr new_session;
        if (port_ == g_ModbusPort)
        {
            new_session.reset(new modbus_session(io_service_work_pool_.get_io_service()
                    , io_service_pool_.get_io_service()
                    , ptrSessions
                    , mutexSessions));
        }
        else
        {
            new_session.reset(new packet_session(io_service_work_pool_.get_io_service()
                    , io_service_pool_.get_io_service()
                    , ptrSessions
                    , mutexSessions));
        }
		acceptor_.async_accept(new_session->socket(), 
			boost::bind(&server::handle_accept, this, new_session, 
			boost::asio::placeholders::error));

        LOG_SESSION_INFO("end");
	} 

	~server()
	{
        LOG_SESSION_INFO("begin");

		stop();

		boost::mutex::scoped_lock lock(mutexSessions);

		ptrSessions.reset();

        LOG_SESSION_INFO("end");
	}

	//void add_timer(boost::shared_ptr<session> new_session);
	//void update_timer(boost::shared_ptr<session> new_session);

	void handle_accept(session_ptr new_session,
		const boost::system::error_code& error)
	{
        LOG_SESSION_INFO("begin error = " << error);

		if (!error) 
		{ 
			{
				boost::mutex::scoped_lock lock(mutexSessions);

				ptrSessions->push_back(new_session);
			}

			if (port_ == g_ModbusPort)
            {
				new_session->start();

                new_session.reset(new modbus_session(io_service_work_pool_.get_io_service()
                        , io_service_pool_.get_io_service()
                        , ptrSessions
                        , mutexSessions));
            }
            else
            {
				new_session->start(CONNECTION_TIMEOUT);

				new_session.reset(new packet_session(io_service_work_pool_.get_io_service()
                        , io_service_pool_.get_io_service()
                        , ptrSessions
                        , mutexSessions));
            }

            acceptor_.async_accept(new_session->socket(),
				boost::bind(&server::handle_accept, this, new_session, 
				boost::asio::placeholders::error)); 
		}
        else
        {
            LOG_SESSION_ERROR("error =  "<< error)
        }

        LOG_SESSION_INFO("end");
	} 

	void run() 
	{
        LOG_SESSION_INFO("begin");

		io_thread_.reset(new boost::thread(boost::bind(&io_service_pool::run
			, &io_service_pool_))); 
		work_thread_.reset(new boost::thread(boost::bind(&io_service_pool::run
			, &io_service_work_pool_)));

        LOG_SESSION_INFO("end");
	} 

	void stop() 
	{
        LOG_SESSION_INFO("begin");

		io_service_pool_.stop(); 
		io_service_work_pool_.stop(); 

		io_thread_->join(); 
		work_thread_->join();

        LOG_SESSION_INFO("end");
	} 

private: 
	boost::shared_ptr<boost::thread> io_thread_; 
	boost::shared_ptr<boost::thread> work_thread_; 
	io_service_pool io_service_pool_; 
	io_service_pool io_service_work_pool_; 
	tcp::acceptor acceptor_; 

	PTRSESSION_CONTAINER    ptrSessions;
    boost::mutex            mutexSessions;

    unsigned short          port_;
};

#endif // ASERVER_H
