CC=cc
LIBS=
CFLAGS=
BIN=.
SRC=./src
PROG=dmg
LIST=$(addprefix $(BIN)/, $(PROG))

all: $(LIST)

$(BIN)/%: $(SRC)/%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)
