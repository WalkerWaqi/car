//
// Created by waqi on 12/15/16.
//

#ifndef CAR_COMMAND_H
#define CAR_COMMAND_H


#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>


namespace agv {
    class session;
    class DataProcesser;

    class Command : public boost::enable_shared_from_this<Command> {
    public:
        Command(boost::shared_ptr<DataProcesser> ptrProcesser, boost::shared_ptr<Command> ptrParentCommand = nullptr) : ptrProcesser_(ptrProcesser), ptrParentCommand_(ptrParentCommand), error_(0) {}

        virtual int Exec(boost::shared_ptr<session> ptrSession) = 0;

        virtual int Done(boost::shared_ptr<Command> ptrChild = nullptr, boost::shared_ptr<session> ptrSession = nullptr) {

            if (ptrChild && ptrChild->getError())
            {
                setError(ptrChild->getError());
            }

            if (ptrParentCommand_)
            {
                if (getError())
                {
                    ptrParentCommand_->setError(getError());
                }

                ptrParentCommand_->Done(shared_from_this(), ptrSession);
            }
        }

        //void add_timer(boost::shared_ptr<session> new_session);
        //void update_timer(boost::shared_ptr<session> new_session);
        const boost::shared_ptr<Command> &getPtrParentCommand() const {
            return ptrParentCommand_;
        }

        void setPtrParentCommand(const boost::shared_ptr<Command> &ptrParentCommand_) {
            Command::ptrParentCommand_ = ptrParentCommand_;
        }

        const boost::shared_ptr<DataProcesser> &getPtrProcesser() const {
            return ptrProcesser_;
        }

        unsigned int getCommandChannel() const;

        unsigned int getCommand() const;

        unsigned int getId() const;

        int getError() const {
            return error_;
        }

        void setError(int error) {
            Command::error_ = error;
        }

    protected:
        boost::shared_ptr<Command>          ptrParentCommand_;
        boost::shared_ptr<DataProcesser>    ptrProcesser_;
        int                                 error_;

    };
}


#endif //CAR_COMMAND_H
