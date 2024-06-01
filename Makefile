TARGET	 	= firewall
TARGET_GEN	= generator

SRC_DIR		= src/
OBJ_DIR 	= obj/
OBJ_GEN_DIR = obj_gen/
TEST_DIR	= test/
DB_DIR		= database/
GEN_DIR		= src/generator/

HEADER 		= src/firewall.h
HEADER_UTILS = src/utils.h
HEADER_GEN	= src/generator/generator.h
			
SRC			= \
			firewall.c \
			utils.c

SRC_GEN		= \
			generator.c \
			utils.c

OBJ			= $(SRC:%.c=$(OBJ_DIR)%.o)
OBJ_GEN		= $(SRC_GEN:%.c=$(OBJ_GEN_DIR)%.o)

RM 			= rm -rf
CC 			= gcc
CFLAGS 		= -std=c11 -Wall -Wextra -Werror

VPATH		= $(GEN_DIR) $(SRC_DIR)

.PHONY : all install install_generator clean uninstall test

all				: install install_generator test

install			: $(TARGET)

install_generator : $(TARGET_GEN)


$(TARGET)		: $(OBJ)
		@$(CC) $(OBJ) -o $(TARGET)
		@echo "Compiled $(TARGET) successfully!"

$(TARGET_GEN)	: $(OBJ_GEN)
		@$(CC) $(OBJ_GEN) -o $(TARGET_GEN)
		@echo "Compiled $(TARGET_GEN) successfully!"

$(OBJ)			: $(HEADER) $(HEADER_UTILS)
$(OBJ_GEN)		: $(HEADER_GEN) $(HEADER_UTILS)

$(OBJ_DIR)%.o 	: $(SRC_DIR)%.c
		@mkdir -p $(OBJ_DIR)
		@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_GEN_DIR)%.o : %.c
		@mkdir -p $(OBJ_GEN_DIR)
		@$(CC) $(CFLAGS) -c $< -o $@

clean 	:
		@if ([ ! -d $(OBJ_DIR) ] && [ ! -d $(OBJ_GEN_DIR) ]); then \
			echo "make: Nothing to be done for 'make clean'."; fi;
		@if [ -d $(OBJ_DIR) ]; then $(RM) $(OBJ_DIR); \
			echo "Removed $(OBJ_DIR) (directory) successfully!"; fi;
		@if [ -d $(OBJ_GEN_DIR) ]; then $(RM) $(OBJ_GEN_DIR); \
			echo "Removed $(OBJ_GEN_DIR) (directory) successfully!"; fi;

uninstall	:
		@if ([ ! -d $(OBJ_DIR) ] && [ ! -d $(OBJ_GEN_DIR) ] && \
			[ ! -f $(TARGET) ] && [ ! -f $(TARGET_GEN) ]); then \
			echo "make: Nothing to be done for 'make uninstall'."; fi;
		@if [ -d $(OBJ_DIR) ]; then $(RM) $(OBJ_DIR); \
			echo "Removed $(OBJ_DIR) (directory) successfully!"; fi;
		@if [ -d $(OBJ_GEN_DIR) ]; then $(RM) $(OBJ_GEN_DIR); \
			echo "Removed $(OBJ_GEN_DIR) (directory) successfully!"; fi;
		@if [ -f $(TARGET) ]; then $(RM) $(TARGET); \
			echo "Removed $(TARGET) (file) successfully!"; fi;
		@if [ -f $(TARGET_GEN) ]; then $(RM) $(TARGET_GEN); \
			echo "Removed $(TARGET_GEN) (file) successfully!"; fi;

test	:
		@if [ -f $(TARGET) ]; then \
			(cp $(TARGET) $(TEST_DIR)$(TARGET) && \
			cd $(TEST_DIR) && ./test_firewall.sh && $(RM) $(TARGET)); \
		else echo "Program $(TARGET) is not compiled! Run 'make install' command."; fi;

dist	:
		$(shell mkdir Firewall/)
		$(shell cp -r $(SRC_DIR) Firewall/)
		$(shell cp -r $(DB_DIR) Firewall/)
		$(shell cp -r $(TEST_DIR) Firewall/)
		$(shell cp Makefile Firewall/)
		$(shell cp README.txt Firewall/)
		@$(MAKE) -C Firewall/ uninstall
		$(shell tar -czf Firewall.tar Firewall)
		@$(RM) Firewall/
		@echo "Create archive Firewall.tar (file) successfully!";

code_style :
	clang-format -n --style=Google $(SRC_DIR)*.c $(SRC_DIR)*.h $(GEN_DIR)*.c $(GEN_DIR)*.h

help	:
		@echo "It's a help manual. All the basic Make commands are labeled here.\n \
	\tThese commands are as follows:\n \
	\033[34m1.\033[0m \033[32mall\033[0m: executes the \033[31minstall\033[0m, \033[31minstall_generator\033[0m, and \033[31mtest\033[0m commands in turn.\n \
	\033[34m2.\033[0m \033[32minstall\033[0m: compiles the program \033[35m$(TARGET)\033[0m.\n \
	\033[34m3.\033[0m \033[32minstall_generator\033[0m: compiles the program \033[35m$(TARGET_GEN)\033[0m.\n \
	\033[34m4.\033[0m \033[32mtest\033[0m: starts testing the program \033[35m$(TARGET)\033[0m.\n \
	\033[34m5.\033[0m \033[32mdist\033[0m: creates an archive of this project \033[35mFirewall.tar\033[0m.\n \
	\033[34m6.\033[0m \033[32mclean\033[0m: removes minor objects that are not needed after compilation of program.\n \
	\033[34m7.\033[0m \033[32muninstall\033[0m: deletes all created files and directories.\n \
	\033[34m8.\033[0m \033[32mcode_style\033[0m: checks if the code matches the Google style.\n \
	\033[34m9.\033[0m \033[32mhelp\033[0m: displays this help.\n"
