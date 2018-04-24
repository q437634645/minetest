#ifndef LOGTEST
#define LOGTEST

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cerrno>
#include "porting.h"

class TestUtil;
extern TestUtil *g_testutil;

enum TestCaseType{
	TCT_PlayerPos,
	TCT_MAX,
};

class TestCase{
public:
	TestCase(TestCaseType tct,std::ostream &stream):
		m_stream(stream),
		type(tct)
	{
		startTime = porting::getTimeMs();
	}
	~TestCase(){
		if(!m_stream.good())
			return;
		m_stream<<"TestCase = "<<(int)type<<"startTime ="<<startTime
			<<"finishTime = "<<porting::getTimeMs()<<std::endl;
	}
	void stop(){
		if(!m_stream.good())
			return;
		m_stream<<"TestCase = "<<(int)type<<"startTime ="<<startTime
			<<" finishTime = "<<porting::getTimeMs()<<std::endl;
	}
private:
	std::ostream &m_stream;
	float startTime;
	TestCaseType type;
};

class TestUtil{
public:
	TestUtil(){
		m_testcase.clear();
	}
	void CreateTestCase(TestCaseType type){
		m_testcase.push_back(TestCase(typem m_stream));
	}
	void FinishTestCase(){
		std::vector<TestCase>::iterator i = m_testcase.begin();
		if( i == m_testcase.end())
			return;
		i->stop();
		m_testcase.erase(i);
	}
	void init(const std::string &filename){
		m_stream.open(filename.c_str(), std::ios::out);
		if (!m_stream.good())
			throw FileNotGoodException("Failed to open test file " +
				filename + ": " + strerror(errno));
	}
private:
	std::ofstream m_stream;
	std::vector<TestCase>m_testcase;
};


#endif
