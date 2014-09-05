TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:.c=.o)

MEMCHECK=tools/memcheck


all: $(TARGET) $(MEMCHECK)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) $(PLATFORM_LDFLAGS)


$(MEMCHECK).o: $(MEMCHECK).c
	$(CC) -O0 -g $(WARNINGS) $(CPPFLAGS) -o $@ -c $<

$(MEMCHECK): $(MEMCHECK).o
	$(CC) -o $@ $(OBJS) $^ -llua $(LIBS) $(PLATFORM_LDFLAGS)


install:
	mkdir -p $(LIBDIR)
	cp $(TARGET) $(LIBDIR)
	rm -f $(OBJS) $(TARGET) $(MEMCHECK).o

