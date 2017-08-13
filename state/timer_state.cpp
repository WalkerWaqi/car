//
// Created by waqi on 12/12/16.
//

#include "timer_state.h"


CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::posix_time::ptime);
CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<timer_object>);

behavior Timer(stateful_actor<timer_state> *self) {
    using std::chrono::milliseconds;

    if (!self->state.init("timer")) {
        LOG_TIMER_INFO("break");
        return {};
    }

    LOG_TIMER_INFO("begin");

    auto remove_object = [&] (stateful_actor<timer_state> *self, boost::shared_ptr<timer_object> pObject, bool all = true) {
        LOG_TIMER_INFO("begin Objects_.size = " << self->state.Objects_.size());

        std::multimap<boost::posix_time::ptime, boost::shared_ptr<timer_object>>::iterator i = self->state.Objects_.begin();
        while (i != self->state.Objects_.end()) {
            int ret = pObject->equal(i->second);
            if (ret == 0) {
                i = self->state.Objects_.erase(i);

                LOG_TIMER_INFO("Objects_.erase");

                if (!all)
                {
                    break;
                }
            }
            else if (ret < 0) {
                i = self->state.Objects_.erase(i);

                LOG_TIMER_INFO("Objects_.erase");
            }
            else {
                ++i;
            }
        }

        LOG_TIMER_INFO("end Objects_.size = " << self->state.Objects_.size());
    };

    auto add_timer = [&] (stateful_actor<timer_state> *self, boost::posix_time::ptime time, boost::shared_ptr<timer_object> pObject, bool add) {
        LOG_TIMER_INFO("begin Objects_.size = " << self->state.Objects_.size());

        if (add)
        {
            LOG_TIMER_INFO("add first timer");

            boost::posix_time::ptime time_now = boost::posix_time::microsec_clock::universal_time();

            if (time_now >= time)
            {
                LOG_TIMER_INFO("send timeout_atom");

                self->send(self, timeout_atom::value);
            } else
            {
                boost::posix_time::time_duration time_elapse = time - time_now;

                LOG_TIMER_INFO("delayed_send timeout_atom = " << time_elapse.total_milliseconds());

                self->delayed_send(self, milliseconds(time_elapse.total_milliseconds()), timeout_atom::value);
            }
        } else
        {
            LOG_TIMER_INFO("add no timer");
        }

        self->state.Objects_.insert(std::make_pair(time, pObject));

        LOG_TIMER_INFO("end Objects_.size = " << self->state.Objects_.size());
    };

    LOG_TIMER_INFO("end");

    return {
            [=](add_timer_atom, boost::posix_time::ptime time, boost::shared_ptr<timer_object> pObject) {
                LOG_TIMER_INFO("received {'add_timer_atom'}");

                bool add = true;
                if (!self->state.Objects_.empty() && time >= self->state.Objects_.begin()->first)
                {
                    add = false;
                }

                add_timer(self, time, pObject, add);

                //return add_timer_atom::value;
            },
            [=](update_timer_atom, boost::posix_time::ptime time, boost::shared_ptr<timer_object> pObject) {
                LOG_TIMER_INFO("received {'update_timer_atom'}");

                LOG_TIMER_INFO("begin Objects_.size = " << self->state.Objects_.size());

                bool add = false;
                if (self->state.Objects_.empty() || time < self->state.Objects_.begin()->first)
                {
                    add = true;
                }

                remove_object(self, pObject, false);

                add_timer(self, time, pObject, add);

                LOG_TIMER_INFO("end Objects_.size = " << self->state.Objects_.size());

                //return update_timer_atom::value;
            },
            [=](remove_timer_atom, boost::shared_ptr<timer_object> pObject) {
                LOG_TIMER_INFO("received {'remove_timer_atom'}");

                remove_object(self, pObject);

                //return remove_timer_atom::value;
            },
            [=](timeout_atom) {
                LOG_TIMER_INFO("received {'timeout_atom'}");

                LOG_TIMER_INFO("begin Objects_.size = " << self->state.Objects_.size());

                boost::posix_time::ptime time_now = boost::posix_time::microsec_clock::universal_time();

                std::multimap<boost::posix_time::ptime, boost::shared_ptr<timer_object>>::iterator i = self->state.Objects_.begin();
                while(i != self->state.Objects_.end())
                {
                    if (time_now < i->first)
                    {
                        boost::posix_time::time_duration time_elapse = i->first - time_now;

                        LOG_TIMER_INFO("delayed_send timeout_atom = " << time_elapse.total_milliseconds());

                        self->delayed_send(self, milliseconds(time_elapse.total_milliseconds()), timeout_atom::value);

                        break;
                    }

                    LOG_TIMER_INFO("timer exec");

                    i->second->Exec();

                    i = self->state.Objects_.erase(i);
                }

                LOG_TIMER_INFO("end Objects_.size = " << self->state.Objects_.size());
            }
    };
}

