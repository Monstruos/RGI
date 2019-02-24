C = gcc
# Linux kernel style
INDENT_PARAMS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

OUT_DIR = ./out

clean:
	rm -rf $(OUT_DIR)

clean_read:
	rm -f $(OUT_DIR)/stegano_read.out

clean_main:
	rm -f $(OUT_DIR)/stegano.out

create_out:
	mkdir -p $(OUT_DIR)

stegano_read: clean_read ./stegano_read.c create_out
	@indent $(INDENT_PARAMS) ./stegano_read.c
	$(C) ./stegano_read.c -g -o $(OUT_DIR)/stegano_read.out -lSDL2

stegano: clean_main ./stegano.c create_out
	@indent $(INDENT_PARAMS) ./stegano.c
	$(C) ./stegano.c -g -o $(OUT_DIR)/stegano.out -lSDL2