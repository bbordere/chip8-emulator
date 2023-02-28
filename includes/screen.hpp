#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "emulator.hpp"
#include "chip8.hpp"

class screen
{
	private:
		sf::RenderWindow	window;
		sf::RectangleShape	pixel;
		std::size_t			height;
		std::size_t			width;

	public:
		screen(void);
		screen(std::size_t const width, std::size_t const height, std::string const name);

		int	isValidKey(sf::Keyboard::Key const &code);
		void	eventHandling(chip8::keyMap_t &keyMap);
		void	display(bool (&screenBuff)[WIDTH_SCREEN * HEIGHT_SCREEN]);
		bool	isOpen(void);
};

#endif