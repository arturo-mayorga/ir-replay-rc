#include "../ecs.h"

#include <Windows.h>
#include <XInput.h>

#include <conio.h>

#include <gtest/gtest.h>
#include "../systems/scraper-system.h"

class TestScraperSystem : public ScraperSystem
{
public:
    std::stringstream _sstream;

    void setSessions(std::vector<SessionSP> sessions)
    {
        _sessions = sessions;
    }

    std::string getString()
    {
        return _sstream.str();
    }

    virtual std::ostream &getOutStream();
    virtual void closeOutStream();
    void resetStream()
    {
        this->_sstream.str("");
        this->_sstream.clear();
    }
};

std::ostream &TestScraperSystem::getOutStream()
{
    return this->_sstream;
}

void TestScraperSystem::closeOutStream() {}

TEST(ScraperSystemTests, MainScraperTests)
{

    int success = 0;
    int fail = 0;

    TestScraperSystem *sys = new TestScraperSystem();

    OnSaveTelemetryRequest e;

    std::vector<SessionSP> testSessions;

    sys->setSessions(testSessions);

    sys->receive(NULL, e);

    EXPECT_EQ(sys->getString(), "[]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    SessionSP ses1(new Session);
    ses1->id = 321;

    testSessions.push_back(ses1);
    sys->setSessions(testSessions);

    sys->resetStream();
    sys->receive(NULL, e);

    EXPECT_EQ(sys->getString(), "[]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    DriverSP d1(new Driver);
    d1->uid = 123;
    ses1->drivers.push_back(d1);

    sys->setSessions(testSessions);
    sys->resetStream();
    sys->receive(NULL, e);

    EXPECT_EQ(sys->getString(), "[{\"id\":321,\"drivers\":[{\"id\":123,\"laps\":[]}]}]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    SessionSP ses2(new Session);
    ses2->id = 432;

    DriverSP d2(new Driver);
    d2->uid = 234;
    ses2->drivers.push_back(d2);

    testSessions.push_back(ses2);

    sys->setSessions(testSessions);
    sys->resetStream();
    sys->receive(NULL, e);

    EXPECT_EQ(sys->getString(), "[{\"id\":321,\"drivers\":[{\"id\":123,\"laps\":[]}]},{\"id\":432,\"drivers\":[{\"id\":234,\"laps\":[]}]}]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
}