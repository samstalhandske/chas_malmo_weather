# C-kompilator (byt vid behov, t.ex. clang)
# Detta är en enkel variabel definition
CC := gcc

# Katalog där källfilerna finns.
# Detta är en enkel variabel definition
SRC_DIR := src

# Katalog där objektfilerna ska placeras
# Detta är en enkel variabel definition
BUILD_DIR := build

# Flaggor: standard, varningar, optimering + auto-dep för headers
# Detta är en enkel variabel definition
CFLAGS := -std=c90 -Isrc -Wall -Wextra -MMD -MP

# Länkarflaggor
# Detta är en enkel variabel definition
LDFLAGS := -flto -Wl,--gc-sections

# Bibliotek att länka mot
# Detta är en enkel variabel definition
LIBS := -lcurl

# Hittar alla .c filer rekursivt i katalogen.
#Den anropar 'find' kommandot i Linux och formaterar resultatet som en lista på sökvägar med mellanslag mellan varje
SRC := $(shell find -L $(SRC_DIR) -type f -name '*.c')

# Mappa varje .c till motsvarande .o i BUILD_DIR
# Häre anropar den inbyggda 'patsubst' funktionen i Make för att ersätta prefix och suffix
# Alltså, den tar varje filväg i SRC som matchar mönstret $(SRC_DIR)/%.c och ersätter det med $(BUILD_DIR)/%.o
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# Tillhörande .d-filer (dependency-filer skapade av -MMD)
# Här härleder vi .d-filerna direkt från OBJ genom att bara byta filändelsen från .o till .d
# Eftersom varje .o kompileras med -MMD (och vi anger -o $@), skriver GCC normalt .d filerna i samma sökväg som .o filerna.
# Så mappningen stämmer rekursivt.
DEP := $(OBJ:.o=.d)

# Namnet på den körbara filen
# Detta är en enkel variabel definition
BIN := main

# Standardmål: bygg binären
# Se det som en function man kan anropa utifrån (make all)
# Det efter : betyder att detta mål beror på $(BIN)
# Alltså, för att bygga målet 'all', måste FÖRST '$(BIN)' byggas.
# Alltså raden "$(BIN): $(OBJ)" nedan körs först
all: $(BIN)
	@echo "Build complete."

# Länksteg: binären beror på alla objektfiler
# Se också detta som en funktion men som anropas inifrån (av 'all' målet)
# Och för att bygga målet '$(BIN)', måste FÖRST listan på objektfiler byggas (alla .o filer i $(OBJ))
# Det ser vi på raden efter : som säger att '$(BIN)' beror på hela listan med objektfiler, alltså '$(OBJ)'
# Efter OBJ är en lista på alla .o filer som ska byggas så tar den varje sökväg och letar efter ett mål som matchar
# mönstret "$(BUILD_DIR)/%.o" (se nedan) och kör det för varje fil i listan.
# Alltså raden "$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c" nedan körs för alla inaktuella filer först. (Han jämför tidsstämplar mellan .c och .o i filsystemet)
$(BIN): $(OBJ)
	@$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LIBS)

# Mönsterregel: bygger en .o från motsvarande .c
# Samma här, detta är en funktion som anropas inifrån (av '$(BIN)' målet)
# Om varje enskild .o fil saknas eller är äldre än sin motsvarande .c fil (eller någon header via dep-filen), körs denna regel för att kompilera.
# Det ser vi på raden efter : som säger att varje .o fil i $(BUILD_DIR) beror på motsvarande .c fil i $(SRC_DIR)
# Det den gör är att den kör denna regel för varje fil som matchar mönstret, exempelvis:
#   $(BUILD_DIR)/subfolder/test.o: $(SRC_DIR)/subfolder/test.c
#   $(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
#   osv...
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	
# Hjälpmål: kör programmet om det är byggt
# Se det som en function anropas utifrån (make run)
# Men för att köra, måste FÖRST '$(BIN)' byggas
run: $(BIN)
	./$(BIN)
	
# Hjälpmål: städa bort genererade filer
clean:
	@rm -rf $(BUILD_DIR) $(BIN)
	
# Hjälpmål: skriv ut variabler för felsökning
# Kör make print för att se variablerna efter expansion
print:
	@echo "Källfiler: $(SRC)"
	@echo "Objektfiler: $(OBJ)"
	@echo "Dependency-filer: $(DEP)"
	
# Inkludera header-beroenden (prefix '-' = ignorera om de inte finns ännu)
-include $(DEP)

# Dessa mål är inte riktiga filer; kör alltid när de anropas
.PHONY: all run clean