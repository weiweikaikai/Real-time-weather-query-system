INCLUDE=-I../sql_connect -I../sql_connect/mysql_lib/include -I./
LIB=-L../sql_connect/mysql_lib/lib -lmysqlclient
BIN=find_cgi
SRC=find_cgi.cpp  ../sql_connect/sql_connect.cpp
FLAG=-g

$(BIN):$(SRC)
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(FLAG) 

.PHONY:output
output:
	#cp debug_cgi ../output/htdocs/cgi_bin 
	cp find_cgi ../htdocs/cgi_bin 
	#cp show_student_cgi ../output/htdocs/cgi_bin
	#cp register_cgi ../output/htdocs/cgi_bin

.PHONY:clean
clean:
	rm -rf  find_cgi
