/*************************************************************************
    > File Name: httpd.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Thu 03 Dec 2015 03:07:53 PM CST
 ************************************************************************/
#include"commsocket.h"
#include"threadpool.h"
#include"epoll.h"


int  get_line(int sock,char*buf,size_t max_len)
{
    if(buf == NULL || max_len <0) 
	{
		return 0;
	}
	int i=0;
	char c ='\0'; 
	int n=0;
	while(i<max_len-1 && c!= '\n')
	{
         n= recv(sock,&c,1,0);
		 if(n>0)//succ
		 {
			 if(c == '\r')
			 {
				 n=recv(sock,&c,1,MSG_PEEK);
				 if(n>0 && c=='\n')
				 {
                    recv(sock,&c,1,0);
				 }
				 else
				 {
                    c='\n';
				 }
			 }
            buf[i++] =c;
		 }else//failed
		 {
            c='\n';
		 }
	}
	buf[i]='\0';
	return 1;
}
void bad_request(int client)
{}
void not_found(int client)
{}
void server_error(int client)
{}
void echo_error_to_client()
{}


 void echo_html(int client,const char* path,unsigned int file_size)
{
     if(path == NULL)
	 {
         return;
	 }
	 int in_fd = open(path,O_RDONLY);
	 if(in_fd < 0)
	 {
		 echo_error_to_client();
		 return ;
	 }
	 char echo_line[1024];
	 memset(echo_line,'\0',sizeof(echo_line));
     strncpy(echo_line,SERVICE_VERSION,strlen(SERVICE_VERSION)+1);
	 strcat(echo_line," 200 OK\r\n");
	 strcat(echo_line,"content-Type:text/html;Charset=utf-8\r\n\r\n");
     send(client,echo_line,strlen(echo_line),0);
     printf("service say: %s\n",echo_line);
	 if(sendfile(client,in_fd,NULL,file_size) < 0)
	 {
          close(in_fd);
		  return;
	 }
	 close(in_fd);
}

 void clear_header(int client)
{
    char buf[1024];
	memset(buf,'\0',sizeof(buf));
     int ret=0;
   do
   {
       ret=get_line(client,buf,sizeof(buf));
   }while(ret>0 && strcmp(buf,"\n" ) != 0);
}


void exe_cgi(int sock_client,const char*path,const char*method,const char*query_string)
{
	char buf[BUF_SIZE];
	int content_length=0;
    int numchars = 0;
	int cgi_input[2]={0,0};
	int cgi_output[2]={0,0};

   if(strcasecmp(method,"GET") == 0)//GET
   {
	   clear_header(sock_client);
   }
   if(strcasecmp(method,"POST") == 0)//POST 
   {
        do
		{
			memset(buf,'\0',sizeof(buf));
			numchars =get_line(sock_client,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length:",15) == 0)
			{
             content_length = atoi(&buf[16]);
			}
		}while(numchars>0 && strcmp(buf,"\n") != 0);
   }
   if(content_length == -1)
   {
	echo_error_to_client();
	return;
   }
   memset(buf,'\0',sizeof(buf));
   strcpy(buf,SERVICE_VERSION);
    strcat(buf," 200 OK\r\n");
   strcat(buf,"content-Type:text/html;Charset=utf-8");
	 strcat(buf,"\r\n\r\n");
   send(sock_client,buf,strlen(buf),0);
    
   if(pipe(cgi_input) == -1)
   {
	   return ;
   }
   if(pipe(cgi_output) == -1)
   {
	   close(cgi_input[0]);
	   close(cgi_input[1]);
	   return ;
   }
   pid_t id = fork();
   if(id <0)
   {
	   close(cgi_input[0]);
	   close(cgi_input[1]);
	   close(cgi_output[0]);
	   close(cgi_output[1]);
	   return ;
   }else if(id == 0)
   {
          char query_env[BUF_SIZE/10];
          char method_env[BUF_SIZE];
          char content_len_env[BUF_SIZE];
		  memset(method_env,'\0',sizeof(method_env));
		  memset(query_env,'\0',sizeof(query_env));
		  memset(content_len_env,'\0',sizeof(content_len_env));

		  close(cgi_input[1]);
		  close(cgi_output[0]);

		  dup2(cgi_input[0],0);
		  dup2(cgi_output[1],1);

		  sprintf(method_env,"REQUEST_METHOD=%s",method);
		  putenv(method_env);
		  if(strcasecmp("GET",method) == 0) //GET
		  {
           sprintf(query_env,"QUERY_STRING=%s",query_string);
		   putenv(query_env);
		  }else //POST
		  { 
           sprintf(content_len_env,"QUERY_STRING=%s",content_length);
         putenv(content_len_env);

		  }
     execl(path,path,NULL);
	 exit(1);
   }
   else //father
   {
      close(cgi_input[0]);
	  close(cgi_output[1]);
      
	  int i=0;
	  char c='\0';
	  if(strcasecmp("POST",method) == 0)
	  {
		  for(;i<content_length;++i)
		  {
	        recv(sock_client,&c,1,0);
			write(cgi_input[1],&c,1);
		  }
	  }
	  while(read(cgi_output[0],&c,1) > 0)
	  {
		  send(sock_client,&c,1,0);
	  }
     close(cgi_input[1]);
	 close(cgi_output[0]);
     
	 waitpid(id,NULL,0);
   }

}

void* accept_request(void *arg)
{

   // pthread_detach(pthread_self()); //线程分离
	int sock_client = (int)arg;

	char buf[BUF_SIZE];
	char method[BUF_SIZE/10];
	char url[BUF_SIZE];
    char path[BUF_SIZE];

	memset(buf,0,sizeof(buf));
	memset(url,0,sizeof(url));
	memset(method,0,sizeof(method));
	memset(path,0,sizeof(path));

		int cgi = 0;
    char* query_string=NULL;

     if(get_line(sock_client,buf,sizeof(buf)) < 0)
     {
		 echo_error_to_client();
            return (void*)-1;
	 }
     printf("client say: %s\n",buf);
	 int i=0;
	 int j=0;//buf line index

	 //get method
     while(!isspace(buf[j]) && i<sizeof(method)-1 && j<sizeof(buf)  )
	 {
		 method[i] = buf[j];
		 i++;
		 j++;
	 }
	 //clear space 
	 while(isspace(buf[j]) && j < sizeof(buf))
	 {
		 ++j;
	 }
	 //get url
	 i=0;
	 while(!isspace(buf[j]) && i<sizeof(url)-1 && j<sizeof(buf))
	 {
            url[i] = buf[j];
			i++;
			j++;
	 }
	 //printf("method %s  url %s\n",method,url);
	 if(strcasecmp(method,"GET")<0 && strcasecmp(method,"POST")<0)
	 {
		 echo_error_to_client();
		 return NULL;
	 }
	 if(strcasecmp(method,"POST") == 0)
	 {
		 cgi = 1;
	 }
	 if(strcasecmp(method,"GET") == 0)
	 {
           query_string = url;
           while(*query_string != '?' && *query_string != '\0')
		   {
			   query_string++;
		   }
		   if(*query_string == '?') //url = /add/index?val=0
		   {
              *query_string = '\0';
			   query_string++;
			   cgi = 1;
		   }
	 }
	 sprintf(path,"htdocs%s",url);
     if(path[strlen(path)-1] == '/')
	 {
		 strcat(path,MAIN_PAGE);
	 }

    struct stat st;
	if(stat(path,&st) <0)//failed return not zero
	{
		clear_header(sock_client);
		 echo_error_to_client();
		 return NULL;  
	}else //file exist
	{
           if(S_ISDIR(st.st_mode))
		   {
		      strcat(path,"/");
              strcat(path,MAIN_PAGE);
		   }
		   else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH)
		   {
               cgi =1;
		   }
		   else
		   {

		   }
		   if(cgi == 1)
		   {
              exe_cgi(sock_client,path,method,query_string);
		   }else
		   {
		        clear_header(sock_client);
                echo_html(sock_client,path,st.st_size);
		   }
	}
	close(sock_client);
	return NULL;
}

