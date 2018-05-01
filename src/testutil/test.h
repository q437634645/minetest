#ifndef LOGTEST
#define LOGTEST

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cerrno>
#include "exceptions.h"
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
	~TestCase(){}
	std::string stop(){
		std::ostringstream os(std::ios_base::binary);
		os<<"TestCase = "<<(int)type<<" "
			"startTime = "<<startTime<<" "
			"endTime = "<<porting::getTimeMs()<<std::endl;
		return os.str();
	}
private:
	u64 startTime;
	TestCaseType type;
};

class TestUtil{
public:
	TestUtil():
		active(false)
	{
		m_testcase.clear();
	}
	void SetActive(bool _active){
		active = _active;
	}
	void CreateTestCase(TestCaseType type){

		if(!active)return;

		m_testcase.push_back(TestCase(type));
	}
	void FinishTestCase(){

		if(!active)return;

		std::vector<TestCase>::iterator i = m_testcase.begin();
		if( i == m_testcase.end())
			return;
		m_stream<<i->stop();
		m_testcase.erase(i);
	}
	void init(const std::string &filename){

		if(!active)return;

		m_stream.open(filename.c_str(), std::ios::out);
		if (!m_stream.good())
			throw FileNotGoodException("Failed to open test file " +
				filename + ": " + strerror(errno));
	}
private:
	bool active;
	std::ofstream m_stream;
	std::vector<TestCase>m_testcase;
};


#endif
