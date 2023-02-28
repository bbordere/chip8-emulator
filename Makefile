CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -I includes/ -g3 -Ilib/include -std=c++17 -O3

FILES = src/main.cpp src/chip8.cpp src/screen.cpp src/opcodes.cpp

OBJS = $(FILES:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

NAME = chip8

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -Ilib/include -Llib/SFML_LINUX/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o $(NAME)

debug: CXXFLAGS += -D DEBUG=1
debug: all


clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re