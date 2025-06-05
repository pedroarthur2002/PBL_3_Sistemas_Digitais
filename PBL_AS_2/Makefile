C_FILE = main
S_FILE = matrix_io
FILTERS_FILE = filters
TARGET = main

all: $(S_FILE).o $(C_FILE).o $(FILTERS_FILE).o $(TARGET)
	@echo "Compilation complete"

$(S_FILE).o: $(S_FILE).s
	as -o $(S_FILE).o $(S_FILE).s

$(C_FILE).o: $(C_FILE).c interface.h stb_image.h stb_image_write.h
	gcc -c -o $(C_FILE).o $(C_FILE).c -lm 
 
$(FILTERS_FILE).o: $(FILTERS_FILE).c interface.h
	gcc -c -o $(FILTERS_FILE).o $(FILTERS_FILE).c

$(TARGET): $(S_FILE).o $(C_FILE).o $(FILTERS_FILE).o
	gcc -o $(TARGET) $(S_FILE).o $(C_FILE).o $(FILTERS_FILE).o -lm

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

clean-images:
	rm -f *.png *.jpg

debug: $(TARGET)
	gdb $(TARGET)