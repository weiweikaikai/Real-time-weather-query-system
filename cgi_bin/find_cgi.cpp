#include<stdlib.h>
#include<unistd.h>
#include<string>
#include<string.h>
#include<iostream>
#include"sql_connect.h"
using namespace std;

void find(char * data_string)//data1=XXX&data2=YYY;
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
     char str[33];
    strcpy(str,data);
    cout<<str<<endl;
}

int main()
{
	int content_length = -1;
	char method[1024];
	char query_string[1024];
	char post_data[4096];
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
        find((char*)query_string);//data1=XXX&data2=YYY;
	}else if( strcasecmp("POST", method) == 0 ){
		content_length = atoi(getenv("CONTENT_LENGTH"));
		int i = 0; 
		for(; i < content_length; i++ ){
			read(0, &post_data[i], 1);
		}
		post_data[i] = '\0';
		find(post_data);//data1=XXX&data2=YYY;
	}else{
		//DO NOTHING
		return 1;
	}

    string host = "127.0.0.1";
    string user = "root";
    string passwd = "WKyun123456";
    string db = "Tianqi_info";
    sql_connecter conn(host,user,passwd,db);
     conn.begin_connect();


	cout<<"</body>\n"<<endl;
	cout<<"</html>\n"<<endl;
}
