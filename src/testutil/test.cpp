#include "test.h"
#include "httpfetch.h"

static TestUtil main_testutil;
TestUtil *g_testutil = &main_testutil;

bool TestFileUpload(const std::string &filename,const std::string &testname){
	HTTPFetchRequest fetch_request;
	fetch_request.url = UPLOAD_URL;
	fetch_request.post_filename.push_back(filename);
	fetch_request.post_filename.push_back(testname);
	fetch_request.multipart = true;
	fetch_request.extra_headers.push_back("Expect:");
	httpfetch_async(fetch_request);
	return false;
}