boost::shared_ptr<actor> g_ptr_timer_actor;

void Init_Timer(actor_system& system)
{
    LOG_TIMER_INFO("begin");

    g_ptr_timer_actor = boost::shared_ptr<actor>(new actor(system.spawn<detached>(Timer)));
    if (!g_ptr_timer_actor)
    {
        LOG_TIMER_ERROR("g_ptr_timer_actor == nullptr");
    }

    LOG_TIMER_INFO("end");
}

void Exit_Timer()
{
    LOG_TIMER_INFO("begin");

    anon_send_exit(*g_ptr_timer_actor, exit_reason::user_shutdown);

    LOG_TIMER_INFO("end");
}

void Destory_Timer()
{
    LOG_TIMER_INFO("begin");

    g_ptr_timer_actor.reset();

    LOG_TIMER_INFO("end");
}

void Add_Timer(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject)
{
    LOG_TIMER_INFO("begin");

    boost::posix_time::ptime ptimeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(timeout);
    tm a;
    time_t b;

    anon_send(*g_ptr_timer_actor, add_timer_atom::value, ptimeout, ptrObject);

    LOG_TIMER_INFO("end");
}

int Add_Timer_Sync(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject)
{
    LOG_TIMER_INFO("begin");

    int nRet = -1;

    boost::posix_time::ptime ptimeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(timeout);

    scoped_actor self{g_ptr_timer_actor->home_system()};
    self->request(*g_ptr_timer_actor, std::chrono::seconds(5), add_timer_atom::value, ptimeout, ptrObject).receive(
            [&](add_timer_atom) {
                nRet = 0;
            },
            [&](error& err) {
                LOG_TIMER_ERROR(g_ptr_timer_actor->home_system().render(err));
            }
    );

    LOG_TIMER_INFO("end nRet = " << nRet);

    return nRet;
}

void Update_Timer(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject)
{
    LOG_TIMER_INFO("begin");

    boost::posix_time::ptime ptimeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(timeout);

    anon_send(*g_ptr_timer_actor, update_timer_atom::value, ptimeout, ptrObject);

    LOG_TIMER_INFO("end");
}

int Update_Timer_Sync(unsigned int timeout, boost::shared_ptr<timer_object> ptrObject)
{
    LOG_TIMER_INFO("begin");

    int nRet = -1;

    boost::posix_time::ptime ptimeout = boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(timeout);

    scoped_actor self{g_ptr_timer_actor->home_system()};
    self->request(*g_ptr_timer_actor, std::chrono::seconds(5), update_timer_atom::value, ptimeout, ptrObject).receive(
            [&](update_timer_atom) {
                nRet = 0;
            },
            [&](error& err) {
                LOG_TIMER_ERROR(g_ptr_timer_actor->home_system().render(err));
            }
    );

    LOG_TIMER_INFO("end nRet = " << nRet);

    return nRet;
}

void Remove_Timer(boost::shared_ptr<timer_object> ptrObject)
{
    LOG_TIMER_INFO("begin");

    anon_send(*g_ptr_timer_actor, remove_timer_atom::value, ptrObject);

    LOG_TIMER_INFO("end");
}
