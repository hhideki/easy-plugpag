SOURCE_DIR := sources
INCLUDE_DIRS := includes
OUTPUT_DIR := output
EXPORT_DIR := exported
LIB_DIRS = libs/raspberry-pi

BINARY_FILE := $(OUTPUT_DIR)/easy-plugpag.bin

LIBS_DIRS := $(patsubst %,-L"%",$(LIB_DIRS))
LIBS := BTSerial \
		PPPagSeguro \
		dl

CC := gcc
CFLAGS := -c -Wall -pedantic $(patsubst %,-I"%" ,$(INCLUDE_DIRS))
LFLAGS := -L"$(LIB_DIRS)" $(patsubst %,-l"%" , $(LIBS))


SOURCE_FILES := easy-plugpag.c


all: clean build export run


clean:
	rm -rf $(OUTPUT_DIR)/* $(EXPORT_DIR)/*


build: compile link


compile: $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(SOURCE_DIR)/easy-plugpag.c -o $(OUTPUT_DIR)/easy-plugpag.o
	$(CC) $(CFLAGS) $(SOURCE_DIR)/main.c -o $(OUTPUT_DIR)/main.o


export: $(EXPORT_DIR) compile
	$(CC) -shared $(OUTPUT_DIR)/easy-plugpag.o -o $(EXPORT_DIR)/easy-plupag.so
	cp $(INCLUDE_DIRS)/*.h $(EXPORT_DIR)/


link:
	$(CC) $(LFLAGS) $(OUTPUT_DIR)/*.o -o $(BINARY_FILE)


run: build
	$(BINARY_FILE)


$(OUTPUT_DIR):
	mkdir -p $@


$(EXPORT_DIR):
	mkdir -p $@