OBJ = main.o
INC = -I "./"

NextWeekerrain: $(OBJ)
	g++ -std=c++11 $(OBJ) -o NextWeekerrain
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) NextWeekerrain

