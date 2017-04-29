#pragma once
#include <string>
#include <map>
#include <vector>
#include "steamItem.h"
class inventory
{
public:
	inventory(std::string id = "UNDEFINED");
	~inventory();

	void getInventory();
	void printPrices();
	void printPrice();
	void readInventory();
	void printInventory();
	void fetchPrices();
	void sumPrices();
private:

	std::string m_steamID64;
	float m_price = 0.0f;
	std::map<std::string, int> m_items;
	std::vector<steamItem> m_ItemsVec;
};

