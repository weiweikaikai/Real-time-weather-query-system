#include<string>  
#include<iostream>  
#include<vector>  
#include<time.h>  
#include<queue>  
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<unistd.h>
#include"sql_connect.h"
using namespace std;  

class data
{
public:
    data(vector<string>&d,char* city_name,vector<string> &we,vector<string> &l_degree,vector<string>&hig_degree):date(d),name(city_name),weather(we),low_degree(l_degree),high_degree(hig_degree)
	{}
    void SaveToSql()
    {
    for(int i=0;i<14;++i)
    {
    cout<<date[i]<<" "<<name<<endl;
    }
    }

    vector<string> date;
	string name;
    vector<string> weather;
    vector<string> low_degree;
	vector<string> high_degree;
};

bool GetHttpResponse(string &resource,char * &pageBuf)
{
string host = "tianqi.114la.com";
struct hostent *hp;
hp = gethostbyname(host.c_str());

if(hp == NULL)
{
cout<<"can't find host address\n";
return false;
}
int sock = socket(PF_INET,SOCK_STREAM,0);

if(sock < 0)
{
 cout<<"create socket error\n";
  return false;
}
struct sockaddr_in address;
bzero(&address,sizeof(address));
address.sin_family = AF_INET;
memcpy(&address.sin_addr.s_addr,hp->h_addr_list[0],hp->h_length);
address.sin_port = htons(80);

int ret = connect(sock,(struct sockaddr*)&address,sizeof(address));

if(ret < 0)
{
  printf("error: connect\n");
  close(sock);
  return false;
}

string request = "GET " + resource+ " HTTP/1.1\r\nHost:"+host+"\r\nConnection:close\r\n\r\n";

if(send(sock,request.c_str(),request.size(),0) < 0)
{
printf("send error\n");
close(sock);
return false;
}

int SZ=1024;
pageBuf = (char*)malloc(SZ);
memset(pageBuf,0,SZ);

ret = 1;
int bytesread =0;
while(ret > 0)
{
ret = recv(sock,pageBuf+bytesread,SZ-bytesread,0);
if(ret > 0)
{
bytesread +=ret;
}

if(SZ-bytesread < 100)
{
    SZ*=2;
pageBuf = (char*)realloc(pageBuf,SZ);
}
}

pageBuf[bytesread] = '\0';

//cout<<"bytesread:"<<bytesread<<endl;
close(sock);
return true;
}

void find_resource(char *resp,vector<string> &vec)
{
char *tag1 = "wordStart";
char *tag2 = "href=\"";
char *pos = strstr(resp,tag1);
if(pos == NULL)
{
cout<<"can't find wordstart"<<endl;
}

char res[100];

while(pos)
{
pos = strstr(pos,tag2);
if(pos == NULL)
{
cout<<"can't find "<<endl;
}
pos += strlen(tag2);
int readnum = sscanf(pos,"%[^\"]",res);
vec.push_back(res);
if(vec.size() == 51)
{
break;
}
}
}

bool  prase(string &str,char *resp)
{
    if(str.size() != 0|| resp == NULL)
    {
    return true;
    }
   char*pos = NULL;
   pos=strstr(resp,"301");
   if(pos == NULL)
   {
//   cout<<"error: strstr\n";
   return true ;
   }
   pos = strstr(resp,"Location:");
   
   pos += 33;
   int i=0;
   while(*pos != '\r')
   {
   str[i++] = *pos++;
   }

   return false;
}

