#include "emulator.hpp"
#include "chip8.hpp"

int main(int ac, char **av)
{
	if (ac != 2 && ac != 3)
	{
		std::cout << "Usage: ./chip8 <path_to_rom>" << std::endl;
		return (1);
	}
	chip8 chip;
	if (av[2])
		chip.setCycleDelay(std::atoi(av[2]));
		
	if (!chip.loadRom(av[1]))
	{
		std::cout << "Error while opening " << av[1] << std::endl;
		return (1);
	}
	chip.run();
	return 0;
}