void usage(const char* proc)
{
      printf("uage %s [ip][port]\n",proc);
}
int main(int argc,char* argv[])
{
	if(argc != 3)
	{
    usage(argv[0]);
	 exit(1);
	}
    short port = atoi(argv[2]);
    const char* ip=argv[1];
    // daemon(1,1);
     int listenfd = Server_init(ip,port);
	 if(listenfd < 0)
	 {
		 printf("func Server_init  err\n");
		 return -1;
	 }
	 activate_nonblock(listenfd);
	 int epfd=My_epoll_create(0);//这里可以加类似于EPOLL_CLOEXEC的fd属性值
	 struct epoll_event event;

	 event.data.fd= listenfd;
	 event.events = EPOLLIN | EPOLLET;
	 My_epoll_add(epfd,listenfd,&event);
      
    threadpool_t *tp = threadpool_init(5);

     int timeout=-1;
      int count=0;
	while(1)
	{
		
		int activenum=My_epoll_wait(epfd,events,MAXEVENTS,timeout);
		if(activenum < 0)
		{
		     printf("epoll_wait error\n");
			 break;
		}
		if(activenum == 0)
		{
		    printf("epoll_wait timeout\n");
			continue;
		}
		int i=0;
		for(;i<activenum;++i)
		{
		  int fd =events[i].data.fd;
		  if(listenfd == fd)
			{
		       int conn=0;
			   while(1)
				{
				     struct sockaddr_in clientaddr;
					 socklen_t len = sizeof(clientaddr);
	                 memset(&clientaddr, 0, sizeof(clientaddr));
			          conn = accept(listenfd, (struct sockaddr *)&clientaddr, &len);
                    if (conn < 0) 
						{
                           if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
							{
							   //处理了所有输入连接
                               break;
                            } else{
								printf("accept error\n");
                                break;
                            }   
			             }
						 ++count;
					printf("%d  a client online ip= %s port= %d\n ",count, inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
				    activate_nonblock(conn);
					event.data.fd = conn;
					event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
					My_epoll_add(epfd, conn, &event);
		         }
				 
		     }else{
			          if ((events[i].events & EPOLLERR) ||
                          (events[i].events & EPOLLHUP) ||
                          (!(events[i].events & EPOLLIN))) 
					    {
                          printf("error fd\n");
                          close(fd);
                          continue;
                        }
			           if(threadpool_add(tp,accept_request,events[i].data.fd) != 0)
				         {
					          printf("threadpool_add error\n");
					     }
			  }
	     }
	}
       if (threadpool_destroy(tp,0) < 0)
		   {
              printf("destroy threadpool failed");
            }

        return  0;
}

