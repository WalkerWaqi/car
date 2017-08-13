#ifndef BASE_STATE_H
#define BASE_STATE_H


#include <caf/all.hpp>
#include <caf/io/all.hpp>
using namespace caf;
#include "logger.h"

#define S_PRINT(logEvent) \
LOG4CPLUS_INFO(g_Actor_Logger, name << " (id = " << self->id() << "): " << logEvent);
#define A_PRINT(logEvent) \
LOG4CPLUS_INFO(g_Actor_Logger, self->state.name << " (id = " << self->id() << "): " << logEvent);

using add_timer_atom = caf::atom_constant<caf::atom("addtimer")>;
using update_timer_atom = caf::atom_constant<caf::atom("updattimer")>;
using remove_timer_atom = caf::atom_constant<caf::atom("removtimer")>;
using timeout_atom = caf::atom_constant<caf::atom("timeout")>;

struct base_state {
    base_state(local_actor *thisptr) : self(thisptr) {
        S_PRINT("base_state");
    }

    //actor_ostream print() {
    //    return aout(self) << name << " (id = " << self->id() << "): ";
    //}

    virtual bool init(std::string m_name) {
        name = std::move(m_name);
        S_PRINT("started");
        return true;
    }

    virtual ~base_state() { S_PRINT("done"); }

    unsigned short get_packet_id() {
        return packetId_ += 2;
    }

    local_actor *self;
    std::string name;
    unsigned short  packetId_ = 0;
};


#endif // BASE_STATE_H
