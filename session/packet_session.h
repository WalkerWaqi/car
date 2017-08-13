//
// Created by waqi on 12/6/16.
//

#ifndef CAR_MANAGE_SESSION_H
#define CAR_MANAGE_SESSION_H


#include "session.h"
#include "commhead.h"
#include "PacketProcesser.h"
#include "RawProcesser.h"
#include "Command.h"
#include "common.h"


namespace agv {
    class packet_session : public session {
    public:
        using session::session;

        virtual ~packet_session() {
            LOG_SESSION_INFO("begin");

            LOG_SESSION_INFO("end");
        }

        virtual int check_packet(boost::asio::streambuf &buffer) override {
            LOG_SESSION_INFO("begin buffer size = " << buffer.size());

            int ret = -2;

            try {
                do {
                    if (buffer.size() >= sizeof(COMM_HEAD)) {
                        const COMM_HEAD *pHead = boost::asio::buffer_cast<const COMM_HEAD *>(buffer.data());
                        if (ntohl(pHead->size) < sizeof(COMM_HEAD)) {
                            LOG_SESSION_ERROR("packet: error size: " << ntohl(pHead->size));

                            break;
                        } else {
                            int version = pHead->version;
                            int channel = pHead->channel;
                            LOG_SESSION_INFO("packet: version = " << version << " channel = " << channel << " id = " << ntohs(pHead->id) << " size = " << ntohl(pHead->size));
                        }

                        if (buffer.size() >= ntohl(pHead->size)) {
                            ret = ntohl(pHead->size);

                            LOG_SESSION_INFO("packet: enough size = " << ret);

                            if (pHead->channel <= CHANNEL_MANAGE) {
                                boost::shared_ptr<PacketProcesser> ptrPacket(new PacketProcesser((const char*)(pHead + 1), ret - sizeof(COMM_HEAD), pHead->version, pHead->channel, ntohs(pHead->id)));
                                if(!ptrPacket)
                                {
                                    LOG_SESSION_ERROR("new");

                                    throw std::runtime_error("new");
                                }

                                io_work_service.post(boost::bind(&packet_session::on_packet, boost::dynamic_pointer_cast<packet_session>(shared_from_this()), ptrPacket));

                                buffer.consume(ret);

                                ret = 0;
                            }
                        } else {
                            LOG_SESSION_INFO("buffer.size() < ntohl(pHead->size)");

                            ret = -1;
                        }
                    } else {
                        LOG_SESSION_INFO("buffer.size() < sizeof(COMM_HEAD)");

                        ret = -1;
                    }
                } while (0);
            }
                /*catch (std::exception& e)
                {
                    LOG_SESSION_ERROR("std error: " << e.what());
                }
                catch (boost::system::system_error& e)
                {
                    LOG_SESSION_ERROR("boost error: " << e.what());
                }*/
            catch (...) {
                LOG_SESSION_ERROR("crash");
            }

            LOG_SESSION_INFO("end ret = " << ret);

            return ret;
        }

        virtual void on_receive(boost::shared_ptr<std::vector<char> > buffer, size_t bytes_transferred) override {
            LOG_SESSION_INFO("begin buffer->size = " << buffer->size() << " bytes_transferred = " << bytes_transferred);

            if (buffer && bytes_transferred >= (sizeof(COMM_HEAD) + sizeof(COMM_BODY))) {
                boost::shared_ptr<RawProcesser> ptrRaw(new RawProcesser(buffer));
                if(!ptrRaw)
                {
                    LOG_SESSION_ERROR("new");

                    throw std::runtime_error("new");
                }

                unsigned int command = ptrRaw->getCommand();
                LOG_SESSION_INFO("packet: command = " << command);

                if (isWeakup() || command == COMMAND_WEAKUP)
                {
                    boost::shared_ptr<Command> ptrCommand = ptrRaw->CreateCommand();
                    if(ptrCommand)
                    {
                        if (ptrCommand->Exec(shared_from_this()) >= 0)
                        {
                        }
                        else
                        {
                            LOG_SESSION_ERROR("exec error");
                        }
                    }
                    else
                    {
                        LOG_SESSION_ERROR("command = " << command);
                    }
                } else
                {
                    LOG_SESSION_ERROR("session not weakup");
                }
            } else {
                LOG_SESSION_ERROR("no data");
            }

            LOG_SESSION_INFO("end");
        }

