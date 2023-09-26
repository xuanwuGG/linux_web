file = $(wildcard *.c)
obj = $(patsubst %.c,%,$(file))

ALL:$(obj)

%:%.c
	gcc $< -o $@ -g -Wall

clean:
	-rm -rf $(obj)
.PHONY: clean ALL
