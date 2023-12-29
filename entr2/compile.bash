gcc -c db.c -o ../bin/entr2/db.o -lmysqlclient
gcc entr2.c -o ../bin/entr2/entr2 -lmysqlclient   
../bin/entr2/entr2
#gnome-terminal -- bash -c "../bin/entr2/entr2 exec bash"
