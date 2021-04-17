CC=gcc
CC_OPTS=-g -Os -static -nostdlib -nostdinc -fno-pie -no-pie -mno-red-zone -fno-omit-frame-pointer -pg -mnop-mcount
INCLUDE=-fuse-ld=bfd -Wl,-T,ape/ape.lds -include ape/cosmopolitan.h ape/crt.o ape/ape.o ape/cosmopolitan.a

NAME=md2html

all: $(NAME).com

$(NAME).com.dbg: $(NAME).c
	$(CC) $(CC_OPTS) $(NAME).c -o $(NAME).com.dbg $(INCLUDE)

$(NAME).com: $(NAME).com.dbg
	objcopy -S -O binary $(NAME).com.dbg $(NAME).com

clean:
	rm $(NAME).com $(NAME).com.dbg

deploy: $(NAME).com
	scp $(NAME).com srv1:/var/www/niedzwiedzinski.cyou/$(NAME).com
