all: myls.o myShell.o myCd.o mycp.o myps.o mygrep.o help.o
	gcc  *.o -lreadline -o myShell  && rm *.o 
myShell.o: myShell.c
	gcc -c myShell.c -o myShell.o
myCd.o: myCd.c
	gcc -c myCd.c -o myCd.o
myls.o: myls.c
	gcc -c myls.c -o myls.o
mycp.o: mycp.c
	gcc -c mycp.c -o mycp.o
myps.o: myps.c
	gcc -c myps.c -o myps.o
mygrep.o:
	gcc -c mygrep.c -o mygrep.o
help.o:
	gcc -c help.c -o help.o
clean:
	rm myShell 