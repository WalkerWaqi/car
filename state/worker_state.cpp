//
// Created by waqi on 1/6/17.
//

#include "worker_state.h"
#include "JobDoneCommand.h"
#include "TriggerCommand.h"
#include "PositionNotification.h"
#include "PositionGet.h"
#include "RawRecvPositionNotification.h"
#include "StatusNotification.h"
#include "RawRecvStatusNotification.h"
#include "Tools.h"
#include "car.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<agv::Command>);

behavior agv::Worker(stateful_actor<worker_state> *self)
{
    using std::chrono::milliseconds;

    if (!self->state.init("worker")) {
        A_PRINT("break");
        return {};
    }

    A_PRINT("begin");

    auto get_session = [&] (stateful_actor<worker_state> *self) -> boost::shared_ptr<packet_session> {
        return boost::dynamic_pointer_cast<packet_session>(self->state.ptrSession_.lock());
    };

    auto do_action = [&] (stateful_actor<worker_state> *self, const Action& action, boost::shared_ptr<Command>& ptrCommand) -> int {
        int nRet = FAILED;

        A_PRINT("do_action begin");
        A_PRINT("do_action begin action = " << action.action());
        A_PRINT("do_action begin params_size = " << action.params_size());
        for(int i = 0; i < action.params_size(); ++i)
        {
            A_PRINT("do_action params " << i << " size = " << action.params(i).size());
        }

        switch (action.action())
        {
            case (AT_WAIT):
            {
                self->delayed_send(self, milliseconds(ntohl(*(int*)(action.params(0).data()))), done_action_atom::value, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            case (AT_WAIT_TRIGGER):
            {
                nRet = SUCCEED;
            }
                break;
            case (AT_FORWARD):
            {
                PSENSOR_COMMAND pSensor = (PSENSOR_COMMAND)action.params(2).data();
                std::string strSensor((char*)pSensor, sizeof(SENSOR_COMMAND));
                anon_send(*g_ptr_raw_actor, set_sensor_atom::value, strSensor);


                TRACING_COMMAND tracing = {};
                tracing.speed = *(int*)(action.params(0).data());
                tracing.flags = *(int*)(action.params(1).data());
                pSensor = (PSENSOR_COMMAND)action.params(action.params_size() - 1).data();
                strncpy((char*)tracing.rfid, (char*)pSensor->rfid, sizeof(tracing.rfid));

                A_PRINT("do_action AT_FORWARD rfid = " << std::string((char*)tracing.rfid, sizeof(tracing.rfid)) << " speed = " << ntohl(tracing.speed) << " flags = " << ntohl(tracing.flags));

                std::string strData((char *) &tracing, sizeof(tracing));
                anon_send(*g_ptr_raw_actor, tracing_forward_atom::value, strData, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            case (AT_BACK):
            {
                PSENSOR_COMMAND pSensor = (PSENSOR_COMMAND)action.params(2).data();
                std::string strSensor((char*)pSensor, sizeof(SENSOR_COMMAND));
                anon_send(*g_ptr_raw_actor, set_sensor_atom::value, strSensor);


                TRACING_COMMAND tracing = {};
                tracing.speed = *(int*)(action.params(0).data());
                tracing.flags = *(int*)(action.params(1).data());
                pSensor = (PSENSOR_COMMAND)action.params(action.params_size() - 1).data();
                strncpy((char*)tracing.rfid, (char*)pSensor->rfid, sizeof(tracing.rfid));

                A_PRINT("do_action AT_BACK rfid = " << std::string((char*)tracing.rfid, sizeof(tracing.rfid)) << " speed = " << ntohl(tracing.speed) << " flags = " << ntohl(tracing.flags));

                std::string strData((char *) &tracing, sizeof(tracing));
                anon_send(*g_ptr_raw_actor, tracing_back_atom::value, strData, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            case (AT_TURN):
            {
                PSENSOR_COMMAND pSensor = (PSENSOR_COMMAND)action.params(1).data();
                std::string strSensor((char*)pSensor, sizeof(SENSOR_COMMAND));
                anon_send(*g_ptr_raw_actor, set_sensor_atom::value, strSensor);


                TURN_COMMAND turn = {};
                turn.angle = *(int*)(action.params(0).data());

                std::string strData((char *) &turn, sizeof(turn));
                anon_send(*g_ptr_raw_actor, turn_atom::value, strData, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            case (AT_RUN):
            {
                if (action.params_size() > 0 && !action.params(0).empty())
                {
                    anon_send(*g_ptr_raw_actor, axle_move_atom::value, std::string(action.params(0)), ptrCommand);

                    nRet = SUCCEED;
                }
            }
                break;
            case (AT_RUN_FILE):
            {
                if (action.params_size() > 0 && !action.params(0).empty())
                {
                    std::string strFile = CAR_DATA_PATH;
                    strFile += "/";
                    strFile += action.params(0);

                    std::string strData;
                    Tools::read_file(strFile, strData);

                    if (!strData.empty())
                    {
                        anon_send(*g_ptr_raw_actor, axle_move_atom::value, strData, ptrCommand);

                        nRet = SUCCEED;
                    }
                }
            }
                break;
            case (AT_START_CHARGE):
            {
                CHARGE_COMMAND charge = {};
                charge.station = ntohl(*(int*)(action.params(0).data()));

                std::string strData((char *) &charge, sizeof(charge));
                anon_send(*g_ptr_raw_actor, start_charge_atom::value, strData, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            case (AT_STOP_CHARGE):
            {
                CHARGE_COMMAND charge = {};
                charge.station = ntohl(*(int*)(action.params(0).data()));

                std::string strData((char *) &charge, sizeof(charge));
                anon_send(*g_ptr_raw_actor, stop_charge_atom::value, strData, ptrCommand);

                nRet = SUCCEED;
            }
                break;
            default:
                break;
        }

        A_PRINT("do_action end ret = " << nRet);

        return nRet;
    };

    auto do_trigger = [&] (stateful_actor<worker_state> *self, TriggerType type) -> int {
        int nRet = FAILED;

        switch (type)
        {
            case TT_ANY:
            {
                if (self->state.ptrCurrentJobCommand_)
                {
                    boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());

                    if (!self->state.ptrCurrentDoneCommand_ && self->state.ptrCurrentJobCommand_->getError() == 0 && (self->state.jobActionIndex_  < ptrPacket->getPacket().job().actions_size()))
                    {
                        if (ptrPacket->getPacket().job().actions(self->state.jobActionIndex_).action() == AT_WAIT_TRIGGER)
                        {
                            anon_send(self, done_action_atom::value, self->state.ptrCurrentJobCommand_->shared_from_this());

                            break;
                        }
                    }
                }
            }
                break;
            case TT_EXCHANGE:
            case TT_TAKE:
            {
                boost::shared_ptr<PacketProcesser> ptrDataProcesser(new PacketProcesser(PROTOCOL_VERSION, CHANNEL_MANAGE, self->state.get_packet_id(), TRIGGER));
                boost::shared_ptr<TriggerCommand> ptrTrigger = boost::dynamic_pointer_cast<TriggerCommand>(ptrDataProcesser->CreateCommand());

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                int ret = -1;
                if (ptrPacketSession)
                {
                    ptrTrigger->setType(type);

                    ret = ptrTrigger->Exec(ptrPacketSession);
                }

                if (ret <= 0)
                {
                    self->delayed_send(self, milliseconds(1000), trigger_atom::value, type);
                }
            }
                break;
            default:
                break;
        }

        return nRet;
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
            },
            [=](recv_ack_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'recv_ack_atom'} ptrCommand->getId() = " << ptrCommand->getId());

                if (self->state.ptrCurrentDoneCommand_)
                {
                    A_PRINT("received {'recv_ack_atom'} self->state.ptrCurrentDoneCommand_->getId() = " << self->state.ptrCurrentDoneCommand_->getId());

                    if (self->state.ptrCurrentDoneCommand_->getId() == ptrCommand->getId())
                    {
                        A_PRINT("received {'recv_ack_atom'} self->state.ptrCurrentDoneCommand_->getId() == ptrCommand->getId()");

                        self->state.ptrCurrentDoneCommand_.reset();
                        self->state.ptrCurrentJobCommand_.reset();
                        self->state.jobActionIndex_ = 0;
                    }
                }
            },
            [=](get_status_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'get_status_atom'} ptrCommand->getId() = " << ptrCommand->getId());

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    ptrCommand->Done(nullptr, ptrPacketSession);
                }
            },
            [=](status_nty_atom, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'status_nty_atom'}");

                boost::shared_ptr<RawRecvStatusNotification> ptrRawStatus = boost::dynamic_pointer_cast<RawRecvStatusNotification>(ptrParent);
                if (ptrRawStatus)
                {
                    STATUS_ACK& rawStatus = ptrRawStatus->GetRawStatus();
                    g_status.set_battery(rawStatus.battery);
                    g_status.set_blocking(rawStatus.blocking);
                    g_status.set_empty(rawStatus.empty);

                    boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                    if (ptrPacketSession)
                    {
                        boost::shared_ptr<PacketProcesser> ptrDataProcesser(new PacketProcesser(PROTOCOL_VERSION, CHANNEL_MANAGE, self->state.get_packet_id(), STATUS_NTY));
                        boost::shared_ptr<StatusNotification> ptrStatus = boost::dynamic_pointer_cast<StatusNotification>(ptrDataProcesser->CreateCommand());
                        if (ptrStatus)
                        {
                            ptrStatus->SetStatus(g_status);

                            ptrStatus->Exec(ptrPacketSession);
                        }
                    }
                }
            },
            [=](get_position_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'get_position_atom'} ptrCommand->getId() = " << ptrCommand->getId());

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    ptrCommand->Done(nullptr, ptrPacketSession);
                }
            },
            [=](set_position_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'set_position_atom'} ptrCommand->getId() = " << ptrCommand->getId());

                boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                if (ptrPacketSession)
                {
                    ptrCommand->Done(nullptr, ptrPacketSession);
                }
            },
            [=](position_nty_atom, boost::shared_ptr<Command> ptrParent) {
                A_PRINT("received {'position_nty_atom'}");

                boost::shared_ptr<RawRecvPositionNotification> ptrRawPosition = boost::dynamic_pointer_cast<RawRecvPositionNotification>(ptrParent);
                if (ptrRawPosition)
                {
                    POSITION_ACK& rawPosition = ptrRawPosition->GetRawPosition();

                    A_PRINT("received {'position_nty_atom'} rfid = " << rawPosition.rfid << " offset = " << rawPosition.offset << " offset_angle = " << rawPosition.offset_angle << " angle = " << rawPosition.angle << " speed = " << rawPosition.speed);

                    if (self->state.ptrCurrentJobCommand_)
                    {
                        boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());
                        if (self->state.jobActionIndex_ < ptrPacket->getPacket().job().actions_size())
                        {
                            Action& action = (Action&)ptrPacket->getPacket().job().actions(self->state.jobActionIndex_);
                            if (action.action() == AT_FORWARD || action.action() == AT_BACK)
                            {
                                bool found = false;
                                for(int i = 2; i < action.params_size(); ++i)
                                {
                                    PSENSOR_COMMAND pSensor = (PSENSOR_COMMAND)action.params(i).data();
                                    if (strncmp((char*)rawPosition.rfid, (char*)pSensor->rfid, sizeof(rawPosition.rfid)) == 0)
                                    {
                                        std::string strData((char*)pSensor, sizeof(SENSOR_COMMAND));

                                        anon_send(*g_ptr_raw_actor, set_sensor_atom::value, strData);

                                        found = true;

                                        break;
                                    }
                                }

                                A_PRINT("received {'position_nty_atom'} found = " << found);

                                /*if (!found)
                                {
                                    self->state.ptrCurrentJobCommand_->setError(RC_EXCEPTION);

                                    anon_send(*g_ptr_raw_actor, cancel_atom::value);
                                }*/
                            }
                        }
                    }

                    g_position.set_station_id(std::string((char*)rawPosition.rfid, sizeof(rawPosition.rfid)).c_str());
                    g_position.set_angle(ntohl(rawPosition.angle));
                    g_position.set_speed(ntohl(rawPosition.speed));

                    boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                    if (ptrPacketSession)
                    {
                        boost::shared_ptr<PacketProcesser> ptrDataProcesser(new PacketProcesser(PROTOCOL_VERSION, CHANNEL_MANAGE, self->state.get_packet_id(), POSITION_NTY));
                        boost::shared_ptr<PositionNotification> ptrPosition = boost::dynamic_pointer_cast<PositionNotification>(ptrDataProcesser->CreateCommand());
                        if (ptrPosition)
                        {
                            ptrPosition->AddPosition(g_position);

                            ptrPosition->Exec(ptrPacketSession);
                        }
                    }
                }
            },
            [=](config_atom) {
                A_PRINT("received {'config_atom'}");
            },
            [=](job_start_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'job_start_atom'} ptrCommand->getId() = " << ptrCommand->getId());

                do
                {
                    boost::shared_ptr<JobStartCommand> ptrJobTmp = boost::dynamic_pointer_cast<JobStartCommand>(ptrCommand);
                    boost::shared_ptr<PacketProcesser> ptrPacketTmp = boost::dynamic_pointer_cast<PacketProcesser>(ptrJobTmp->getPtrProcesser());
                    if (self->state.ptrCurrentJobCommand_)
                    {
                        boost::shared_ptr<PacketProcesser> ptrPacketCurrent = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());
                        if (ptrPacketCurrent->getPacket().job().id() == ptrPacketTmp->getPacket().job().id())
                        {
                            A_PRINT("received {'job_start_atom'} same job = " << ptrPacketTmp->getPacket().job().id());
                            break;
                        }
                        else
                        {
                            A_PRINT("received {'job_start_atom'} old job = " << ptrPacketCurrent->getPacket().job().id());
                        }
                    }

                    self->state.ptrCurrentJobCommand_ = ptrJobTmp;
                    self->state.jobActionIndex_ = ptrPacketTmp->getPacket().job().action_index();
                    self->state.ptrCurrentDoneCommand_.reset();

                    A_PRINT("received {'job_start_atom'} new job_id = " << ptrPacketTmp->getPacket().job().id() << " jobActionIndex_ = " << self->state.jobActionIndex_);

                    anon_send(self, start_action_atom::value, ptrCommand, self->state.jobActionIndex_);
                } while(0);
            },
            [=](start_action_atom, boost::shared_ptr<Command> ptrCommand, unsigned int action_index) {
                A_PRINT("received {'start_action_atom'} action_index = " << action_index);

                if (self->state.ptrCurrentJobCommand_)
                {
                    A_PRINT("received {'start_action_atom'} self->state.ptrCurrentJobCommand_ != nullptr");

                    if (self->state.ptrCurrentJobCommand_ == ptrCommand)
                    {
                        A_PRINT("received {'start_action_atom'} self->state.ptrCurrentJobCommand_ == ptrCommand");

                        boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());

                        if (ptrPacket)
                        {
                            A_PRINT("received {'start_action_atom'} ptrPacket != nullptr");
                            A_PRINT("received {'start_action_atom'} actions_size = " << ptrPacket->getPacket().job().actions_size());
                            if (do_action(self, ptrPacket->getPacket().job().actions(action_index), ptrCommand) != SUCCEED)
                            {
                                anon_send(self, done_action_atom::value, ptrCommand);
                            }
                        }
                        else
                        {
                            A_PRINT("received {'start_action_atom'} error ptrPacket == nullptr");
                        }
                    }
                    else
                    {
                        A_PRINT("received {'start_action_atom'} error self->state.ptrCurrentJobCommand_ != ptrCommand");
                    }
                }
                else
                {
                    A_PRINT("received {'start_action_atom'} error self->state.ptrCurrentJobCommand_ == nullptr");
                }
            },
            [=](done_action_atom, boost::shared_ptr<Command> ptrCommand) {
                A_PRINT("received {'done_action_atom'}");

                if (self->state.ptrCurrentJobCommand_ && self->state.ptrCurrentJobCommand_ == ptrCommand)
                {
                    A_PRINT("self->state.ptrCurrentDoneCommand_ error = " << ptrCommand->getError());

                    boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());
                    if (!self->state.ptrCurrentDoneCommand_ && ptrCommand->getError() == 0 && (self->state.jobActionIndex_ < (ptrPacket->getPacket().job().actions_size() - 1)))
                    {
                        ++self->state.jobActionIndex_;

                        A_PRINT("start action = " << self->state.jobActionIndex_);

                        anon_send(self, start_action_atom::value, ptrCommand, self->state.jobActionIndex_);
                    }
                    else
                    {
                        A_PRINT("jobe done action =  " << self->state.jobActionIndex_ << " error = " << ptrCommand->getError());

                        //self->state.ptrCurrentJobCommand_->getPtrProcesser()->setId(self->state.get_packet_id());
                        //JobDoneCommand jobDone(self->state.ptrCurrentJobCommand_->getPtrProcesser());
                        //self->state.ptrCurrentJobCommand_->getPtrProcesser()->setCommand(JOB_DONE);

                        boost::shared_ptr<packet_session> ptrPacketSession = get_session(self);
                        int ret = -1;
                        if (ptrPacketSession)
                        {
                            A_PRINT("ptrPacketSession != nullptr");

                            if (!self->state.ptrCurrentDoneCommand_)
                            {
                                A_PRINT("self->state.ptrCurrentDoneCommand_ == nullptr");

                                boost::shared_ptr<PacketProcesser> ptrDataProcesser(new PacketProcesser(PROTOCOL_VERSION, CHANNEL_MANAGE, self->state.get_packet_id(), JOB_DONE));
                                self->state.ptrCurrentDoneCommand_ = boost::dynamic_pointer_cast<JobDoneCommand>(ptrDataProcesser->CreateCommand());

                                boost::shared_ptr<PacketProcesser> ptrPacket = boost::dynamic_pointer_cast<PacketProcesser>(self->state.ptrCurrentJobCommand_->getPtrProcesser());
                                self->state.ptrCurrentDoneCommand_->setJobInfo(((Job &) ptrPacket->getPacket().job()).id(), self->state.jobActionIndex_);

                                self->state.ptrCurrentDoneCommand_->setError(ptrCommand->getError());
                            }

                            if (self->state.ptrCurrentDoneCommand_)
                            {
                                A_PRINT("will job done");

                                ret = self->state.ptrCurrentDoneCommand_->Exec(ptrPacketSession);
                            }
                        }
                        else{
                            A_PRINT("ptrPacketSession == nullptr");
                        }

                        self->delayed_send(self, milliseconds(SEND_TIMEOUT), done_action_atom::value, ptrCommand);

                        /*if (ret <= 0)
                        {
                            self->delayed_send(self, milliseconds(1000), done_action_atom::value, ptrCommand);
                        }
                        else
                        {
                            self->state.ptrCurrentJobCommand_.reset();
                            self->state.jobActionIndex_ = 0;
                        }*/
                    }
                }
            },
            [=](job_cancel_atom) {
                A_PRINT("received {'job_cancel_atom'}");

                self->state.ptrCurrentJobCommand_->setError(RC_CANCEL);

                anon_send(*g_ptr_raw_actor, cancel_atom::value);
            },
            [=](stop_nty_atom) {
                A_PRINT("received {'stop_nty_atom'}");
            },
            [=](trigger_atom, TriggerType type) {
                A_PRINT("received {'trigger_atom'}");

                do_trigger(self, type);
            }
    };
}
