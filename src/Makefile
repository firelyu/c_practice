CC=gcc
RM=rm

all: list btree markov create_dedup_file

list: list.c
	${CC} list.c -o list
btree: btree.c
	${CC} btree.c -o btree
markov: markov.c
	${CC} markov.c -o markov
create_dedup_file: create_dedup_file.c
	${CC} create_dedup_file.c -o create_dedup_file

debug: markov.dbg create_dedup_file.dbg

markov.dbg: markov.c
	${CC} markov.c -g -o markov.dbg
create_dedup_file.dbg: create_dedup_file.c
	${CC} create_dedup_file.c -g -o create_dedup_file.dbg

clean:
	${RM} list
	${RM} btree
	${RM} markov
	${RM} markov.dbg
	${RM} create_dedup_file
	${RM} create_dedup_file.dbg
