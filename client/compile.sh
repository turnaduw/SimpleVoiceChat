g++ -c $1.cpp
#g++ $1.o -o $1.run -pthread -lboost_system -lboost_thread -lpthread -lportaudio
g++ $1.o -o $1.run -std=c++11 -pthread -lboost_system -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread -lportaudio
#rm *.o
