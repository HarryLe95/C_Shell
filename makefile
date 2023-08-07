OUTDIR=out

clean:
	rm -rf $(OUTDIR)/*.out $(OUTDIR)/*.o *.out *.o
	clear 

test_D:
	gcc -Wall test_cd.c cd.c -o $(OUTDIR)/test.o -DDEBUG
	./$(OUTDIR)/test.o

test:
	gcc -Wall test_cd.c cd.c -o $(OUTDIR)/test.o 
	./$(OUTDIR)/test.o

minishell:
	gcc -Wall minishell.c -o $(OUTDIR)/minishell.o 

run: minishell 
	./$(OUTDIR)/minishell.o