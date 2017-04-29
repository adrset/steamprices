#pragma once
#include <curl/curl.h>
#include <string>
class Httpc
{
	CURLM* multi_handle;
	int handle_count;

public:
	Httpc();
	~Httpc();
	void Update();
	void AddRequest(const char* uri, std::string id);
};

