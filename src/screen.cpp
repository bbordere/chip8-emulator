#include "emulator.hpp"
#include "screen.hpp"

screen::screen(void)
{
	window.create(sf::VideoMode(800, 600), "Random Screen");
	width = 800;
	height = 600;
	pixel.setSize({PIXEL_SIZE, PIXEL_SIZE});
}

screen::screen(std::size_t const width, std::size_t const height, std::string const name)
{
	window.create(sf::VideoMode(width, height), name);
	this->width = width;
	this->height = height;
	pixel.setSize({PIXEL_SIZE, PIXEL_SIZE});
}

int	screen::isValidKey(sf::Keyboard::Key const &code)
{
	sf::Keyboard::Key keysValids[] = {
		sf::Keyboard::X, sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3,
		sf::Keyboard::A, sf::Keyboard::Z, sf::Keyboard::E, sf::Keyboard::Q,
		sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::W, sf::Keyboard::C,
		sf::Keyboard::Quote, sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::V};
	for (unsigned char i = 0; i < 16; ++i)
	{
		if (keysValids[i] == code)
			return (i);
	}
	return (-1);
}

void	screen::eventHandling(chip8::keyMap_t &keyMap)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();
		if (event.type == sf::Event::KeyPressed)
		{
			int index = isValidKey(event.key.code);
			if (index != -1)
				keyMap[index] = 1;
		}
		if (event.type == sf::Event::KeyReleased)
		{
			int index = isValidKey(event.key.code);
			if (index != -1)
				keyMap[index] = 0;
		}
	}
}

void	screen::display(bool (&screenBuff)[WIDTH_SCREEN * HEIGHT_SCREEN])
{
	window.clear();
	for (unsigned char i = 0; i < 32; i++)
	{
		for (unsigned char j = 0; j < 64; ++j)
		{
			pixel.setPosition({static_cast<float>(j * PIXEL_SIZE), static_cast<float>(i * PIXEL_SIZE)});
			if (screenBuff[i * 64 + j])
				pixel.setFillColor(FG_COLOR);
			else
				pixel.setFillColor(BG_COLOR);
			window.draw(pixel);
		}
	}
	window.display();
}

bool	screen::isOpen(void)
{
	return (window.isOpen());
}