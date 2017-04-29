#include "steamItem.h"
#include <iostream>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/filereadstream.h>
#include "Errors.h"


using namespace rapidjson;


//MISC - not member of class steamItem

size_t inline write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

std::string removeChar(char to_rem, std::string s) {
	size_t pos = s.find(to_rem);
	if(pos != std::string::npos)s.erase(pos, std::count(s.begin(), s.end(), to_rem));
	return s;
}

// end od MISC

int steamItem::m_numMarketable = 0;
int steamItem::m_numAll = 0;
int steamItem::m_fetchedPrices = 0;
bool steamItem::m_tfBackpackDownloaded = false;

steamItem::steamItem(float price, std::string name, int amount, std::string url,
	int market, int trade, bool hError, std::string hashName ):
	m_price(price), m_name(name), m_amount(amount), m_url(url),
	m_marketable(market), m_tradable(trade), m_parseError(hError), m_hashName(hashName){

	if (m_marketable == 0) {
		m_numAll++;
	}
	if (m_marketable == 1) {
		m_numMarketable++;
	}
}

steamItem::~steamItem() {}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void steamItem::setPriceSteam(){
	//very uneffective!
	if (m_marketable == 1) {
		CURL *curl;
		CURLcode res;
		std::string readBuffer;
		Document d;

		struct curl_slist *headers = NULL;
		curl_slist_append(headers, "Accept: application/json");
		curl_slist_append(headers, "Content-Type: application/json");
		curl_slist_append(headers, "charsets: utf-8");
		curl = curl_easy_init();
		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, m_url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);


			if (d.Parse<0>(readBuffer.c_str()).HasParseError()) {
				m_parseError = true;
			}
			else {
				const Value& success = d["success"];
				if (!success.GetBool()) {
					std::cout << "Error fetching price for item :" << m_name << std::endl;
				}
				else {
					//std::cout << removeChar('$', d["lowest_price"].GetString()) << std::endl;
					m_price = std::stof(removeChar('$', d["lowest_price"].GetString()));
					m_fetchedPrices++;
				}
			}

		}
		Sleep(100);
	}
}

void steamItem::downloadBackpackTF(){
	CURL *curl;
	CURLcode res;
	FILE *fp;
	std::string key = "{KEY}";
	curl = curl_easy_init();
	std::string url = "http://backpack.tf/api/IGetMarketPrices/v1?key=" + key + "&appid=730";
	if (curl) {
		fp = fopen("items.json", "w");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		if (res == 0) {
			std::cout << "Item prices downloaded!" << std::endl;
			m_tfBackpackDownloaded = true;
		}
		else {
			fatalError("Could not download item prices... Sorry!");
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}
}

void steamItem::setPriceBacpackTF(){
	if (!m_tfBackpackDownloaded) {
		downloadBackpackTF();
	}
	if (m_marketable == 1) {
		FILE* fp = fopen("items.json", "r");
		if (fp == NULL) {
			fatalError("Could not open file: prices.json");
		}
		long lSize;

		fseek(fp, 0, SEEK_END);
		lSize = ftell(fp);
		lSize *= sizeof(char);
		rewind(fp);
		char * buffer = new char[sizeof(char)*lSize];
		FileReadStream is(fp, buffer, lSize);
		Document d;
		d.ParseStream(is);
		fclose(fp);
		const Value& inv = d["response"]["success"]["items"];
		if (d["response"]["success"].GetInt() == 0) {
			fatalError("Response success state is : false");
			exit(1);
		}//~!!!
		if (inv.HasMember(m_hashName.c_str())) {
			m_price = (inv[m_hashName.c_str()]["value"].GetInt()) / 100.0f;
			m_fetchedPrices++;
		}
	}
}

void steamItem::setError(bool h){
	m_parseError = h;
}

void steamItem::print(){
	std::cout << m_name << " | amount = " << m_amount << std::endl;
}
