#ifndef TCPServerTest_INCLUDED
#define TCPServerTest_INCLUDED


#include "Poco/Net/Net.h"
#include "CppUnit/TestCase.h"


class TCPServerTest
{
public:
	TCPServerTest(const std::string& name);
	~TCPServerTest();

	void setUp();
	void tearDown();

};


#endif // TCPServerTest_INCLUDED