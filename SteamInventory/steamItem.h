#pragma once
#include <string>
class steamItem
{
public:
	steamItem(float price = -1, std::string name = "NoName", int amount = 0,
		std::string url = "Undefined", int market = -1 , int trade = -1,
		bool hError = 0, std::string hashName = "Noname");
	~steamItem();

	void setPriceSteam();
	void setPriceBacpackTF();
	void setError(bool h);
	void print();

	std::string getName() const { return m_name; };
	std::string getUri() const { return m_url; };

	int getAmount() const { return m_amount; };

	float getPrice() const { return m_price; };

	//static
	static void downloadBackpackTF();

	static int m_numMarketable;
	static int m_numAll;
	static int m_fetchedPrices;

private:
	bool m_parseError;
	int m_amount;
	int m_marketable;
	int m_tradable;

	float m_price;

	std::string m_name;
	std::string m_hashName;
	std::string m_url;

	//static
	static bool m_tfBackpackDownloaded;

};