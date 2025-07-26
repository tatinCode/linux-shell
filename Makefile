ROOTNAME=main

RUNOPTIONS="Prompt> "

CC=gcc
CFLAGS= -g -Wall -I.

DEPS = 
OBJ = $(ROOTNAME).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ROOTNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean run vrun

clean:
	rm *.o $(ROOTNAME)

run: $(ROOTNAME)
	./$(ROOTNAME) $(RUNOPTIONS)

vrun: $(ROOTNAME)$(HW)$(FOPTION)
	valgrind ./$(ROOTNAME) $(RUNOPTIONS)


