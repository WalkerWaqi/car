//
// Created by waqi on 12/15/16.
//

#ifndef CAR_COMMANDCREATER_H
#define CAR_COMMANDCREATER_H


#include "Command.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "logger.h"


namespace agv {
    struct CommandKey {
        unsigned int   channel_;
        unsigned int   command_;

        CommandKey(unsigned int channel, unsigned int command) : channel_(channel), command_(command) {}

        bool operator==(const CommandKey &commandKey) const
        {
            return channel_ == commandKey.channel_ && command_ == commandKey.command_;
        }
    };

    std::size_t hash_value(CommandKey const &commandKey);

    class CommandCreater {
    public:
        static int RegisterCommand(unsigned char channel, int command, boost::shared_ptr<CommandCreater> pteCommandCreater) {
            LOG_COMMAND_INFO("begin channel = " << (int)channel << " command = " << command);

            creaters_[CommandKey(channel, command)] = pteCommandCreater;

            LOG_COMMAND_INFO("end");
        }

        static int UnregisterCommand(unsigned char channel, int command) {
            LOG_COMMAND_INFO("begin channel = " << channel << " command = " << command);

            creaters_.erase(CommandKey(channel, command));

            LOG_COMMAND_INFO("end");
        }

        static void init();

        static boost::shared_ptr<Command> CreateCommand(boost::shared_ptr<DataProcesser> ptrProcesser);

        virtual boost::shared_ptr<Command> CreateCommandT(boost::shared_ptr<DataProcesser>& ptrProcesser) = 0;

    private:
        static boost::unordered_map<CommandKey, boost::shared_ptr<CommandCreater>>    creaters_;
    };

    template <class T, class base = CommandCreater>
    class CommandCreaterT : public base {
    public:
        virtual boost::shared_ptr<Command> CreateCommandT(boost::shared_ptr<DataProcesser>& ptrProcesser) override {
            return boost::shared_ptr<Command>(new T(ptrProcesser));
        }
    };
}


#endif //CAR_COMMANDCREATER_H
