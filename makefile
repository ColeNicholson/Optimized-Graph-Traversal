Optimal_Value_Calculator.out : OVC.o
	g++ -g -o Optimal_Value_Calculator.out OVC.o

OVC.o : Driver.cpp adjList.hpp
	g++ -g -c Driver.cpp

clean : 
	rm *.o Optimal_Value_Calculator.out *~