void Get_weather_info(char*resp,vector<data> &weather_data)
{
 char *tag1 = "<title>";
 char *tag2 = "firstWeather";
 char *tag3 = "date";
 char *tag4 = "weather1\">";
 char *tag5 = "red\">";
 char *tag6 = "blue\">";
 
  vector<string> d;
  vector<string> w;
  vector<string> l;
  vector<string> h;
 char *pos = strstr(resp,tag1);
if(pos == NULL)
{
return ;
}
pos += 7;
char city_name[33]={'\0'};
sscanf(pos,"%[^,]",city_name);

city_name[strlen(city_name)-12]='\0';
//cout<<city_name<<endl;

pos = strstr(resp,tag2);
if(pos == NULL)
{
return ;
}
for(int i=0;i<14;++i)
{
pos = strstr(pos,tag3);
if(pos == NULL)
{
return ;
}
pos +=6;
char date[33]={'\0'};
sscanf(pos,"%[^<]",date);
//cout<<date<<endl;
d.push_back(date);

pos = strstr(pos,tag4);
if(pos == NULL)
{
return ;
}
pos +=10;
char weath[33]={'\0'};
sscanf(pos,"%[^<]",weath);
//cout<<weath<<endl;
w.push_back(weath);

pos = strstr(pos,tag6);
if(pos == NULL)
{
return ;
}
pos += 6;
char low_degree[33]={'\0'};
sscanf(pos,"%[^<]",low_degree);
//cout<<low_degree<<endl;
l.push_back(low_degree);

pos = strstr(pos,tag5);
if(pos == NULL)
{
return ;
}
pos +=5;
char high_degree[33]={'\0'};
sscanf(pos,"%[^<]",high_degree);
//cout<<high_degree<<endl;
h.push_back(high_degree);
}
//int num = atoi(high_degree);
//cout<<num<<endl;
//data city_data(date,city_name,weath,low_degree,high_degree);
data city_data(d,city_name,w,l,h);
weather_data.push_back(city_data);
}
bool wea_cmp(data n1,data n2)
{
 return (atoi(n1.high_degree[0].c_str()) > atoi(n2.high_degree[0].c_str()));
}
int main()  
{  
	char* resp = NULL;
	vector<string> vec;
	vector<data> weather_data;
	
    string res_quanguo = "/quanguo";
	GetHttpResponse(res_quanguo,resp);	
     //cout<<resp<<endl;	

    find_resource(resp,vec);
   
//for(int i=0;i<51;++i)
//{
//cout<<vec[i]<<"\n";
//}
   int k=0;
    for(;k<51;++k)
   {
     free(resp);
     resp=NULL;
      GetHttpResponse(vec[k],resp); 
       string str;
       char *pos = strstr(resp,"200 OK");
       if(pos == NULL)
       {
        prase(str,resp);
        string tmp =str.c_str();
        free(resp);
        resp=NULL;
        GetHttpResponse(tmp,resp);
       }
     //cout<<resp<<endl;
     Get_weather_info(resp,weather_data);
     }
     free(resp);
     resp=NULL;

     vector<string> city_name(51);

     city_name[0]="%E9%9E%8D%E5%B1%B1";
     city_name[1]="%E5%8C%97%E4%BA%AC";
  city_name[2]="%E5%8C%97%E6%88%B4%E6%B2%B3";
  city_name[3]="%E9%95%BF%E6%B2%99";
  city_name[4]="%E9%87%8D%E5%BA%86";
  city_name[5]="%E9%95%BF%E6%98%A5";
  city_name[6]="%E5%A4%A7%E8%BF%9E+";
  city_name[7]="%E5%A4%A7%E8%8D%94+";
  city_name[8]="%E4%B8%9C%E8%8E%9E+";
  city_name[9]="%E7%A6%8F%E5%B7%9E";
  city_name[10]="%E4%BD%9B%E5%B1%B1";
  city_name[11]="%E6%A1%82%E6%9E%97";
city_name[12]="%E5%B9%BF%E5%B7%9E+";
city_name[13]="%E6%A1%82%E9%98%B3";
city_name[14]="%E8%B5%A3%E5%B7%9E";
city_name[15]="+%E6%9D%AD%E5%B7%9E";
city_name[16]="%E5%90%88%E8%82%A5";
city_name[17]="%E6%B5%B7%E5%8F%A3";
city_name[18]="%E8%A1%A1%E9%98%B3+";
city_name[19]="%E5%93%88%E5%B0%94%E6%BB%A8";
city_name[20]="%E4%B9%9D%E6%B1%9F";
city_name[21]="%E6%B5%8E%E5%8D%97";
city_name[22]="%E8%BF%9E%E4%BA%91%E6%B8%AF";
city_name[23]="%E8%BF%9E%E4%BA%91%E6%B8%AF";
city_name[24]="%E6%8B%89%E8%90%A8";
city_name[25]="%E5%8D%97%E5%AE%81";
city_name[26]="%E5%8D%97%E6%98%8C";
city_name[27]="%E5%8D%97%E4%BA%AC";
city_name[28]="%E5%8D%97%E9%80%9A";
city_name[29]="%E5%AE%81%E6%B3%A2";
city_name[30]="%E9%9D%92%E5%B2%9B";
city_name[31]="%E6%97%A5%E7%85%A7";
city_name[32]="%E6%B2%88%E9%98%B3+";
city_name[33]="%E6%B7%B1%E5%9C%B3+";
city_name[34]="%E4%B8%8A%E6%B5%B7+";
city_name[35]="%E5%A4%AA%E5%8E%9F+";
city_name[36]="%E5%A4%A9%E6%B4%A5";
city_name[37]="%E4%B9%8C%E9%B2%81%E6%9C%A8%E9%BD%90+";
city_name[38]="%E6%97%A0%E9%94%A1+";
city_name[39]="%E6%AD%A6%E6%B1%89";

     city_name[40]="%E8%A5%BF%E5%AE%89";
     city_name[41]="%E8%A5%BF%E5%AE%81";
     city_name[42]="%E5%BE%90%E5%B7%9E";
     city_name[43]="%E6%89%AC%E5%B7%9E";
     city_name[44]="%E9%83%91%E5%B7%9E";

       string host = "127.0.0.1";
       string user = "root";
       string passwd = "WKyun123456";
       string db = "test";

       sql_connecter conn(host,user,passwd,db);
       conn.begin_connect();
       string lf="'";
       string rf="'";
       string d=",";
       string blank=" ";
       string bo="~";
       string jing="#";
       conn.delete_table();
      conn.creat_table(); 
       for(int j=0;j<45;++j)
       {
string data=lf+city_name[j]+rf+d;
string info=lf+weather_data[j].name+jing;
for(int  i=0;i<14;++i)
{
info+=weather_data[j].date[i]+blank+weather_data[j].name+blank+weather_data[j].weather[i]+blank+weather_data[j].low_degree[i]+bo+weather_data[j].high_degree[i]+jing;
}
data +=info;
data+=rf;
       conn.insert_sql(data);
    }
   sort(weather_data.begin(),weather_data.end(),wea_cmp);
cout<<"\
<!DOCTYPE html>\
<html>\
<head>\
<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\"/>\
<title> 欢迎来到凯微天气预报</title>\
<style type=\"text/css\" >\
.s_btn_wr{\
    width: 100px;\
    height: 36px;\
    color: white;\
    font-size: 15px;\
    letter-spacing: 1px;\
    background: #BD601B;\
    border-bottom: 1px solid #E62A67;\
    outline: medium;\
    -webkit-appearance: none;\
    -webkit-border-radius: 0;\
}\
.s_btn{\
   width: 100px;\
    height: 36px;\
    color: white;\
    font-size: 15px;\
    letter-spacing: 1px;\
    background: #3385ff;\
    margin-left: -10px;\
}\
.s_ipt_wr {\
    border-color: #7b7b7b #b6b6b6 #b6b6b6 #7b7b7b;\
    background: #fff;\
    display: inline-block;\
    vertical-align: top;\
    width: 539px;\
    margin-right: 0;\
    border-right-width: 0;\
    border-color: #b8b8b8 transparent #ccc #b8b8b8;\
    overflow: hidden;\
}\
.s_ipt{\
    width: 526px;\
    height: 30px;\
    font: 16px/18px arial;\
    line-height: 22px\9;\
    margin-left:9px ;\
}\
.word{\
width:500px;\
margin-top: 40px;\
margin-left: 40px;\
font-size: 20px;\
}\
.w1{\
	    margin-top: 50px;\
}\
.bg{\
	margin-left: 300px;\
}\
.s1{\
display: inline-block;\
margin-left: 170px;\
color: #3385ff;\
}\
</style>\
</head>\
<body>\
	<div  class=\"bg\">\
	<span class=\"s1\"><h1 >凯微天气早知道</h1></span>\
<form action=\"./cgi_bin/find_cgi\",method=\"GET\">\
	<input type = \"text\" name = \"cityname\"/>\
	<input type=\"submit\" id=\"su\" value=\"天气搜索\" class=\"s_btn\"></form>       <div class=\"word\">"<<endl;
       for(int i=0;i<(int)weather_data.size();++i)
       {
        cout<<"<p>"<<weather_data[i].date[0]<<": "<<weather_data[i].name<<" "<<weather_data[i].weather[0]<<" "<<weather_data[i].low_degree[0]<<"~"<<weather_data[i].high_degree[0]<<"</p>"<<endl;
       }
       cout<<"</div> </div> <body> </html>"<<endl;

      
    return 0;  
}        
         
