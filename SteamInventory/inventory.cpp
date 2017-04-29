#include "inventory.h"
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h> 
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/filereadstream.h>
#include <cstdio>
#include "Errors.h"

using namespace rapidjson;

size_t inline write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

inventory::inventory(std::string id): m_steamID64(id)
{
	if (m_steamID64 == "UNDEFINED") {
		fatalError("STEAMID is undefined!");
	}else if(m_steamID64.length() != 17){
		fatalError("STEAMID is invalid!");
	}
}


inventory::~inventory()
{
}



void inventory::getInventory() {
		CURL *curl;
		CURLcode res;
		FILE *fp;
		curl = curl_easy_init();
		std::string url = "https://steamcommunity.com/profiles/" + m_steamID64 + "/inventory/json/730/2";
		std::string id = m_steamID64 + ".json";
		if (curl) {
			fp = fopen(id.c_str(), "w");
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			res = curl_easy_perform(curl);
			if (res == 0) {
				std::cout << "Inventory downloaded and saved to " << id << std::endl;
			}
			else {
				fatalError("Could not download the inventory... Sorry!");
			}
			/* always cleanup */
			curl_easy_cleanup(curl);
			fclose(fp);
		}

}

void inventory::readInventory() {
	//read the file
	FILE* f;
	long lSize;
	std::string fp = m_steamID64 + ".json";
	f = fopen(fp.c_str(), "r");
	if (f == NULL) {
		fatalError("Could not open file: " + fp);
	}
	fseek(f, 0, SEEK_END);
	lSize = ftell(f);
	lSize *= sizeof(char);
	rewind(f);
	char * buffer = new char[sizeof(char)*lSize];
	FileReadStream is(f, buffer, lSize);
	//printf("%s", buffer);
	Document d;
	d.ParseStream(is);
	fclose(f);
	//
	const Value& success = d["success"];
	if (!success.GetBool()) {
		std::remove(fp.c_str());
		fatalError("JSON success is false. You probably gave wrong ID or Steam servers are busy.\nFile " + fp + " removed!");
	}
	const Value& inv = d["rgInventory"];
	

	for (Value::ConstMemberIterator itr = inv.MemberBegin(); itr != inv.MemberEnd(); ++itr)
	{
		std::string tmp = itr->value["classid"].GetString();
		tmp += "_";
		tmp += itr->value["instanceid"].GetString();
		auto mit = m_items.find(tmp);
		if (mit != m_items.end()) {
			mit->second++; //it is not the first one if its name!
		}
		else {
			m_items.insert(make_pair(tmp, 1)); // first element is inserted
		}
	}

	const Value& desc = d["rgDescriptions"];
	std::cout << "Your inventory:" << std::endl;
	CURL *curl = curl_easy_init();

	std::string urlPattern = "http://steamcommunity.com/market/priceoverview/?currency=1&appid=730&market_hash_name=";
	
	for (auto const &it : m_items) {
		std::string tmp = desc[it.first.c_str()]["market_hash_name"].GetString();
		m_ItemsVec.emplace_back(-1.0f, desc[it.first.c_str()]["name"].GetString(), it.second, urlPattern + curl_easy_escape(curl, tmp.c_str(), tmp.length()), desc[it.first.c_str()]["marketable"].GetInt(), desc[it.first.c_str()]["tradable"].GetInt(), desc[it.first.c_str()]["market_hash_name"].GetString());
	}

	
}

void inventory::printInventory()
{
	for (unsigned int i = 0; i < m_ItemsVec.size();i++) {
		m_ItemsVec[i].print();
	}
}

void inventory::fetchPrices()
{
	std::cout << "Fetching all prices from Steam. It may take a while." << std::endl;

	for (unsigned int i = 0; i < m_ItemsVec.size(); i++) {
		m_ItemsVec[i].setPriceSteam();
	}
	
	if (steamItem::m_fetchedPrices != steamItem::m_numMarketable) {

		std::cout << "Could not load all prices from steam. Loaded " << steamItem::m_fetchedPrices << " of " << steamItem::m_numMarketable << " unique item prices." << std::endl;
	}
}

void inventory::sumPrices()
{
	for (unsigned int i = 0; i < m_ItemsVec.size(); i++) {
		//only marketable prices counts.
		if (m_ItemsVec[i].getPrice() != -1.0f) {
			m_price += m_ItemsVec[i].getAmount() * m_ItemsVec[i].getPrice();
		}
	}
}

void inventory::printPrices() {
	for (unsigned int i = 0; i < m_ItemsVec.size(); i++) {
		if (float m = m_ItemsVec[i].getPrice() != -1.0f) {
			std::cout << m << " * " << m_ItemsVec[i].getAmount() << std::endl;
		}
	}
}

void inventory::printPrice()
{
	std::cout << "Total price is : " << m_price << std::endl;
}
