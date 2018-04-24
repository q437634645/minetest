#ifndef LOGTEST
#define LOGTEST

#include "porting.h"
#include <iostream>
#include <vector>

class TestUtil;
class TestCase;
extern TestUtil g_testutil;

enum TestCaseType{
	TCT_PlayerPos,
	TCT_MAX,
};

class TestUtil{
public:
	TestUtil(){
		m_testcase.clear();
	}
	void CreateTestCase(TestCaseType type){
		m_testcase.insert(TestCase(type));
	}
	void FinishTestCase(){
		std::vector<TestCase>::const_iterator i = m.begin();
		i->stop();
		m_testcase.erase(i);
	}
private:
	std::vector<TestCase>m_testcase;
};

class TestCase{
public:
	TestCase(TestCaseType tct):type(tct){
		startTime = porting::getTimeMs();
	}
	~TestCase(){
		std::cout<<"TestCase = "<<(int)type<<"startTime ="<<startTime
			<<"finishTime = "<<porting::getTimeMs()<<std::endl;
	}
	void stop(){
		std::cout<<"TestCase = "<<(int)type<<"startTime ="<<startTime
			<<"finishTime = "<<porting::getTimeMs()<<std::endl;
	}
private:
	float startTime;
	TestCaseType type;
};

#endif
