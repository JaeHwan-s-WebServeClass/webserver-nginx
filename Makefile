NAME 		:=	webserv

SRC_DIR 	:=	./srcs/class/
SRC_NAME 	=	Server.cpp \
				ServerSocket.cpp \
				Request.cpp \
				Response.cpp \
				Transaction.cpp

UTILS_DIR 	:=	./srcs/utils/
UTILS_NAME 	=	split.cpp \
				errorhandler.cpp \
				hexToInt.cpp \
				trim.cpp

SRC 		=	$(addprefix $(SRC_DIR),$(SRC_NAME)) \
				$(addprefix $(UTILS_DIR),$(UTILS_NAME)) 

OBJ 		=	$(SRC:.c=.o)

INCLUDE		:=	-I./srcs/include/
CC 			:=	c++
#CXXFLAGS 	:=	-Wall -Wextra -Werror -std=c++98
CXXFLAGS 	:=	

all : $(NAME)

#clean :
#	rm -f *.o ./class/*.o ./utils/*.o

#fclean : clean
#	rm -f $(NAME)

#re : fclean all

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) ./srcs/main.cpp -o $@

.PHONY : all clean fclean

%.c : %.o
	$(CC) $(CXXFLAGS) $(INCLUDE) $@ -o $^
