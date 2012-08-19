CC=gcc
RM=rm

all: list btree markov create_dedup_file create_dedup_file2

list: list.c
	${CC} list.c -o list
btree: btree.c
	${CC} btree.c -o btree
markov: markov.c
	${CC} markov.c -o markov
create_dedup_file: create_dedup_file.c
	${CC} create_dedup_file.c -o create_dedup_file
create_dedup_file2: create_dedup_file2.c
	#${CC} create_dedup_file2.c -o create_dedup_file2 -lssl
	# For Mac OS
	${CC} create_dedup_file2.c -o create_dedup_file2 -lcrypto

debug: markov.dbg create_dedup_file.dbg create_dedup_file2.dbg

markov.dbg: markov.c
	${CC} markov.c -g -o markov.dbg
create_dedup_file.dbg: create_dedup_file.c
	${CC} create_dedup_file.c -g -o create_dedup_file.dbg
create_dedup_file2.dbg: create_dedup_file2.c
	#${CC} create_dedup_file2.c -g -o create_dedup_file2.dbg -lssl
	# For Mac OS
	${CC} create_dedup_file2.c -g -o create_dedup_file2.dbg -lcrypto

clean:
	${RM} -f list
	${RM} -f btree
	${RM} -f markov
	${RM} -f markov.dbg
	${RM} -f create_dedup_file
	${RM} -f create_dedup_file.dbg
	${RM} -f create_dedup_file2
	${RM} -f create_dedup_file2.dbg
