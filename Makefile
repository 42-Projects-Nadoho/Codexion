CC = cc
RM = rm -f
NAME = test
SRC_DIR = src
CFLAGS = -Wall -Wextra -Werror


FILES_SOURCES = test.c

MY_SOURCES = $(addprefix $(SRC_DIR)/, $(FILES_SOURCES))
MY_OBJECTS = ${MY_SOURCES:.c=.o}


all: $(NAME)

$(NAME) : $(MY_OBJECTS)
		$(CC) $(CFLAGS) $(MY_OBJECTS) -o $(NAME)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

help:
	@echo "Targets disponibles:"
	@echo "  make              - Compiler le projet"
	@echo "  make help         - Afficher cette aide"
	@echo "  make clean        - Netoyer les fichiers objects (.o)"
	@echo "  make lint         - Netoyer les fichiers objects et l'executable"
	@echo "  make lint-strict  - Supprimer tous les fichiers creer et recompiler le projet"

clean:
		$(RM) $(MY_OBJECTS) $(MY_OBJECTS_BONUS)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)


.PHONY: all help clean fclean re
