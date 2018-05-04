#include "test.h"
#include "httpfetch.h"

static TestUtil main_testutil;
TestUtil *g_testutil = &main_testutil;

bool TestFileUpload(const std::string &filename,const std::string &testname){
	HTTPFetchRequest fetch_request;
	fetch_request.url = UPLOAD_URL;
	fetch_request.post_fields["name"] = "@" + filename;
	fetch_request.post_fields["TestName"] = testname;
	fetch_request.multipart = true;
	httpfetch_async(fetch_request);
	return false;
}
