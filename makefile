CC=gcc
RM=rm
MKDIR=mkdir
INC_DIR=inc
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

TGT=$(BIN_DIR)/test
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

IFLAGS=-I
CFLAGS=-Wall
LDLIBS=-lm

.PHONY: 	all clean

all: 		$(TGT)
		@echo "\nExecutable generated --> ./bin/test"
		
$(TGT): 	$(OBJ) | $(BIN_DIR)
		$(CC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CC) $(IFLAGS)$(INC_DIR) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
		$(MKDIR) -p $@

clean:
		$(RM) -rf $(BIN_DIR) $(OBJ_DIR) *.csv

