#include "chip8.hpp"
#include "emulator.hpp"
#include "screen.hpp"

chip8::chip8(uint16_t const _cycleDelay): cycleDelay(_cycleDelay)
{
	chip8();
}

void chip8::setCycleDelay(uint16_t const _cycleDelay)
{
	cycleDelay = _cycleDelay;
}

chip8::chip8(void): cycleDelay(4)
{
	srand(time(nullptr));
	pc = START_ADDRESS;
	const uint8_t fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	std::memcpy(memory + FONT_START_ADDRESS, fontset, 80);

	instructionsMap[0x00E0] = &chip8::OP_00E0;
	instructionsMap[0x00EE] = &chip8::OP_00EE;
	{
		const chip8::fctPtr instructions[] = {&chip8::OP_1NNN, &chip8::OP_2NNN, &chip8::OP_3XKK, 
											&chip8::OP_4XKK, &chip8::OP_5XY0, &chip8::OP_6XKK, &chip8::OP_7XKK};

		for (uint16_t i = 0; i < 7; ++i)
			instructionsMap[(i + 1) * (0x1000)] = instructions[i];
	}
	{
		const chip8::fctPtr instructions[] = {&chip8::OP_8XY0, &chip8::OP_8XY1, &chip8::OP_8XY2, &chip8::OP_8XY3,
											&chip8::OP_8XY4, &chip8::OP_8XY5, &chip8::OP_8XY6, &chip8::OP_8XY7};
		for (uint16_t i = 0; i < 8; ++i)
			instructionsMap[0x8000 | i] = instructions[i];
		instructionsMap[0x800E] = &chip8::OP_8XYE;
	}
	{
		const chip8::fctPtr instructions[] = {&chip8::OP_9XY0, &chip8::OP_ANNN, &chip8::OP_BNNN, &chip8::OP_CXKK, &chip8::OP_DXYN,};
		for (uint16_t i = 0; i < 5; ++i)
			instructionsMap[0x9000 + (i * 0x1000)] = instructions[i];
	}
	instructionsMap[0xE09E] = &chip8::OP_EX9E;
	instructionsMap[0xE0A1] = &chip8::OP_EXA1;
	{
		const uint16_t	index[] = {0xF007, 0xF00A, 0xF015, 0xF018, 0xF01E, 0xF029, 0xF033, 0xF055, 0xF065};
		const chip8::fctPtr instructions[] = {&chip8::OP_FX07, &chip8::OP_FX0A, &chip8::OP_FX15, &chip8::OP_FX18,
											&chip8::OP_FX1E, &chip8::OP_FX29, &chip8::OP_FX33, &chip8::OP_FX55, &chip8::OP_FX65};
		for (uint16_t i = 0; i < 9; ++i)
			instructionsMap[index[i]] = instructions[i];
	}
}

bool	chip8::loadRom(std::string const path)
{
	std::ifstream romFile(path, std::ios::binary | std::ios::ate);
	if (!romFile.is_open())
	{
		return (false);
	}
	std::streampos size = romFile.tellg();
	if (size > 4096)
	{
		romFile.close();
		return (false);
	}
	char buff[static_cast<uint16_t>(size)];
	romFile.seekg(0, std::ios::beg);
	romFile.read(buff, size);
	romFile.close();
	std::memcpy(memory + START_ADDRESS, buff, size);
	return (true);
}

void	chip8::playSound(void)
{
	static sf::SoundBuffer buffer;
	static sf::Sound sound;
	static bool init = true;
	if (init)
	{
		init = false;
		if (!buffer.loadFromFile("sound.wav"))
			return;
		sound.setBuffer(buffer);
	}
	sound.play();
}

void	chip8::cycle(void)
{
	opCode = (memory[pc] << 8u) | memory[pc + 1];
	pc += 2;

	if (INST_CODE(opCode) == 0 || INST_CODE(opCode) == 0xE000 || INST_CODE(opCode) == 0xF000)
		((*this).*instructionsMap[INST_INDEX(opCode)])();
	else if (INST_CODE(opCode) == 0x8000)
		((*this).*instructionsMap[INST_INDEX2(opCode)])();
	else
		((*this).*instructionsMap[INST_CODE(opCode)])();
	if (delayTimer > 0)
		--delayTimer;
	if (soundTimer > 0)
	{
		playSound();
		--soundTimer;
	}
	#ifdef DEBUG
		debug();
	#endif
}

