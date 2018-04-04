CC := gcc

IRODS_HOME := /home/billy/Applications/irods

IRODS_LIB_DIR := $(IRODS_HOME)/usr/lib
IRODS_INCLUDE_DIR := $(IRODS_HOME)/usr/include/irods

IRODS_LIBS := irods_client irods_common irods_plugin_dependencies

OUTPUT_DIR	 := build


SRC_FILES := main.c byte_buffer.c

OBJ_FILES := $(SRC_FILES:%.c=$(OUTPUT_DIR)/%.o)

VPATH := src

CFLAGS += -I$(IRODS_INCLUDE_DIR) -Iinclude 

LDFLAGS += $(addprefix -l, $(IRODS_LIBS)) \
	-L $(IRODS_LIB_DIR) \



all: init comp $(OBJ_FILES)




comp: init $(OBJFILES)
	@echo "LIB_PATHS: $(LIB_PATHS)"
	$(LD) -o $(OUTPUT_DIR)/irods_bash_completer $(OBJFILES) $(STATIC_LIBS) $(LDFLAGS)

init:
	mkdir -p $(OUTPUT_DIR)


$(OUTPUT_DIR)/%.o: %.c 
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -fr $(OUTPUT_DIR)/*

