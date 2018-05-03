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
#include <ctime>
#include "exceptions.h"
#include "porting.h"
#include "gettime.h"
#include "filesys.h"

class TestUtil;
extern TestUtil *g_testutil;

#define TESTFILE "Test"

class TestRecord{
public:
	int m_time;// record time in ms
	float m_value;// record value
	TestRecord(int t,float v):m_time(t),m_value(v){}
	friend ostream& operator<<(ostream &out,const TestRecord &data);
}

std::ostream& operator<<(std::ostream &out,const TestRecord &data){
	out<<data.m_time<<' '<<data.m_value;
}

class TestUtil{
public:
	TestUtil():m_active(false),m_finishing(false){}
	std::string GetTimeString(){
		// get string of time of fromat YYMMDDHHMMSS
		std::time_t result = std::time(NULL);
		std::tm *tm = gmtime(result);
		char cs[20];
		strftime(cs, 20, "%Y%m%d%H%M%S",tm);
		return cs;
	}
	void Begin(){
		if(m_active)return;
		m_active_time = porting::getTimeMs();
		testdir = dirroot + DIR_DELIM + GetTimeString();
		if(!fs::PathExists(testdir)){
			fs::CreateDir(testdir);
		}
		Clear(); // be sure container be empty
		m_active = true;
	}
	void Finish(){
		if(!m_active)return;
		m_finishing=true;// set finishing flag
		OutputToFile();
		m_finishing=false;
		m_active=false;
	}
	bool isActive(){
		return m_active;
	}
	void init(const std::string &dirroot){
		this->dirroot = dirroot;
	}
	void OutputString(const std::string &str){
		m_stream<<str<<std::endl;
	}
	void Count(const std::string &name, int value=1){
		if(!m_active)return;
		std::map<std::string,int>::iterator i;
		i=m_count.find(name);
		if(i==m_count.end())
			m_count[name]=value;
		else i->second+=value;
	}
	// Record Special Data
	void Avg(const std::string &name, float numerator,float denominator){
		if(!m_active)return;
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
	// Record All Data To Cache
	void RecordAll(){
		if(porting::getTimeMs() - m_active_time < 100 || m_finishing)return;
		if(!m_active)return;
		int curtime = porting::getTimeMs();
		RecordCount(curtime);
		RecordAverage(curtime);
		Clear();
	}
	void RecordCount(int curtime){
		std::map<std::string,int>::iterator i;
		for(i=m_count.begin();i!=m_count.end();i++){
			CacheRecord(i->first,curtime,i->second);
		}
	}
	void RecordAverage(int curtime){
		std::map<std::string,std::pair<float,float> >::iterator i;
		for(i=m_average.begin();i!=m_average.end();i++){
			float value=i->second.first;
			if(i->second.second>1e-6)value/=i->second.second;
			CacheRecord(i->first,curtime,value);
		}
	}
	void Clear(){
		m_count.clear();
		m_average.clear();
		m_cache.clear();
	}
	~TestUtil(){
		Finish();
	}
private:
	void CacheRecord(const std::string name,const int &t,const int &v){
		m_cache[name].push_back(TestRecord(t,v));
	}
	void OutputToFile(){
		// Record duration limit min 100ms
		if(!m_active)return;
		std::map<std::string,std::vector<TestRecord> >::iterator i;
		for(i=m_cache.begin();i!=m_cache.end();i++){
			// open different file for store
			if(m_stream.is_open())m_stream.close();
			filename = testdir + DIR_DELIM + i->first + ".txt";
			m_stream.open(filename.c_str(),std::ios::app|std::ios::ate);
			if(!m_stream.good())
				throw FileNotGoodException("Failed to open test file " +
					filename + ": " + strerror(errno));
			
			std::vector<TestRecord>::iterator j;
			std::vector<TestRecord>&record=i->second;
			for(j=record.begin();j!=record.end();j++){
				m_stream<<*j<<std::endl;
			}
		}
	}
	std::ofstream m_stream;
	std::map<std::string,int>m_count;
	std::map<std::string,std::pair<float,float> >m_average;
	std::map<std::string,std::vector<TestRecord> >m_cache;
	std::string dirroot;
	std::string testdir;
	std::string filename;
	bool m_active;
	u64 m_active_time;
	bool m_finishing;
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
