CC=gcc
RM=rm

all: csv_parse create_dedup_file markov1 markov btree list
csv_parse: csv_parse.h csv_parse.c
	${CC} csv_parse.c -o csv_parse
create_dedup_file: create_dedup_file.c
	${CC} create_dedup_file.c -o create_dedup_file -lcrypto
markov1: markov1.c
	${CC} markov1.c -o markov1
markov: markov.c
	${CC} markov.c -o markov
btree: btree.c
	${CC} btree.c -o btree
list: list.c
	${CC} list.c -o list

debug: csv_parse.dbg create_dedup_file.dbg

csv_parse.dbg: csv_parse.h csv_parse.c
	${CC} csv_parse.c -g -o csv_parse.dbg
create_dedup_file.dbg: create_dedup_file.c
	${CC} create_dedup_file.c -g -o create_dedup_file.dbg -lcrypto

clean:
	${RM} -f csv_parse
	${RM} -f csv_parse.dbg
	${RM} -f create_dedup_file
	${RM} -f create_dedup_file.dbg
	${RM} -f markov1
	${RM} -f markov
	${RM} -f btree
	${RM} -f list
