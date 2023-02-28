#ifndef CHIP_8_HPP
#define CHIP_8_HPP

#include "emulator.hpp"

#define X(op) ((op & 0x0F00u) >> 8u)
#define Y(op) ((op & 0x00F0u) >> 4u)

#define NNN(op) ((op & 0x0FFFu))
#define KK(op) ((op & 0x00FFu))
#define N(op) ((op & 0x000Fu))

#define INST_CODE(op) (op & 0xF000u)
#define INST_INDEX(op) ((op & 0xF000u) | KK(op))
#define INST_INDEX2(op) ((op & 0xF000u) | N(op))

class chip8
{
	public:
		typedef std::map<uint16_t, bool> keyMap_t;
		typedef void (chip8::*fctPtr)(void);
		typedef std::unordered_map<uint16_t, fctPtr> fctMap_t;

		uint8_t		memory[4096]{};
		bool		screenBuff[WIDTH_SCREEN * HEIGHT_SCREEN]{};
		uint16_t	pc{};
		uint8_t		sp{};
		uint16_t	i{};
		uint16_t	stack[16]{};
		uint8_t		delayTimer{};
		uint8_t		soundTimer{};
		uint16_t	V[16]{};

		keyMap_t keypad{};
		fctMap_t instructionsMap{};

		uint16_t		opCode;
		uint16_t 		cycleDelay;


		chip8(void);
		chip8(uint16_t const cycleDelay);
		bool	loadRom(std::string const path);
		void	setCycleDelay(uint16_t const _cycleDelay);

		void	cycle(void);
		void	debug(void);
		void	printInstruct(void);
		void	run(void);
		void	playSound(void);

		void	OP_00E0(void);
		void	OP_00EE(void);
		void	OP_1NNN(void);
		void	OP_2NNN(void);
		void	OP_3XKK(void);
		void	OP_4XKK(void);
		void	OP_5XY0(void);
		void	OP_6XKK(void);
		void	OP_7XKK(void);
		void	OP_8XY0(void);
		void	OP_8XY1(void);
		void	OP_8XY2(void);
		void	OP_8XY3(void);
		void	OP_8XY4(void);
		void	OP_8XY5(void);
		void	OP_8XY6(void);
		void	OP_8XY7(void);
		void	OP_8XYE(void);
		void	OP_9XY0(void);
		void	OP_ANNN(void);
		void	OP_BNNN(void);
		void	OP_CXKK(void);
		void	OP_DXYN(void);
		void	OP_EX9E(void);
		void	OP_EXA1(void);
		void	OP_FX07(void);
		void	OP_FX0A(void);
		void	OP_FX15(void);
		void	OP_FX18(void);
		void	OP_FX1E(void);
		void	OP_FX29(void);
		void	OP_FX33(void);
		void	OP_FX55(void);
		void	OP_FX65(void);
};


#endif	