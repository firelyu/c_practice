CC=gcc
RM=rm

all: list btree markov

list: list.c
	${CC} list.c -o list
btree: btree.c
	${CC} btree.c -o btree
markov: markov.c
	${CC} markov.c -o markov

debug: markov.dbg

markov.dbg: markov.c
	${CC} markov.c -g -o markov.dbg

clean:
	${RM} -f list
	${RM} -f btree
	${RM} -f markov
	${RM} -f markov.dbg

