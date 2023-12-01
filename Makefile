P_SERVER=	server
P_SRC	=	src

SRC		=	$(P_SRC)/core.c \
			$(P_SRC)/debug.c \
			$(P_SRC)/expression.c \
			$(P_SRC)/list.c \
			$(P_SRC)/main.c \
			$(P_SRC)/map.c \
			$(P_SRC)/parser.c \
			$(P_SRC)/token.c \
			$(P_SRC)/lexer.c \
			$(P_SRC)/utils.c \

OBJ		=	$(SRC:.c=.o)

NAME	=	SAX

CC = gcc
CPPFLAGS=	-I./include -Wall -Werror -DDEBUG

all:		$(NAME)

$(NAME):	$(OBJ)
			$(CC) -o $(NAME) $(OBJ)

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re