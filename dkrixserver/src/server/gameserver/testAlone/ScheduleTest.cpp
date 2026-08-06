#include "ScheduleTest.h"

#include <unistd.h>

#include <queue>
#include <vector>

#include "Schedule.h"
#include "Scheduler.h"
#include "Work.h"

class TestWork : public Work {
public:
    ~TestWork() {}

public:
    bool test;

public:
    TestWork() {
        test = false;
    }
    void execute() {
        test = true;
    }

    string toString() const throw(Error) {
        return "TestWork";
    }
};

void ScheduleTest::setUp() {}

void ScheduleTest::tearDown() {}

void ScheduleTest::testSchedule() {
    __BEGIN_TEST_CASE

    VSDateTime::setCurrentDateTime(0); 
    VSDateTime dt(100);                

    TestWork* pWork = new TestWork();
    Schedule* pSchedule = new Schedule(pWork, dt);

    CPPUNIT_ASSERT(pWork != NULL);
    CPPUNIT_ASSERT(pSchedule != NULL);

    CPPUNIT_ASSERT(!pSchedule->heartbeat());
    CPPUNIT_ASSERT(!pWork->test);

    VSDateTime::setCurrentDateTime(50); 

    CPPUNIT_ASSERT(!pSchedule->heartbeat()); 
    CPPUNIT_ASSERT(!pWork->test);

    VSDateTime::setCurrentDateTime(101); 

    CPPUNIT_ASSERT(pSchedule->heartbeat()); 
    CPPUNIT_ASSERT(pWork->test);

    SAFE_DELETE(pSchedule);

    __END_TEST_CASE
}

void ScheduleTest::testScheduler() {
    __BEGIN_TEST_CASE

    VSDateTime::setCurrentDateTime(0);
    Scheduler* pScheduler = new Scheduler();

    TestWork* pWork1 = new TestWork();
    Schedule* pSchedule1 = new Schedule(pWork1, VSDateTime(100)); 

    pScheduler->addSchedule(pSchedule1);

    TestWork* pWork2 = new TestWork();
    Schedule* pSchedule2 = new Schedule(pWork2, VSDateTime(200));

    TestWork* pWork = NULL;

    pScheduler->addSchedule(pSchedule2);

    CPPUNIT_ASSERT(pScheduler->getSize() == 2); 

    pScheduler->heartbeat();

    CPPUNIT_ASSERT(!pWork1->test);
    CPPUNIT_ASSERT(!pWork2->test);

    VSDateTime::setCurrentDateTime(50); 

    pScheduler->heartbeat();
    CPPUNIT_ASSERT(pScheduler->getSize() == 2);

    CPPUNIT_ASSERT(!pWork1->test); 
    CPPUNIT_ASSERT(!pWork2->test);

    VSDateTime::setCurrentDateTime(101); 

    pWork = dynamic_cast<TestWork*>(pScheduler->heartbeat());
    CPPUNIT_ASSERT(pWork1 == pWork);
    CPPUNIT_ASSERT(pScheduler->getSize() == 1); 

    CPPUNIT_ASSERT(pWork1->test); 
    CPPUNIT_ASSERT(!pWork2->test);

    VSDateTime::setCurrentDateTime(151); 

    pScheduler->heartbeat();
    CPPUNIT_ASSERT(pScheduler->getSize() == 1); 

    CPPUNIT_ASSERT(pWork1->test);
    CPPUNIT_ASSERT(!pWork2->test);

    VSDateTime::setCurrentDateTime(201); 

    pWork = dynamic_cast<TestWork*>(pScheduler->heartbeat());

    CPPUNIT_ASSERT(pWork2 == pWork);
    CPPUNIT_ASSERT(pScheduler->getSize() == 0); 

    CPPUNIT_ASSERT(pWork1->test);
    CPPUNIT_ASSERT(pWork2->test);

    SAFE_DELETE(pScheduler);

    __END_TEST_CASE
}

class Less {
public:
    bool operator()(int i1, int i2) {
        return i1 > i2;
    }
};

void ScheduleTest::testPQ() {
    priority_queue<int, vector<int>, Less> pq;

    pq.push(5);
    pq.push(2);
    pq.push(6);
    pq.push(4);
    pq.push(3);
    pq.push(1);
    pq.push(7);

    CPPUNIT_ASSERT(pq.top() == 1);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 2);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 3);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 4);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 5);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 6);
    pq.pop();
    CPPUNIT_ASSERT(pq.top() == 7);
    pq.pop();
    CPPUNIT_ASSERT(pq.empty());
}
