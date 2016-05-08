INCLUDE=-I../sql_connect -I../sql_connect/mysql_lib/include -I./
LIB=-L../sql_connect/mysql_lib/lib -lmysqlclient
BIN=main
SRC=main.cpp  ../sql_connect/sql_connect.cpp
FLAG=-g

$(BIN):$(SRC)
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(FLAG) 
.PHONY:clean
clean:
	rm -rf $(BIN)

