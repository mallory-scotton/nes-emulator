TARGET			=	nes

DIRECTORY		=	Source

CXX				=	g++

LDFLAGS			=	-lsfml-graphics \
					-lsfml-window \
					-lsfml-system \
					-lsfml-audio
INCLUDES		=	-I$(DIRECTORY)
CXXFLAGS		=	-std=gnu++20
DFLAGS			=	
WARNING_FLAGS	=	-Wall -Wextra

FLAGS			=	$(LDFLAGS) $(INCLUDES) $(CXXFLAGS) $(DFLAGS)

SOURCES			=	$(shell find $(DIRECTORY) -type f -iname "*.cpp")
OBJECTS			=	$(SOURCES:.cpp=.o)

TOTAL			:=	$(words $(SOURCES))

RESET			=	\033[0m
BOLD			=	\033[1m
GREEN			=	\033[32m
BLUE			=	\033[34m
YELLOW			=	\033[33m

all: $(TARGET)

show_infos:
	@printf "$(BLUE)$(BOLD)[INFO]$(RESET) Compilation flags: $(FLAGS)\n"
	@printf "$(BLUE)$(BOLD)[INFO]$(RESET) Total source files: $(TOTAL)\n"

%.o: %.cpp
	@printf "$(BLUE)[CXX]$(RESET) $(BOLD)Compiling$(RESET) $<\n"
	@$(CXX) -c $< -o $@ $(FLAGS)

$(TARGET): show_infos $(OBJECTS)
	@printf "$(GREEN)[LINK]$(RESET) $(BOLD)Linking$(RESET) $@\n"
	@$(CXX) -o $(TARGET) $(OBJECTS) $(FLAGS)
	@printf "$(GREEN)$(BOLD)Build completed successfully!$(RESET)\n"

clean:
	@printf "$(YELLOW)[CLEAN]$(RESET) $(BOLD)Removing object files$(RESET)\n"
	@find -type f -iname "*.o" -delete
	@find -type f -iname "*.d" -delete

fclean: clean
	@printf "$(YELLOW)[FCLEAN]$(RESET) $(BOLD)Removing target$(RESET)\n"
	@rm -f $(TARGET)

re: fclean all
