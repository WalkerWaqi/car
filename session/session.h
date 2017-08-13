//
// Created by waqi on 12/6/16.
//

#ifndef CAR_SESSION_H
#define CAR_SESSION_H

#include <list>
#include "allocator.h"
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
#include "logger.h"

namespace agv {
    class session;

    typedef boost::shared_ptr<session> session_ptr;
    typedef std::list<session_ptr> SESSION_CONTAINER;
    typedef boost::shared_ptr<std::list<session_ptr> > PTRSESSION_CONTAINER;
    typedef boost::weak_ptr<std::list<session_ptr> > WPTRSESSION_CONTAINER;

    class session
            : public boost::enable_shared_from_this<session> {
    public:
        session(boost::asio::io_service &work_service, boost::asio::io_service &io_service,
                PTRSESSION_CONTAINER &pSessions, boost::mutex &mSessions, bool inContainer = true)
                : socket_(io_service), io_work_service(work_service), strand_(io_service), ptrSessions(pSessions),
                  mutexSessions(mSessions), in_container(inContainer) {
            LOG_SESSION_INFO("begin");
            LOG_SESSION_INFO("end");
        }

        virtual ~session() {
            LOG_SESSION_INFO("begin");
            LOG_SESSION_INFO("end");
        }

        inline void deleteMe() {
            LOG_SESSION_INFO("begin");

            {
                boost::mutex::scoped_lock lock(writeConditionMutex);
                writeCondition.notify_one();
            }

            if (in_container) {
                boost::mutex::scoped_lock lock(mutexSessions);

                PTRSESSION_CONTAINER pSessions = ptrSessions.lock();
                if (pSessions) {
                    pSessions->remove(shared_from_this());
                }

                in_container = false;
            }

            LOG_SESSION_INFO("end");
        }

        void close() {
            LOG_SESSION_INFO("begin");

            {
                boost::mutex::scoped_lock lock(socketMutex_);
                if(socket_.is_open())
                {
                    LOG_SESSION_INFO("socket is open");

                    socket_.close();
                }
            }

            LOG_SESSION_INFO("end");
        }

        tcp::socket &socket() {
            return socket_;
        }

        std::string get_remote_addr() {
            return std::string(socket_.remote_endpoint().address().to_string());
        }

        unsigned short get_remote_port() {
            return socket_.remote_endpoint().port();
        }

        std::string get_peer_name() {
            return std::string(socket_.remote_endpoint().address().to_string());
        }

        std::string get_local_addr() {
            return std::string(socket_.local_endpoint().address().to_string());
        }

        unsigned short get_local_port() {
            return socket_.local_endpoint().port();
        }

        std::string get_host_name() {
            return boost::asio::ip::host_name();
        }

        void set_no_delay(bool noDelay) {
            socket_.set_option(boost::asio::ip::tcp::no_delay(noDelay));
        }

        void start(unsigned int timeout = 0) {
            LOG_SESSION_INFO("begin");

            if (timeout)
            {
                add_timer(timeout);
            }

            socket_.async_read_some(recvBuf.prepare(max_length),
                                    make_custom_alloc_handler(allocator_,
                                                              strand_.wrap(boost::bind(&session::handle_read,
                                                                                       shared_from_this(),
                                                                                       boost::asio::placeholders::error,
                                                                                       boost::asio::placeholders::bytes_transferred))));

            LOG_SESSION_INFO("end");
        }

        void handle_read(const boost::system::error_code &error,
                         size_t bytes_transferred) {
            LOG_SESSION_INFO("begin error = " << error << " bytes_transferred = " << bytes_transferred);

            if (!error) {
                recvBuf.commit(bytes_transferred);
                LOG_SESSION_INFO("recvBuf = " << recvBuf.size());

                while(recvBuf.size())
                {
                    int packetSize = check_packet(recvBuf);
                    LOG_SESSION_INFO("packetSize = " << packetSize);

                    if (packetSize > 0) {
                        const char *pBuf = boost::asio::buffer_cast<const char *>(recvBuf.data());

                        boost::shared_ptr<std::vector<char> > buf(new std::vector<char>);
                        buf->resize(packetSize);
                        std::copy(pBuf, pBuf + packetSize, buf->begin());

                        io_work_service.post(boost::bind(&session::on_receive, shared_from_this(), buf, packetSize));

                        recvBuf.consume(packetSize);
                    } else if (packetSize < 0) {
                        LOG_SESSION_INFO("packetSize = " << packetSize << " recvBuf = " << recvBuf.size());

                        if (packetSize < -1)
                        {
                            LOG_SESSION_ERROR("packetSize = " << packetSize << " recvBuf = " << recvBuf.size());

                            close();
                            deleteMe();
                        }

                        break;
                    }
                }

                socket_.async_read_some(recvBuf.prepare(max_length),
                                        make_custom_alloc_handler(allocator_,
                                                                  strand_.wrap(boost::bind(&session::handle_read,
                                                                                           shared_from_this(),
                                                                                           boost::asio::placeholders::error,
                                                                                           boost::asio::placeholders::bytes_transferred))));
            } else {
                LOG_SESSION_ERROR("error = " << error);

                deleteMe();
            }

            LOG_SESSION_INFO("end");
        }

/*    void handle_write(const boost::system::error_code& error)
    {
        LOG_SESSION_INFO("begin error = " << error);

        if (!error)
        {
            ptrSendBuf.reset();

        }
        else
        {
            LOG_SESSION_ERROR("error = " << error);

            close();
        }

        LOG_SESSION_INFO("end");
    }*/

        void handle_timeout(const boost::system::error_code &error) {
            LOG_SESSION_INFO("begin error = " << error);

            if (!error) {
                LOG_SESSION_ERROR("error = " << error);

                close();
            }

            LOG_SESSION_INFO("end");
        }

        int send(char *buf, unsigned int size, unsigned int timeout = 0) {
            LOG_SESSION_INFO("begin size = " << size << " timeout = " << timeout);

            boost::mutex::scoped_lock lock(writeConditionMutex);

            boost::asio::deadline_timer t(socket_.get_io_service(), boost::posix_time::milliseconds(timeout));
            if (timeout) {
                t.async_wait(boost::bind(&session::handle_timeout,
                                         shared_from_this(),
                                         boost::asio::placeholders::error));
            }

            socket_.async_write_some(boost::asio::buffer(buf, size),
                                     strand_.wrap(boost::bind(&session::handle_write,
                                                              shared_from_this(),
                                                              boost::asio::placeholders::error,
                                                              boost::asio::placeholders::bytes_transferred)));

            writeCondition.wait(lock);

            if (timeout) {
                t.cancel();
            }

            if (write_error_) {
                LOG_SESSION_ERROR("end write_error_ = " << write_error_);

                return -1;
            }

            LOG_SESSION_INFO("end write_size_ = " << write_size_);

            return write_size_;
        }

        void handle_write(const boost::system::error_code &error,
                          size_t bytes_transferred) {
            LOG_SESSION_INFO("begin error = " << error << " bytes_transferred = " << bytes_transferred);

            write_error_ = error;
            write_size_ = bytes_transferred;

            if (!error) {
            } else {
                LOG_SESSION_ERROR("error = " << error);

                close();
            }

            {
                boost::mutex::scoped_lock lock(writeConditionMutex);

                writeCondition.notify_one();
            }

            LOG_SESSION_INFO("end");
        }

        virtual int check_packet(boost::asio::streambuf &buffer) = 0;

        virtual void on_receive(boost::shared_ptr<std::vector<char> > buffer, size_t bytes_transferred) = 0;

        void add_timer(unsigned int timeout);
        void update_timer(unsigned int timeout);
        void remove_timer();

    protected:
        // The io_service used to finish the work.
        boost::asio::io_service &io_work_service;
    private:
        // The socket used to communicate with the client.
        tcp::socket socket_;
        boost::mutex socketMutex_;

        // Buffer used to store data received from the client.
        enum {
            max_length = 1024
        };
        boost::asio::streambuf recvBuf;
        boost::shared_ptr<boost::asio::streambuf> ptrSendBuf;

        // The allocator to use for handler-based custom memory allocation.
        handler_allocator allocator_;

        //
        boost::asio::io_service::strand strand_;

        WPTRSESSION_CONTAINER ptrSessions;
        boost::mutex &mutexSessions;

        boost::mutex writeConditionMutex;
        boost::condition_variable writeCondition;
        boost::system::error_code write_error_;
        size_t write_size_;

        bool in_container;
    };
}

#endif //CAR_SESSION_H
