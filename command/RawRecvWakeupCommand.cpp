//
// Created by waqi on 1/11/17.
//

#include "RawRecvWakeupCommand.h"
#include "Tools.h"


namespace agv {
    CAF_ALLOW_UNSAFE_MESSAGE_TYPE(boost::shared_ptr<session>);

    int RawRecvWakeupCommand::Exec(boost::shared_ptr<session> ptrSession) {
        LOG_COMMAND_INFO("begin");

        {
            std::string strFile = CAR_DATA_PATH;
            strFile += "/";
            strFile += CAR_DATA_CONFIG;
            std::string strData;
            Tools::read_file(strFile, strData);
            setData(strData);
        }
        getPtrProcesser()->setCommand(COMMAND_ACK);
        int ret = RawSendDataCommand::Exec(ptrSession);

        if (ret > 0)
        {
            ptrSession->remove_timer();

            boost::dynamic_pointer_cast<packet_session>(ptrSession)->setWeakup(true);

            anon_send(*g_ptr_raw_actor, set_session_atom::value, ptrSession);
        }

        LOG_COMMAND_INFO("end ret = " << ret);

        return ret;
    }
}