OBJ = main.o
INC = -I "./"

Bookerrain: $(OBJ)
	g++ -std=c++11 $(OBJ) -o Bookerrain
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) Bookerrain

