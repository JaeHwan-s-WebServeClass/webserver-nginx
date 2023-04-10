NAME 		:=	webserv

DIR 		:=	./class/
SRC_NAME 	=	Server.cpp \
				ServerSocket.cpp

SRC 		=	$(addprefix $(DIR),$(SRC_NAME))
OBJ 		=	$(SRC:.c=.o)

INCLUDE		:=	-I./include/
CC 			:=	c++
CXXFLAGS 	:=	-Wall -Wextra -Werror -std=c++98 $(INCLUDE)

all : $(NAME)

#clean :
#	rm -f *.o ./class/*.o

#fclean : clean
#	rm -f $(NAME)

#re : fclean all

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) main.cpp -o $@

.PHONY : all clean fclean
