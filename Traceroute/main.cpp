#include "Traceroute.h"

void main() {
	Traceroute traceroute;
	traceroute.Run();
	std::cout << "\nPress Enter for exit...";
	std::cin.ignore(); 
	std::cin.get();
}