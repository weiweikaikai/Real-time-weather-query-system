SEV_SRC=shm_sev
CLI_SRC=shm_cli

SEV_DST=comm.c shm_sev.c
CLI_DST=comm.c shm_cli.c

FLAG=-Wall -O2 -g 
CC=gcc

INCLUDE=./

.PHONY:all
all:$(SEV_SRC) $(CLI_SRC)

$(SEV_SRC):$(SEV_DST)
	$(CC) $(FLAG) $^ -o $@ -I$(INCLUDE)
$(CLI_SRC):$(CLI_DST)	
	$(CC) $(FLAG) $^ -o $@ -I$(INCLUDE)

.PHONY:clean
clean:
	rm -f $(SEV_SRC) $(CLI_SRC)

