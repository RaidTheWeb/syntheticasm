OUT = synthetic
SOURCE_DIR = src
BIN_DIR = bin
BUILD_DIR = build
HEADERS = $(wildcard $(SOURCE_DIR)/include/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))
VERSION = $(shell cat version)
CC = gcc
OUTCAP = $(shell echo '$(OUT)' | tr '[:lower:]' '[:upper:]')
CFLAGS = -g -static -O0 -Isrc/include -D$(OUTCAP)_VERSION=\"$(VERSION)\"

all: $(BIN_DIR)/$(OUT) assembler compiler

$(BIN_DIR)/$(OUT): $(OBJECTS)
	@printf "%8s %-40s %s\n" $(CC) $@ "$(CFLAGS)"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@

assembler:
	@cd src/assembler; make

compiler:
	@chmod +x $(SOURCE_DIR)/compiler/syncc
	@cp $(SOURCE_DIR)/compiler/* bin/


$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)/
	@$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -r bin
	rm -r build