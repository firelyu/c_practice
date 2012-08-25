CC=gcc
RM=rm

all: list btree markov markov1 create_dedup_file

list: list.c
	${CC} list.c -o list
btree: btree.c
	${CC} btree.c -o btree
markov: markov.c
	${CC} markov.c -o markov
markov1: markov1.c
	${CC} markov1.c -o markov1
create_dedup_file: create_dedup_file.c
	${CC} create_dedup_file.c -o create_dedup_file -lcrypto

debug: markov.dbg markov1.dbg create_dedup_file.dbg

markov.dbg: markov.c
	${CC} markov.c -g -o markov.dbg
markov1.dbg: markov1.c
	${CC} markov1.c -g -o markov1.dbg
create_dedup_file.dbg: create_dedup_file.c
	${CC} create_dedup_file.c -g -o create_dedup_file.dbg -lcrypto

clean:
	${RM} -f list
	${RM} -f btree
	${RM} -f markov
	${RM} -f markov.dbg
	${RM} -f markov1
	${RM} -f markov1.dbg
	${RM} -f create_dedup_file
	${RM} -f create_dedup_file.dbg
