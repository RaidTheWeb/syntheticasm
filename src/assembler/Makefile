OUT = synas
SOURCE_DIR = src
BIN_DIR = ../../bin
BUILD_DIR = build
HEADERS = $(wildcard ../include/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))
VERSION = $(shell cat ../../version)
CC = gcc
OUTCAP = $(shell echo '$(OUT)' | tr '[:lower:]' '[:upper:]')
CFLAGS = -g -static -O0 -I../include -D$(OUTCAP)_VERSION=\"$(VERSION)\"

$(BIN_DIR)/$(OUT): $(OBJECTS)
	@printf "%8s %-40s %s\n" $(CC) $@ "$(CFLAGS)"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)/
	@$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -r bin
	rm -r build