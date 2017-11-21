OBJ = main.o
INC = -I "./"

Rasterrain: $(OBJ)
	g++ -std=c++11 $(OBJ) -o Rasterrain
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) Rasterrain

