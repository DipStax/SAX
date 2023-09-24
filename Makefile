P_SERVER=	server
P_SRC	=	src

SRC		=	$(P_SRC)/main.c \
			$(P_SRC)/list.c \
			$(P_SRC)/map.c \
			$(P_SRC)/utils.c \
			$(P_SRC)/token.c \
			$(P_SRC)/tokenizer.c \
			$(P_SRC)/core.c \

OBJ		=	$(SRC:.c=.o)

NAME	=	SAX

CC = gcc
CPPFLAGS=	-I./include -Wall -Werror

all:		$(NAME)

$(NAME):	$(OBJ)
			$(CC) -o $(NAME) $(OBJ)

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re