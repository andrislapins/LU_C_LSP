output: LSP_PD3_andris_lapins.o LSP_MD3_andris_lapins.o
	gcc LSP_PD3_andris_lapins.o LSP_MD3_andris_lapins.o -o output -I

LSP_MD3_andris_lapins.o: LSP_MD3_andris_lapins.c
	gcc -c LSP_MD3_andris_lapins

LSP_PD3_andris_lapins.o: LSP_PD3_andris_lapins.c LSP_PD3_andris_lapins.h
	gcc -c LSP_PD3_andris_lapins.c

clean:
	rm *.o output