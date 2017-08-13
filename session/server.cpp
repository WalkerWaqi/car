
#include "server.h"
#include "common.h"
#include "timer_state.h"
#include "session_timer.h"


/*CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::posix_time::ptime);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<session>);

void server::add_timer(boost::shared_ptr<session> new_session)
{
    LOG_SESSION_INFO("begin");

    boost::shared_ptr<timer_object> ptrObject(new session_timer(new_session));
    Add_Timer(CONNECTION_TIMEOUT, ptrObject);

    LOG_SESSION_INFO("end");
}

void server::update_timer(boost::shared_ptr<session> new_session)
{
    LOG_SESSION_INFO("begin");

    boost::shared_ptr<timer_object> ptrObject(new session_timer(new_session));
    Update_Timer(CONNECTION_TIMEOUT, ptrObject);

    LOG_SESSION_INFO("end");
}*/
