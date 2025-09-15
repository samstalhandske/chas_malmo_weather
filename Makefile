# --- Basvariabler --
CC 		 ?= gcc
CFLAGS ?= -std=c11 -Iinclude -Wall -Wextra -O2 -MMD -MP
SRC := $(wildcard src/*.c)
MODE ?= release

ifeq ($(MODE),debug)
 CFLAGS += -g -O0
 OUTDIR := build/debug
else
 OUTDIR := build/release
endif

OBJ := $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRC))
BIN := $(OUTDIR)/app
DEP := $(OBJ:.o=.d)

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@
				
$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c  $< -o $@ -MF $(basename $@).d -MT $@

clean:
	$(RM) -r build
	
run: $(BIN)
	./$(BIN)
	
-include $(DEP)

.PHONY: all clean