        virtual void on_packet(boost::shared_ptr<PacketProcesser> ptrPacket) {
            LOG_SESSION_INFO("begin");

            if (ptrPacket) {
                unsigned short id = ptrPacket->getPacket().id();
                CommandType command = ptrPacket->getPacket().command();

                LOG_SESSION_INFO("packet: id = " << id << " command = " << command);

                if (isWeakup() || command == WAKEUP)
                {
                    boost::shared_ptr<Command> ptrCommand = ptrPacket->CreateCommand();
                    if(ptrCommand)
                    {
                        if (ptrCommand->Exec(shared_from_this()) > 0)
                        {
                            update_timer(CONNECTION_TIMEOUT);
                        }
                        else
                        {
                            LOG_SESSION_ERROR("exec error");
                        }
                    }
                    else
                    {
                        LOG_SESSION_ERROR("command = " << command);
                    }
                } else
                {
                    LOG_SESSION_ERROR("session not weakup");
                }

                LOG_SESSION_INFO("Packet end");
            } else {
                LOG_SESSION_ERROR("no data");

                throw std::runtime_error("no_data error");
            }

            LOG_SESSION_INFO("end");
        }

        int send_packet(DataProcesser& pp, unsigned int timeout = 0) {
            LOG_SESSION_INFO("begin timeout = " << timeout);

            int ret = -1;

            COMM_HEAD head;
            head.version = pp.getVersion();
            head.channel = pp.getChannel();
            head.id = htons(pp.getId());

            std::string strSend((char*)&head, sizeof(head));

            (dynamic_cast<PacketProcesser &>(pp)).getPacket().AppendToString(&strSend);

            PCOMM_HEAD pHead = (PCOMM_HEAD)strSend.data();
            pHead->size = htonl(strSend.size());

            LOG_SESSION_INFO("packet: version = " << (int)pHead->version << " channel = " << (int)pHead->channel << " id = " << ntohs(pHead->id) << " size = " << ntohl(pHead->size));

            ret = send((char*)strSend.data(), strSend.size(), timeout);

            LOG_SESSION_INFO("end ret = " << ret);

            return ret;
        }

        int send_command(boost::shared_ptr<DataProcesser> ptrProcesser, char* sendBuf, int sendSize, unsigned int timeout = 0)
        {
            LOG_SESSION_INFO("begin timeout = " << timeout);

            int ret = -1;

            COMM_HEAD head;
            head.version = ptrProcesser->getVersion();
            head.channel = ptrProcesser->getChannel();
            head.id = htons(ptrProcesser->getId());

            std::string strSend((char*)&head, sizeof(head));

            strSend.append(sendBuf, sendSize);

            PCOMM_HEAD pHead = (PCOMM_HEAD)strSend.data();
            pHead->size = htonl(strSend.size());

            LOG_SESSION_INFO("packet: version = " << (int)pHead->version << " channel = " << (int)pHead->channel << " id = " << ntohs(pHead->id) << " size = " << ntohl(pHead->size));

            ret = send((char*)strSend.data(), strSend.size(), timeout);

            LOG_SESSION_INFO("end ret = " << ret);

            return ret;
        }

        bool isWeakup() const {
            return weakup_;
        }

        void setWeakup(bool weakup) {
            packet_session::weakup_ = weakup;
        }

    private:
        bool weakup_ = false;
    };
}


#endif //CAR_MANAGE_SESSION_H
