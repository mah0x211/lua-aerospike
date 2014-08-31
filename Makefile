TARGET=$(PACKAGE).$(LIB_EXTENSION)
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(WARNINGS) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

install:
	mkdir -p $(LIBDIR)
	cp $(TARGET) $(LIBDIR)
	rm -f $(OBJS) $(TARGET)
