#include <iostream>
#include <string>
#include "inventory.h"

int main(void)
{
	std::cout << "*** SteamInventoryPricer ***" << std::endl;
	std::cout << "Please insert your steam id64: " << std::endl;
	std::string id;
	std::cin >> id;

	inventory inv(id);
	
	inv.getInventory();
	inv.readInventory();
	//inv.printInventory();
	inv.fetchPrices();
	//inv.printPrices();
	inv.sumPrices();
	inv.printPrice();
	std::cout << "Press enter to exit." << std::endl;
	std::cin.ignore();
	getchar();
	return 0;
}