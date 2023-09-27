g++ *.h *.cpp -o server.run -lsqlite3 -lboost_system -lboost_filesystem
rm *.o
rm *.gch
rm svc_server.db
