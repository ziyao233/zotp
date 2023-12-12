ifeq ($(DEBUG),1)
	DEBUG_FLAGS	:= -O0 -g
else
	DEBUG_FLAGS	:= -O2
endif

SRC		:= $(wildcard src/*.c)
HEADERS		:= $(wildcard src/*.h)
OUTPUT_EXEC	:= zotp
CC		?= gcc
CFLAGS		:= -Wall -Werror -pedantic -Wextra $(DEBUG_FLAGS)
LDFLAGS		:= -lm -lcrypto
OBJS		:= $(patsubst %.c, %.o, $(SRC))

#used in compiler directives for development purposes
ifneq ($(wildcard .git/.),)
	GIT_HASH:=$(shell git describe --abbrev=4 --dirty --always --tags)
	DEFINES+=-DGIT_HASH=\"$(GIT_HASH)\"
endif

default: $(OUTPUT_EXEC)

$(OUTPUT_EXEC): $(OBJS)
	$(CC) -o $(OUTPUT_EXEC) $(OBJS) $(LDFLAGS)

clean:
	@echo Removing object files
	rm $(OUTPUT_EXEC)
	find . -name "*.o" -exec rm {} \;

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: default clean
