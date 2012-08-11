CC=gcc
RM=rm

all: list btree markov

list: list.c
	${CC} list.c -o list
btree: btree.c
	${CC} btree.c -o btree
markov: markov.c
	${CC} markov.c -o markov

clean:
	${RM} -f list
	${RM} -f btree
	${RM} -f markov

