//
// Created by waqi on 12/20/16.
//

#ifndef CAR_TIMER_OBJECT_H
#define CAR_TIMER_OBJECT_H


#include <boost/shared_ptr.hpp>


class timer_object {
public:
    virtual int Exec() = 0;

    virtual int equal(boost::shared_ptr<timer_object>& ptrObject) = 0;
};


#endif //CAR_TIMER_OBJECT_H
