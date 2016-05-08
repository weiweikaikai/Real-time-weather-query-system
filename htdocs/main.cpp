/*************************************************************************
	> File Name: main.cpp
	> Author: wk
	> Mail: 18402927708@163.com
	> Created Time: Sun 08 May 2016 10:56:50 AM CST
 ************************************************************************/

#include<iostream>
using namespace std;
#include"sql_connect.h" 
int main()
{
       string host = "127.0.0.1";
       string user = "root";
       string passwd = "WKyun123456";
       string db = "test";

       sql_connecter conn(host,user,passwd,db);
       conn.begin_connect();
       string lf="'";
       string rf="'";
       string d=",";
       string nn="dfsdsa";
       string inf="ha";
       string data="'xian','hello weikai'";
       conn.delete_table();
        conn.creat_table(); 
       conn.insert_sql(data);
    return 0;
}

