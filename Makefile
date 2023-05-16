NAME		:=	webserv

RED			=	\033[0;31m
GRN			= 	\033[0;32m
YLW			=	\033[0;33m
BLU			= 	\033[0;36m
DFT			= 	\033[0;37m

SRC_MAIN 	:=	./srcs/main.cpp

PARSER_DIR 	:=	./srcs/parser/
PARSER_NAME 	=	parser.cpp \

CLASS_DIR 	:=	./srcs/class/
CLASS_NAME 	=	Server.cpp \
				ServerSocket.cpp \
				Request.cpp \
				Response.cpp \
				Transaction.cpp \
				ServerConfig.cpp

UTILS_DIR 	:=	./srcs/utils/
UTILS_NAME 	=	split.cpp \
				print.cpp \
				file.cpp \
				convert.cpp \
				trim.cpp \
				safe.cpp \
				findSuffix.cpp

SRC 		=	$(SRC_MAIN) \
				$(addprefix $(PARSER_DIR),$(PARSER_NAME)) \
				$(addprefix $(CLASS_DIR),$(CLASS_NAME)) \
				$(addprefix $(UTILS_DIR),$(UTILS_NAME)) 

OBJ 		=	$(SRC:.cpp=.o)

INCLUDE		:=	-I./srcs/include/
CC 			:=	c++
CPPFLAGS 	:=	-Wall -Wextra -Werror -std=c++98 -o3
# CPPFLAGS 	:=	-fsanitize=address -g3 -o3

all : $(NAME)

clean :
	@ rm -f *.o ./srcs/class/*.o ./srcs/utils/*.o ./srcs/*.o ./srcs/parser/*.o
	@ echo "$(NAME) is $(RED)cleaned$(DFT)"

fclean : clean
	@ rm -f $(NAME)
	@ echo "$(NAME) is $(RED)fcleaned$(DFT)"


re : fclean all

$(NAME): $(OBJ)
	@ $(CC) $(CPPFLAGS) $(INCLUDE) $(OBJ) -o $@
	@ echo "$(NAME) is $(GRN)ready$(DFT)"

.PHONY : all clean fclean

%.o : %.cpp
	@ $(CC) $(CPPFLAGS) $(INCLUDE) -c $< -o $@
	