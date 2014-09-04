TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:.c=.o)

MEMCHECK=tools/memcheck
SRC=tools/memcheck.c
OBJ=tools/memcheck.o


all: $(TARGET) $(MEMCHECK)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(PLATFORM_LDFLAGS)


$(MEMCHECK): $(OBJ)
	$(CC) -o $@ $^ -llua


install:
	mkdir -p $(LIBDIR)
	cp $(TARGET) $(LIBDIR)
	rm -f $(OBJS) $(TARGET) $(OBJ)

