#ifndef LOGTEST
#define LOGTEST

#include <iostream>
#include <vector>
#include "porting.h"

class TestUtil;
extern TestUtil *g_testutil;

enum TestCaseType{
	TCT_PlayerPos,
	TCT_MAX,
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

class TestUtil{
public:
	TestUtil(){
		m_testcase.clear();
	}
	void CreateTestCase(TestCaseType type){
		m_testcase.push_back(TestCase(type));
	}
	void FinishTestCase(){
		std::vector<TestCase>::iterator i = m_testcase.begin();
		i->stop();
		m_testcase.erase(i);
	}
private:
	std::vector<TestCase>m_testcase;
};


#endif
