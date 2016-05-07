#include "sql_connect.h"

sql_connecter::sql_connecter(const std::string &_host,\
		const std::string &_user,\
		const std::string &_passwd,\
		const std::string &_db)
{
	this->mysql_base = mysql_init(NULL);
	this->res = NULL;
	this->host = _host;
	this->user = _user;
	this->passwd = _passwd;
	this->db   = _db;
}

bool sql_connecter::begin_connect()
{
	if( mysql_real_connect(mysql_base, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 3306, NULL, 0) == NULL){
		std::cerr<<"connect error"<<std::endl;
		return false;
	}else{
		std::cout<<"connect done..."<<std::endl;
	}
	return true;
}

bool sql_connecter::close_connect()
{
	mysql_close(mysql_base);
	std::cout<<"connect close..."<<std::endl;
    return true;
}

bool sql_connecter::select_sql(std::string field_name[],std::string _out_data[][5], int &_out_row)
{
	std::string sql = "SELECT * FROM student";
	if(mysql_query(mysql_base, sql.c_str()) == 0){
		//std::cout<<"query success!"<<std::endl;
	}else{
		//std::cerr<<"query failed!"<<std::endl;
	}
	res = mysql_store_result(mysql_base);
	int row_num = mysql_num_rows(res);
	int field_num = mysql_num_fields(res);
	_out_row = row_num;

	MYSQL_FIELD *fd = NULL;
	int i = 0;
	for(; fd = mysql_fetch_field(res); ){
		field_name[i++] = fd->name;
	}
	//MYSQL_ROW	STDCALL mysql_fetch_row(MYSQL_RES *result);
	for(int index=0; index < row_num; index++){
    	MYSQL_ROW _row = mysql_fetch_row(res);
    	if(_row){
    		int start = 0;
    		for(; start < field_num; start++){
				_out_data[index][start] = _row[start];
    		}
    	}
	}
	return true;
}

bool sql_connecter::insert_sql(const std::string &data)
{
	std::string sql = "insert tianqi (cityname,info) values ";
	sql += "(";
	sql +=data;
	sql +=");";
    std::cout<<sql.c_str()<<std::endl;
	if(mysql_query(mysql_base, sql.c_str()) == 0){
		std::cout<<"query success!"<<std::endl;
		return true;
	}else{
		std::cerr<<"query failed!"<<std::endl;
		return false;
	}
}

bool sql_connecter:: creat_table()
{
	std::string sql = "create table tianqi (id int primary key auto_increment,cityname varchar(30) NOT NULL,info varchar(1000) NOT NULL)charset utf8";
	if(mysql_query(mysql_base, sql.c_str()) == 0){
		//std::cout<<"query success!"<<std::endl;
		return true;
	}else{
		//std::cerr<<"query failed!"<<std::endl;
		return false;
	}
   
}
bool  sql_connecter::delete_table(const std::string &table)
{
	std::string sql = "drop table ";
    sql+=table;
	if(mysql_query(mysql_base, sql.c_str()) == 0){
		//std::cout<<"query success!"<<std::endl;
		return true;
	}else{
		//std::cerr<<"query failed!"<<std::endl;
		return false;
	}

}
sql_connecter::~sql_connecter()
{
	close_connect();
	if(res != NULL){
		free(res);
	}
}

void sql_connecter::show_info()
{
	std::cout<<mysql_get_client_info()<<std::endl;
}
