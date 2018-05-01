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
#include "gettime.h"
#include "filesys.h"

class TestUtil;
extern TestUtil *g_testutil;

#define TESTFILE "Test"

class TestUtil{
public:
	TestUtil():
		active(false),finished(false)
	{
	}
	void SetActive(bool active){
		m_active = active;
		m_active_time = porting::getTimeMs();
		filename = dirname + DIR_DELIM + getTimestamp() + ".txt";
		m_stream.open(filename,std::ios::out);
		if(!m_stream.good()){
			throw FileNotGoodException("Failed to open test file " +
				filename + ": " + strerror(errno));
		}
	}
	void init(const std::string &dirname){
		this->dirname = dirname;
		/*
		if(m_stream.isopen())m_stream.close();
		m_stream.open(filename.c_str(), std::ios::out);
		if (!m_stream.good())
			throw FileNotGoodException("Failed to open test file " +
				filename + ": " + strerror(errno));
		*/
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
			m_average[name]=std::make_pair(numerator,denominator);
		}
		else {
			std::pair<float,float>&p=i->second;
			p.first+=numerator;
			p.second+=denominator;
		}
	}
	void Output(){
		if(porting::getTimeMs() - m_active_time < 100 || finished)return;
		if(!m_active){
			return;
		}
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
		Clear();
	}
	void Clear(){
		m_active = false;
		m_count.clear();
		m_average.clear();
	}
	void ~TestUtil(){
		finished = true;
		Output();
	}
private:
	std::ofstream m_stream;
	std::map<std::string,int>m_count;
	std::map<std::string,std::pair<float,float> >m_average;
	std::string dirname;
	std::string filename;
	bool m_active;
	u64 m_active_time;
	bool finished;
};


enum TestCaseType{
	TCT_AVG,
	TCT_COUNT,
};

class TestCase{
public:
	TestCase(TestUtil *testutil,const std::string& name,TestCaseType type)
		:m_testutil(testutil),m_name(name),m_type(type)
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
	TestUtil *m_testutil;
	std::string m_name;
	TestCaseType m_type;
	u64 m_starttime;
};

#endif
