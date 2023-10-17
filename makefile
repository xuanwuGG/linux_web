file = $(wildcard *.c)
obj = $(patsubst %.c,%,$(file))

ALL:$(obj)

%:%.c
	gcc  $< -o $@ -lwrap -L./lib -I./inc -g -Wall

clean:
	-rm -rf $(obj)
.PHONY: clean ALL
