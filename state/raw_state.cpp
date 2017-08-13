//
// Created by waqi on 1/12/17.
//


#include "raw_state.h"
#include "car.h"
#include "RawSendCommand.h"
#include "timer_state.h"
#include "processer_timer.h"
#include "RawSendDataCommand.h"
#include "RawRecvPositionNotification.h"
#include "RawSendPositionGet.h"
#include "RawSendPositionSet.h"
#include "PositionSet.h"
#include "RawSendStatusGet.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<agv::Command>);

behavior agv::Raw(stateful_actor<raw_state> *self)
{
    if (!self->state.init("raw")) {
        A_PRINT("break");
        return {};
    }

    A_PRINT("begin");

    auto get_session = [&] (stateful_actor<raw_state> *self) -> boost::shared_ptr<packet_session> {
        return boost::dynamic_pointer_cast<packet_session>(self->state.ptrSession_.lock());
    };

    auto parse_ack = [&] (stateful_actor<raw_state> *self, boost::shared_ptr<Command>& ptrCommand, boost::shared_ptr<Command>& ptrChild) -> int {

        ptrCommand->Done(ptrChild);

        return 0;
    };

    auto send_comand = [&] (stateful_actor<raw_state> *self, unsigned int nCommand, std::string* pData = NULL, boost::shared_ptr<Command> ptrParent = nullptr, unsigned int timeout = COMMAND_LONG_TIMEOUT) -> int {
        LOG_COMMAND_INFO("begin");

        int ret = FAILED;

        boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
        if (ptrPacketSession)
        {
            boost::shared_ptr<RawProcesser> ptrRaw(new RawProcesser(PROTOCOL_VERSION, CHANNEL_RAW, self->state.get_packet_id(), nCommand));
            boost::shared_ptr<RawSendDataCommand> ptrCommand = boost::dynamic_pointer_cast<RawSendDataCommand>(ptrRaw->CreateCommand());
            if (pData)
            {
                ptrCommand->setData(*pData);
            }
            if (ptrParent)
            {
                ptrCommand->setPtrParentCommand(ptrParent);
            }
            if (ptrCommand->Exec(ptrPacketSession) > 0)
            {
                ret = SUCCEED;

                self->state.commands_[*ptrRaw] = ptrCommand;

                if (timeout)
                {
                    boost::shared_ptr<processer_timer> ptrTimeObject(new processer_timer(*ptrRaw, g_ptr_raw_actor));
                    Add_Timer(timeout, ptrTimeObject);
                }
            }
            else
            {
                ptrPacketSession->close();

                if (ptrParent)
                {
                    ptrParent->setError(RC_FAILED);

                    ptrParent->Done();
                }
            }
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    };

    A_PRINT("end");

    return {
            [=](set_session_atom, boost::shared_ptr<session> ptrSession) {
                A_PRINT("received {'set_session_atom'}");

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession && ptrPacketSession != ptrSession)
                {
                    ptrPacketSession->close();
                }

                self->state.ptrSession_ = ptrSession;

                //anon_send(self, get_status_atom::value);
            },
            [=](recv_ack_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'recv_ack_atom'}");

                std::map<DataProcesser, boost::shared_ptr<Command>>::iterator it = self->state.commands_.find(*ptrCommand->getPtrProcesser());
                if(it != self->state.commands_.end())
                {
                    if (parse_ack(self, it->second, ptrCommand) == 0)
                    {
                        self->state.commands_.erase(it);
                    }
                }
            },
            [=](get_status_atom, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'get_status_atom'}");

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    boost::shared_ptr<RawProcesser> ptrRaw(new RawProcesser(PROTOCOL_VERSION, CHANNEL_RAW, self->state.get_packet_id(), COMMAND_STATUS));
                    boost::shared_ptr<RawSendStatusGet> ptrRawStatus = boost::dynamic_pointer_cast<RawSendStatusGet>(ptrRaw->CreateCommand());
                    if (ptrParent)
                    {
                        ptrRawStatus->setPtrParentCommand(ptrParent);
                    }
                    if (ptrRawStatus->Exec(ptrPacketSession) > 0)
                    {
                        self->state.commands_[*ptrRaw] = ptrRawStatus;

                        boost::shared_ptr<processer_timer> ptrTimeObject(new processer_timer(*ptrRaw, g_ptr_raw_actor));
                        Add_Timer(COMMAND_SHORT_TIMEOUT, ptrTimeObject);
                    }
                    else
                    {
                        ptrPacketSession->close();
                    }
                }
            },
            [=](get_position_atom, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'get_position_atom'}");

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    boost::shared_ptr<RawProcesser> ptrRaw(new RawProcesser(PROTOCOL_VERSION, CHANNEL_RAW, self->state.get_packet_id(), COMMAND_POSITION));
                    boost::shared_ptr<RawSendPositionGet> ptrRawPosition = boost::dynamic_pointer_cast<RawSendPositionGet>(ptrRaw->CreateCommand());
                    if (ptrParent)
                    {
                        ptrRawPosition->setPtrParentCommand(ptrParent);
                    }
                    if (ptrRawPosition->Exec(ptrPacketSession) > 0)
                    {
                        self->state.commands_[*ptrRaw] = ptrRawPosition;

                        boost::shared_ptr<processer_timer> ptrTimeObject(new processer_timer(*ptrRaw, g_ptr_raw_actor));
                        Add_Timer(COMMAND_SHORT_TIMEOUT, ptrTimeObject);
                    }
                    else
                    {
                        ptrPacketSession->close();
                    }
                }
            },
            [=](set_position_atom, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'set_position_atom'}");

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    boost::shared_ptr<RawProcesser> ptrRaw(new RawProcesser(PROTOCOL_VERSION, CHANNEL_RAW, self->state.get_packet_id(), COMMAND_SET_POSITION));
                    boost::shared_ptr<RawSendPositionSet> ptrRawPosition = boost::dynamic_pointer_cast<RawSendPositionSet>(ptrRaw->CreateCommand());
                    boost::shared_ptr<PositionSet> ptrPosition = boost::dynamic_pointer_cast<PositionSet>(ptrParent);
                    if (ptrPosition)
                    {
                        POSITION_ACK position = ptrPosition->GetRawPosition(ptrPosition->GetPacketPosition());
                        std::string strData((char*)&position, sizeof(position));
                        ptrRawPosition->setData(strData);
                    }
                    if (ptrParent)
                    {
                        ptrRawPosition->setPtrParentCommand(ptrParent);
                    }
                    if (ptrRawPosition->Exec(ptrPacketSession) > 0)
                    {
                        self->state.commands_[*ptrRaw] = ptrRawPosition;

                        boost::shared_ptr<processer_timer> ptrTimeObject(new processer_timer(*ptrRaw, g_ptr_raw_actor));
                        Add_Timer(COMMAND_SHORT_TIMEOUT, ptrTimeObject);
                    }
                    else
                    {
                        ptrPacketSession->close();
                    }
                }
            },
            [=](turn_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'turn_atom'}");

                send_comand(self, COMMAND_TURN, &strData, ptrParent);
            },
            [=](stop_atom) {
                A_PRINT("received {'stop_atom'}");

                send_comand(self, COMMAND_STOP);
            },
            [=](cancel_atom) {
                A_PRINT("received {'cancel_atom'}");

                send_comand(self, COMMAND_CANCEL);
            },
            [=](forward_atom, std::string strData) {
                A_PRINT("received {'forward_atom'}");

                send_comand(self, COMMAND_FORWARD, &strData);
            },
            [=](back_atom, std::string strData) {
                A_PRINT("received {'back_atom'}");

                send_comand(self, COMMAND_BACK, &strData);
            },
            [=](tracing_forward_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'tracing_forward_atom'}");

                send_comand(self, COMMAND_TRACING_FORWARD, &strData, ptrParent);
            },
            [=](tracing_back_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'tracing_back_atom'}");

                send_comand(self, COMMAND_TRACING_BACK, &strData, ptrParent);
            },
            [=](axle_move_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'axle_move_atom'}");

                send_comand(self, COMMAND_AXLE_MOVE, &strData, ptrParent);
            },
            [=](control_x_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'control_x_atom'}");

                send_comand(self, COMMAND_CONTROL_X, &strData, ptrParent);
            },
            [=](control_y_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'control_y_atom'}");

                send_comand(self, COMMAND_CONTROL_Y, &strData, ptrParent);
            },
            [=](control_z_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'control_z_atom'}");

                send_comand(self, COMMAND_CONTROL_Z, &strData, ptrParent);
            },
            /*[=](car_align_atom, std::string strData) {
                A_PRINT("received {'car_align_atom'}");

                send_comand(self, COMMAND_CAR_ALIGN, &strData);
            },*/
            [=](start_charge_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'start_charge_atom'}");

                send_comand(self, COMMAND_START_CHARGE, &strData, ptrParent);
            },
            [=](stop_charge_atom, std::string strData, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'stop_charge_atom'}");

                send_comand(self, COMMAND_STOP_CHARGE, &strData, ptrParent);
            },
            [=](status_nty_atom) {
                A_PRINT("received {'status_nty_atom'}");
            },
            [=](position_nty_atom pn, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'position_nty_atom'}");
            },
            [=](set_sensor_atom pn, std::string strData) {
                A_PRINT("received {'set_sensor_atom'}");

                send_comand(self, COMMAND_SET_SENSOR, &strData);
            },
            [=](stop_nty_atom) {
                A_PRINT("received {'stop_nty_atom'}");
            },
            [=](timeout_atom, DataProcesser processer) {
                LOG_TIMER_INFO("received {'timeout_atom'}");

                std::map<DataProcesser, boost::shared_ptr<Command>>::iterator it = self->state.commands_.find(processer);
                if(it != self->state.commands_.end())
                {
                    LOG_TIMER_INFO("command timeout channel = " << it->second->getCommandChannel() << " id = " << it->second->getId() << " command = " << it->second->getCommand());

                    it->second->setError(RC_TIMEOUT);

                    it->second->Done();

                    self->state.commands_.erase(it);

                    /*boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                    if (ptrPacketSession)
                    {
                        ptrPacketSession->close();
                    }
                    self->state.ptrSession_.reset();*/
                }
            }
    };
}
