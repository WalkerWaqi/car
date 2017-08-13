//
// Created by waqi on 1/13/17.
//

#ifndef CAR_COMMAND_TIMER_H
#define CAR_COMMAND_TIMER_H


#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include "timer_object.h"
#include "Command.h"
#include "logger.h"


namespace agv {
    class command_timer : public timer_object {
    public:
        command_timer(boost::shared_ptr<Command> ptrCommand) : ptrCommand_(ptrCommand) {}

        virtual int Exec() override {
            LOG_TIMER_INFO("begin");



            LOG_TIMER_INFO("end");
        }

        virtual int equal(boost::shared_ptr<timer_object>& ptrObject) override {
            LOG_TIMER_INFO("begin");

            int nRet = -1;

            boost::shared_ptr<command_timer> ptrTimer = boost::dynamic_pointer_cast<command_timer>(ptrObject);
            if (ptrTimer)
            {
                boost::shared_ptr<Command> ptrCommand = ptrTimer->ptrCommand_.lock();
                if (ptrCommand)
                {
                    if (ptrCommand == ptrCommand_.lock())
                    {
                        nRet = 0;
                    } else
                    {
                        nRet = 1;
                    }
                } else
                {
                    LOG_TIMER_ERROR("ptrCommand == nullptr");
                }
            }
            else
            {
                nRet = 1;
            }

            LOG_TIMER_INFO("end ret = " << nRet);

            return nRet;
        }

        const boost::shared_ptr<Command> getPtrCommand() const {
            return ptrCommand_.lock();
        }

        void setPtrCommand(const boost::shared_ptr<Command> &ptrCommand_) {
            command_timer::ptrCommand_ = ptrCommand_;
        }

    private:
        boost::weak_ptr<Command>    ptrCommand_;
    };
}


#endif //CAR_COMMAND_TIMER_H
