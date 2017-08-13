//
// Created by waqi on 1/16/17.
//

#ifndef CAR_PROCESSER_TIMER_H
#define CAR_PROCESSER_TIMER_H


#include "timer_object.h"
#include "DataProcesser.h"
#include "logger.h"
#include "car.h"
#include "timer_state.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(DataProcesser);
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<actor>);

    class processer_timer : public timer_object {
    public:
        processer_timer(DataProcesser processer, boost::shared_ptr<actor> actor) : processer_(processer), actor_(actor) {}

        virtual int Exec() override {
            LOG_TIMER_INFO("begin");

            anon_send(*actor_, timeout_atom::value, processer_);

            LOG_TIMER_INFO("end");
        }

        virtual int equal(boost::shared_ptr<timer_object>& ptrObject) override {
            LOG_TIMER_INFO("begin");

            int nRet = 1;

            boost::shared_ptr<processer_timer> ptrTimer = boost::dynamic_pointer_cast<processer_timer>(ptrObject);
            if(ptrTimer &&
                    processer_.getChannel() == ptrTimer->processer_.getChannel() &&
                    processer_.getId() == ptrTimer->processer_.getId())
            {
                nRet = 0;
            }

            LOG_TIMER_INFO("end ret = " << nRet);

            return nRet;
        }

    private:
        DataProcesser    processer_;
        boost::shared_ptr<actor>    actor_;
    };
}


#endif //CAR_PROCESSER_TIMER_H
