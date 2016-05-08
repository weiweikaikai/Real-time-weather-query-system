#include<stdlib.h>
#include<unistd.h>
#include<string>
#include<string.h>
#include<iostream>
#include"sql_connect.h"
using namespace std;

void find(char * data_string,char str[])//data1=XXX&data2=YYY;
{
	if(!data_string){
		return;
	}
	char *data= NULL;
	char *start = data_string;
	while(*start != '\0'){
		if(*start == '=')
        {
			data = start+1;
            break;
        }
         *start++;
	}
    strcpy(str,data);
}

int main()
{
	int content_length = -1;
	char method[1024];
	char query_string[1024];
	char post_data[4096];
    char str[33];
	memset(str, '\0', sizeof(str));
	memset(method, '\0', sizeof(method));
	memset(query_string, '\0', sizeof(query_string));
	memset(post_data, '\0', sizeof(post_data));
	cout<<"<html>"<<endl;
    cout<<"<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"/>"<<endl;
	cout<<"<head> weather </head>"<<endl;
    cout<<"</br>"<<endl;
	cout<<"<body>"<<endl;
	strcpy(method, getenv("REQUEST_METHOD"));
	if( strcasecmp("GET", method) == 0 ){
		strcpy(query_string, getenv("QUERY_STRING"));
        find((char*)query_string,str);//data1=XXX&data2=YYY;
	}else if( strcasecmp("POST", method) == 0 ){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0; 
		for(; i < content_length; i++ ){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		find(post_data,str);//data1=XXX&data2=YYY;
	}else{
		//DO NOTHING
		return 1;
	}

    string host = "127.0.0.1";
    string user = "root";
    string passwd = "WKyun123456";
    string db = "test";
    sql_connecter conn(host,user,passwd,db);
    conn.begin_connect();
   std::string header;
   string sql_data;
   string city_name=(const char*)str;
    cout<<"<p>"<<city_name<<"</p>";
   conn.select_sql(header,sql_data,city_name);
  //  cout<<"<p>"<<header<<"</p>";
   cout<<"<p>"<<sql_data<<"</p>";

	cout<<"</body>\n"<<endl;
	cout<<"</html>\n"<<endl;
}
