CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c init.c uintset.c port.c workers.c uintqueue.c 
SRCDIR = src
OBJDIR = target
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
TARGET = $(OBJDIR)/astra

all: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
