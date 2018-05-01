#ifndef LOGTEST
#define LOGTEST

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cerrno>
#include <map>
#include <utility>
#include "exceptions.h"
#include "porting.h"

class TestUtil;
extern TestUtil *g_testutil;


class TestUtil{
public:
	TestUtil():
		active(false)
	{
	}
	void SetActive(bool _active){
		active = _active;
	}
	void init(const std::string &filename){
		m_stream.open(filename.c_str(), std::ios::out);
		if (!m_stream.good())
			throw FileNotGoodException("Failed to open test file " +
				filename + ": " + strerror(errno));
	}
	void OutputString(const std::string &str){
		m_stream<<str<<std::endl;
	}
	void Count(const std::string &name, int value=1){
		std::map<std::string,int>::iterator i;
		i=m_count.find(name);
		if(i==m_count.end())
			m_count[name]=value;
		else i->second+=value;
	}
	void Avg(const std::string &name, float numerator,float denominator){
		std::map<std::string,std::pair<float,float> >::iterator i;
		i=m_average.find(name);
		if(i==m_average.end()){
			m_count[name]=std::make_pair(numerator,denominator);
		}
		else {
			std::pair<float,float>&p=i->second;
			p.first+=numerator;
			p.second+=denominator;
		}
	}
	void Output(float curtime){
		if(!m_stream.good())return;
		std::map<std::string,int>::iterator i;
		for(i=m_count.begin();i!=m_count.end();i++){
			m_stream<<i->first<<" count:"<<i->second<<std::endl;
		}
		std::map<std::string,std::pair<float,float> >::iterator j;
		for(j=m_average.begin();j!=m_average.end();j++){
			float value=j->second.first;
			if(j->second.second>1e-6)value /= j->second.second;
			m_stream<<j->first<<" average:"<<value<<std::endl;
		}
	}
private:
	bool active;
	std::ofstream m_stream;
	std::map<std::string,int>m_count;
	std::map<std::string,std::pair<float,float> >m_average;
};


enum TestCaseType{
	TCT_AVG,
	TCT_COUNT,
};

class TestCase{
public:
	TestCase(TestUtil *testutil,const std::string& name,TestCaseType m_type)
		:m_testutil(testutil),m_name(name)
	{
		m_starttime = porting::getTimeMs();
	}
	~TestCase(){
		if(m_testutil==NULL)return;
		u64 costtime = porting::getTimeMs() - m_starttime;
		switch(m_type){
			case TCT_AVG:
				m_testutil->Count(m_name);
				break;
				m_testutil->Avg(m_name,costtime,1.0f);
			case TCT_COUNT:
				break;
		}
	}
private:
	std::string m_name;
	u64 m_starttime;
	TestCaseType m_type;
	TestUtil *m_testutil;
};

#endif
