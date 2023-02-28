#include "emulator.hpp"
#include "chip8.hpp"

void	chip8::OP_00E0(void)
{
	std::memset(screenBuff, 0, WIDTH_SCREEN * HEIGHT_SCREEN);
}

void	chip8::OP_00EE(void)
{
	--sp;
	pc = stack[sp];
}

void	chip8::OP_1NNN(void)
{
	pc = NNN(opCode);
}

void	chip8::OP_2NNN(void)
{
	stack[sp] = pc;
	++sp;
	pc = NNN(opCode);
}

void	chip8::OP_3XKK(void)
{
	if (V[X(opCode)] == KK(opCode))
		pc += 2;
}

void	chip8::OP_4XKK(void)
{
	if (V[X(opCode)] != KK(opCode))
		pc += 2;
}

void	chip8::OP_5XY0(void)
{
	if (V[X(opCode)] == V[Y(opCode)])
		pc += 2;
}

void	chip8::OP_6XKK(void)
{
	V[X(opCode)] = KK(opCode);
}

void	chip8::OP_7XKK(void)
{
	V[X(opCode)] = (V[X(opCode)] + KK(opCode)) & 0x00FF;
}

void	chip8::OP_8XY0(void)
{
	V[X(opCode)] = V[Y(opCode)];
}

void	chip8::OP_8XY1(void)
{
	V[X(opCode)] |= V[Y(opCode)];
}

void	chip8::OP_8XY2(void)
{
	V[X(opCode)] &= V[Y(opCode)];
}

void	chip8::OP_8XY3(void)
{
	V[X(opCode)] ^= V[Y(opCode)];
}

void	chip8::OP_8XY4(void)
{
	uint16_t	res = V[X(opCode)] + V[Y(opCode)];
	V[0xF] = (res > 255U);
	V[X(opCode)] = res & 0xFF;
}

void	chip8::OP_8XY5(void)
{
	V[0xF] = (V[X(opCode)] > V[Y(opCode)]);
	V[X(opCode)] = (V[X(opCode)] - V[Y(opCode)]) & 0x00FF;
}

void	chip8::OP_8XY6(void)
{
	V[0xF] = V[X(opCode)] & 0x1;
	V[X(opCode)] >>= 1;
}

void	chip8::OP_8XY7(void)
{
	V[0xF] = (V[Y(opCode)] > V[X(opCode)]);
	V[X(opCode)] = V[Y(opCode)] - V[X(opCode)];
}

void	chip8::OP_8XYE(void)
{
	V[0xF] = (V[X(opCode)] & 0x80) >> 7u;
	V[X(opCode)] = (V[X(opCode)] << 1) & 0x00FF;
}

void	chip8::OP_9XY0(void)
{
	if (V[X(opCode)] != V[Y(opCode)])
		pc += 2;
}

void	chip8::OP_ANNN(void)
{
	i = NNN(opCode);
}

void	chip8::OP_BNNN(void)
{
	pc = NNN(opCode) + V[0];
}

void	chip8::OP_CXKK(void)
{
	V[X(opCode)] = rand() & KK(opCode);
}

void	chip8::OP_DXYN(void)
{
	uint16_t x = V[X(opCode)] & (WIDTH_SCREEN - 1);
	uint16_t y = V[Y(opCode)] & (HEIGHT_SCREEN - 1);
	V[0xF] = 0;
	for (uint16_t row = 0; row < N(opCode); ++row)
	{
		uint16_t spriteByte = memory[i + row];
		for (uint16_t col = 0; col < 8; ++col)
		{
			uint8_t pixel = spriteByte & (0x80 >> col);
			if (pixel)
			{
				if ((y + row) * WIDTH_SCREEN + (x + col) >= (WIDTH_SCREEN * HEIGHT_SCREEN))
					continue;
				if (screenBuff[(y + row) * WIDTH_SCREEN + (x + col)])
					V[0xF] = 1;
				screenBuff[(y + row) * WIDTH_SCREEN + (x + col)] ^= 1;
			}
		}
	}
}

void	chip8::OP_EX9E(void)
{
	if (keypad[V[X(opCode)]])
		pc += 2;
}

void	chip8::OP_EXA1(void)
{
	if (!keypad[V[X(opCode)]])
		pc += 2;
}

void	chip8::OP_FX07(void)
{
	V[X(opCode)] = delayTimer;
}

void	chip8::OP_FX0A(void)
{
	for (uint16_t i = 0; i < 16; ++i)
	{
		if (keypad[i])
		{
			V[X(opCode)] = i;
			return;
		}
	}
	pc -= 2;
}

void	chip8::OP_FX15(void)
{
	delayTimer = V[X(opCode)];
}

void	chip8::OP_FX18(void)
{
	soundTimer = V[X(opCode)];
}

void	chip8::OP_FX1E(void)
{
	i += V[X(opCode)];
}

void	chip8::OP_FX29(void)
{
	i = FONT_START_ADDRESS + (5 * V[X(opCode)]);
}

void	chip8::OP_FX33(void)
{
	uint8_t value = V[X(opCode)];
	memory[i + 2] = value % 10;
	value /= 10;
	memory[i + 1] = value % 10;
	value /= 10;
	memory[i] = value % 10;
}

void	chip8::OP_FX55(void)
{
	for (uint8_t _i = 0; _i <= X(opCode); ++_i)
		memory[this->i + _i] = V[_i];
}

void	chip8::OP_FX65(void)
{
	for (uint8_t _i = 0; _i <= X(opCode); ++_i)
		V[_i] = memory[this->i + _i];
}