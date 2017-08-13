//
// Created by waqi on 12/20/16.
//

#ifndef CAR_SESSION_TIMER_H
#define CAR_SESSION_TIMER_H


#include <boost/shared_ptr.hpp>
#include "timer_object.h"
#include "session.h"


namespace agv {
    class session_timer : public timer_object {
    public:
        session_timer(boost::shared_ptr<session> ptrSession) : ptrSession_(ptrSession) {}

        virtual int Exec() override {
            LOG_TIMER_INFO("begin");

            boost::shared_ptr<session> ptrSession = ptrSession_.lock();
            if (ptrSession) {
                ptrSession->close();
            }

            LOG_TIMER_INFO("end");
        }

        virtual int equal(boost::shared_ptr<timer_object>& ptrObject) override {
            LOG_TIMER_INFO("begin");

            int nRet = -1;

            boost::shared_ptr<session_timer> ptrTimer = boost::dynamic_pointer_cast<session_timer>(ptrObject);
            if (ptrTimer)
            {
                boost::shared_ptr<session> ptrSession = ptrTimer->ptrSession_.lock();
                if (ptrSession)
                {
                    if (ptrSession == ptrSession_.lock())
                    {
                        nRet = 0;
                    } else
                    {
                        nRet = 1;
                    }
                } else
                {
                    LOG_TIMER_ERROR("ptrSession == nullptr");
                }
            }
            else
            {
                nRet = 1;
            }

            LOG_TIMER_INFO("end ret = " << nRet);

            return nRet;
        }

    private:
        boost::weak_ptr<session>    ptrSession_;
    };
}


#endif //CAR_SESSION_TIMER_H
