gcc entr1/entr1.c -o bin/entr1/entr1
gcc -c entr2/db.c -o bin/entr2/db.o -lmysqlclient
gcc entr2/entr2.c -o bin/entr2/entr2 -lmysqlclient   
gcc server/server.c -o bin/server/server

gnome-terminal -- bash -c "bin/entr1/entr1   exec bash"
sleep 2
gnome-terminal -- bash -c "bin/entr2/entr2   exec bash"
sleep 2
gnome-terminal -- bash -c "bin/server/server   exec bash"
