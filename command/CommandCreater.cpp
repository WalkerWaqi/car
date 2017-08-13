//
// Created by waqi on 12/15/16.
//

#include "CommandCreater.h"
#include "WakeupCommand.h"
#include "HeartbeatCommand.h"
#include "JobStartCommand.h"
#include "JobDoneCommand.h"
#include "TriggerCommand.h"
#include "PositionNotification.h"
#include "PositionGet.h"
#include "PositionSet.h"
#include "RawRecvWakeupCommand.h"
#include "RawRecvAckCommand.h"
#include "RawSendDataCommand.h"
#include "RawRecvPositionNotification.h"
#include "RawSendPositionGet.h"
#include "RawSendPositionSet.h"
#include "StatusGet.h"
#include "StatusNotification.h"
#include "RawRecvStatusNotification.h"
#include "RawSendStatusGet.h"
#include "AckCommand.h"
#include "CancelCommand.h"


namespace agv {
    boost::unordered_map<CommandKey, boost::shared_ptr<CommandCreater>> CommandCreater::creaters_;

    std::size_t hash_value(CommandKey const &commandKey)
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, commandKey.channel_);
        boost::hash_combine(seed, commandKey.command_);
        return seed;
    }

    void CommandCreater::init() {
        LOG_COMMAND_INFO("begin");

        RegisterCommand(CHANNEL_ADMIN, WAKEUP, boost::shared_ptr<CommandCreater>(new CommandCreaterT<WakeupCommand>));
        RegisterCommand(CHANNEL_ADMIN, HEARTBEAT, boost::shared_ptr<CommandCreater>(new CommandCreaterT<HeartbeatCommand>));
        RegisterCommand(CHANNEL_MANAGE, JOB_START, boost::shared_ptr<CommandCreater>(new CommandCreaterT<JobStartCommand>));
        RegisterCommand(CHANNEL_MANAGE, JOB_DONE, boost::shared_ptr<CommandCreater>(new CommandCreaterT<JobDoneCommand>));
        RegisterCommand(CHANNEL_MANAGE, TRIGGER, boost::shared_ptr<CommandCreater>(new CommandCreaterT<TriggerCommand>));
        RegisterCommand(CHANNEL_MANAGE, POSITION_NTY, boost::shared_ptr<CommandCreater>(new CommandCreaterT<PositionNotification>));
        RegisterCommand(CHANNEL_MANAGE, POSITION, boost::shared_ptr<CommandCreater>(new CommandCreaterT<PositionGet>));
        RegisterCommand(CHANNEL_MANAGE, POSITION_SET, boost::shared_ptr<CommandCreater>(new CommandCreaterT<PositionSet>));
        RegisterCommand(CHANNEL_MANAGE, STATUS_NTY, boost::shared_ptr<CommandCreater>(new CommandCreaterT<StatusNotification>));
        RegisterCommand(CHANNEL_MANAGE, STATUS, boost::shared_ptr<CommandCreater>(new CommandCreaterT<StatusGet>));
        RegisterCommand(CHANNEL_MANAGE, CMD_ACK, boost::shared_ptr<CommandCreater>(new CommandCreaterT<AckCommand>));
        RegisterCommand(CHANNEL_MANAGE, CANCEL_JOB, boost::shared_ptr<CommandCreater>(new CommandCreaterT<CancelCommand>));

        RegisterCommand(CHANNEL_RAW, COMMAND_WEAKUP, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawRecvWakeupCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_ACK, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawRecvAckCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_STATUS, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendStatusGet>));
        RegisterCommand(CHANNEL_RAW, COMMAND_POSITION, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendPositionGet>));
        RegisterCommand(CHANNEL_RAW, COMMAND_SET_POSITION, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendPositionSet>));
        RegisterCommand(CHANNEL_RAW, COMMAND_STOP, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_CANCEL, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_FORWARD, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_BACK, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_START_CHARGE, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_STOP_CHARGE, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_TURN, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_TRACING_FORWARD, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_TRACING_BACK, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        //RegisterCommand(CHANNEL_RAW, COMMAND_CAR_ALIGN, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_AXLE_MOVE, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_CONTROL_X, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_CONTROL_Y, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_CONTROL_Z, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, COMMAND_SET_SENSOR, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawSendDataCommand>));
        RegisterCommand(CHANNEL_RAW, NOTIFY_POSITION, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawRecvPositionNotification>));
        RegisterCommand(CHANNEL_RAW, NOTIFY_STATUS, boost::shared_ptr<CommandCreater>(new CommandCreaterT<RawRecvStatusNotification>));

        LOG_COMMAND_INFO("end");
    }

    boost::shared_ptr<Command> CommandCreater::CreateCommand(boost::shared_ptr<DataProcesser> ptrProcesser) {
        LOG_COMMAND_INFO("begin channel = " << (int)(ptrProcesser->getChannel()) << " command = " << ptrProcesser->getCommand());

        boost::shared_ptr<CommandCreater> ptrCommandCreater = creaters_[CommandKey(ptrProcesser->getChannel(), ptrProcesser->getCommand())];
        if(ptrCommandCreater)
        {
            return ptrCommandCreater->CreateCommandT(ptrProcesser);
        }
        else
        {
            LOG_COMMAND_ERROR("ptrCommandCreater == nullptr");

            return nullptr;
        }

        LOG_COMMAND_INFO("end");
    }
}