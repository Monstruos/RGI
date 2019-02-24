C = gcc
# Linux kernel style
INDENT_PARAMS = -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1

OUT_DIR = ./out

clean:
	rm -rf ./out

create_out:
	mkdir -p $(OUT_DIR)
	

bmp: ./bmp.c create_out
	@indent $(INDENT_PARAMS) ./bmp.c
	$(C) ./bmp.c -g -o $(OUT_DIR)/bmp.out