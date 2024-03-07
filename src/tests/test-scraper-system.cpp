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

    EXPECT_EQ(sys->getString(), "[{\"id\":321,\"drivers\":[{\"id\":123,\"laps\":[]}],\"epochTelemetry\":{\"numLaps\":0,\"checkeredFlagTime\":0,\"epochList\":[]}}]");

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

    EXPECT_EQ(sys->getString(), "[{\"id\":321,\"drivers\":[{\"id\":123,\"laps\":[]}],\"epochTelemetry\":{\"numLaps\":0,\"checkeredFlagTime\":0,\"epochList\":[]}},{\"id\":432,\"drivers\":[{\"id\":234,\"laps\":[]}],\"epochTelemetry\":{\"numLaps\":0,\"checkeredFlagTime\":0,\"epochList\":[]}}]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    SessionSP ses3(new Session);
    ses3->id = 432;

    ses3->drivers.push_back(d2);
    ses3->epochTelemetry->numLaps = 3;
    ses3->epochTelemetry->checkeredFlagTime = 123;

    auto eTelemData = EpochTelemetryDataSP(new EpochTelemetryData);
    eTelemData->time = 321;
    auto datum1 = EpochTelemetryDriverDatumSP(new EpochTelemetryDriverDatum);
    datum1->uid = 234;
    datum1->percentPos = 0.5;
    datum1->percentPosDelta = 0.1;
    auto datum2 = EpochTelemetryDriverDatumSP(new EpochTelemetryDriverDatum);
    datum2->uid = 34;
    datum2->percentPos = 0.52;
    datum2->percentPosDelta = 0.21;

    eTelemData->data.push_back(datum1);
    eTelemData->data.push_back(datum2);
    ses3->epochTelemetry->epochList.push_back(eTelemData);

    testSessions.clear();
    testSessions.push_back(ses3);

    sys->setSessions(testSessions);
    sys->resetStream();
    sys->receive(NULL, e);

    EXPECT_EQ(sys->getString(), "[{\"id\":432,\"drivers\":[{\"id\":234,\"laps\":[]}],\"epochTelemetry\":{\"numLaps\":3,\"checkeredFlagTime\":123,\"epochList\":[{\"time\":321,\"data\":[{\"uid\":234,\"percentPos\":0.5,\"percentPosDelta\":0.1},{\"uid\":34,\"percentPos\":0.52,\"percentPosDelta\":0.21}]}]}}]");

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
}