void	chip8::printInstruct(void)
{
	std::cout << "0x";
	std::cout << std::setw(4) << std::setfill('0') << std::hex << std::uppercase;
	std::cout << opCode << " ";
	static const std::unordered_map<uint16_t, std::string> instrucName = {
		{0x00E0, "CLS"}, {0x00EE, "RET"}, {0x1000, "JP"}, {0x2000, "CALL"},
		{0x3000, "SE V"}, {0x4000, "SNE V"}, {0x5000, "SE V"}, {0x6000, "LD V"},
		{0x7000, "ADD V"}, {0x8000, "LD V"}, {0x8001, "OR V"}, {0x8002, "AND V"},
		{0x8003, "XOR V"}, {0x8004, "ADD V"}, {0x8005, "SUB V"}, {0x8006, "SHR V"},
		{0x8007, "SUBN V"}, {0x800E, "SHL V"}, {0x9000, "SNE V"}, {0xA000, "LD I"},
		{0xB000, "JP V0"}, {0xC000, "RND V"}, {0xD000, "DRW V"}, {0xE09E, "SKP V"},
		{0xE0A1, "SKNP V"}, {0xF007, "LD V"}, {0xF00A, "LD V"}, {0xF015, "LD DT"},
		{0xF018, "LD ST"}, {0xF01E, "ADD I"}, {0xF029, "LD F"}, {0xF033, "LD B"},
		{0xF055, "LD [I]"}, {0xF065, "LD v"}
	};
	// std::cout << instrucName.at(INST_CODE(opCode)) << '\n';
	switch (INST_CODE(opCode))
	{
		case 0x0:
			std::cout << instrucName.at(INST_INDEX(opCode)) << '\n'; break;
		case 0x1000: case 0x2000: case 0xA000: case 0xB000:
			std::cout << instrucName.at(INST_CODE(opCode)) << ' ' << NNN(opCode) << '\n'; break;
		case 0x3000: case 0x4000: case 0x6000: case 0x7000: case 0xC000:
			std::cout << instrucName.at(INST_CODE(opCode)) << X(opCode) << ", " << KK(opCode) << '\n'; break;
		case 0x5000: case 0x9000:
			std::cout << instrucName.at(INST_CODE(opCode)) << X(opCode) << ", V" << Y(opCode) << '\n'; break;
		case 0x8000:
			std::cout << instrucName.at(INST_INDEX2(opCode)) << X(opCode) << ", V" << Y(opCode) << '\n'; break;
		case 0xD000:
			std::cout << instrucName.at(INST_CODE(opCode)) << X(opCode) << ", V" << Y(opCode) << ", " << KK(opCode) << '\n'; break;
		case 0xE000: 
			std::cout << instrucName.at(INST_INDEX(opCode)) << X(opCode) << '\n'; break;
		case 0xF000:
		{
			switch (INST_INDEX(opCode))
			{
				case 0xF007:
					std::cout << instrucName.at(INST_INDEX(opCode)) << ", DT" << '\n'; break;
				case 0xF00A:
					std::cout << instrucName.at(INST_INDEX(opCode)) << ", K" << '\n'; break;
				case 0xF015: case 0xF018: case 0xF01E: case 0xF029: case 0xF033: case 0xF055:
					std::cout << instrucName.at(INST_INDEX(opCode)) << ", V" << X(opCode) << '\n'; break;
				case 0xF065:
					std::cout << instrucName.at(INST_INDEX(opCode)) << X(opCode) << ", [I]" << '\n'; break;
			}
		}break;
	}
}

void	chip8::debug(void)
{
	std::cout << "---INSTRUCTION---" << '\n';
	printInstruct();
	std::cout << "---REGISTERS---" << '\n';
	std::cout << std::hex << std::uppercase << std::left;
	std::cout << std::setw(8) << std::setfill(' ') << "I:" << std::right << i << std::endl;
	std::cout << std::left << std::setw(8) << "PC:"  << std::setfill(' ') << pc << std::endl;
	std::cout << std::setw(8) << std::setfill(' ') << "DT:" << std::right << (int)delayTimer << std::endl;
	std::cout << std::left << std::setw(8) << "ST:"  << std::setfill(' ') << (int)soundTimer << std::endl;
	for (unsigned char i = 0; i < 16; ++i)
	{
		std::cout <<  std::left << "V[0x" << (int)i << "]: " << std::setw(5) << (int)V[i];
		std::cout << std::right << std::setw(10) << std::setfill(' ') << "Stack[0x" << (int)i << "]: " << (int)stack[i] << std::endl;
	}
	std::cout <<'\n';
}

void	chip8::run(void)
{
	screen chipScreen(WIDTH_SCREEN * PIXEL_SIZE, HEIGHT_SCREEN * PIXEL_SIZE, "chip8");
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	while (chipScreen.isOpen())
	{
		chipScreen.eventHandling(keypad);
		auto currentTime = std::chrono::high_resolution_clock::now();
		double dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;
			cycle();
			chipScreen.display(screenBuff);
		}
	}
}

