ifeq ($(DEBUG),1)
	DEBUG_FLAGS	:= -O0 -g
else
	DEBUG_FLAGS	:= -O2
endif

#used in compiler directives for development purposes
ifneq ($(wildcard .git/.),)
	GIT_HASH:=$(shell git rev-parse --short HEAD)
	DEFINES+=-DGIT_HASH=\"$(GIT_HASH)\"
endif

SRC		:= $(wildcard src/*.c)
HEADERS		:= $(wildcard src/*.h)
OUTPUT_EXEC	:= zotp
CC		?= gcc
CFLAGS		:= -Wall -Werror -pedantic -Wextra $(DEBUG_FLAGS) $(DEFINES)
LDFLAGS		:= -lm -lcrypto
OBJS		:= $(patsubst %.c, %.o, $(SRC))

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
