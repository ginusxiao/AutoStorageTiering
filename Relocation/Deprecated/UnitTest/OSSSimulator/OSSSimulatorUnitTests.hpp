#ifndef __OSS_SIMULATOR_UNIT_TESTS_HPP__
#define __OSS_SIMULATOR_UNIT_TESTS_HPP__

#include "gtest/gtest.h"
#include <pthread.h>
#include <time.h>
#include "AST/OSSSimulator.hpp"

/*
 * !!!!!!!!!!!!!!!!!!!!!!   NOTE  !!!!!!!!!!!!!!!!!!!!!!!
 * OSSSimulator is responsible for integration test before
 * integrating AST into OSS(HUB part), and as integrating
 * with OSS cause some code change, OSSSimulator will not
 * work fine any more, but it can be refactored to be work
 * again, refactor will be later...
 * */
class OSSSimulatorTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class OSSSimulatorTest: public testing::Test
{
public:
    OSSSimulatorTest()
    {
        mOSSSim = new OSSSimulator();
        YWB_ASSERT(mOSSSim != NULL);
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&cond, NULL);
    }

    virtual ~OSSSimulatorTest()
    {
        if(mOSSSim)
        {
//            /*
//             * FIXME: temporarily comment out to avoid core dump
//             * caused by ControlCenter::StopMonClient
//             **/
            delete mOSSSim;
            mOSSSim = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();
    void Lock();
    void Unlock();
    void TimedWait(struct timespec* timeout);

private:
    OSSSimulator* mOSSSim;
    pthread_cond_t cond;
    pthread_mutex_t lock;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
