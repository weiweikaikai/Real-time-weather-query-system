/*************************************************************************
    > File Name: httpd.c
    > Author: WK
    > Mail:18402927708@163.com 
    > Created Time: Thu 03 Dec 2015 03:07:53 PM CST
 ************************************************************************/
#include"commsocket.h"

int sckServer_init(const char*ip,short port, int *listenfd)
{
	int 	ret = 0;
	int mylistenfd;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip);
	
		
	mylistenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (mylistenfd < 0)
	{
		ret = errno ;
		printf("func socket() err:%d \n", ret);
		return ret;
	}
		

	int on = 1;
	ret = setsockopt(mylistenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	if (ret < 0)
	{
		ret = errno ;
		printf("func setsockopt() err:%d \n", ret);
		return ret;
	}
	

	ret = bind(mylistenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		ret = errno ;
		printf("func bind() err:%d \n", ret);
		return ret;
	}
		
	ret = listen(mylistenfd, SOMAXCONN);
	if (ret < 0)
	{
		ret = errno ;
		printf("func listen() err:%d \n", ret);
		return ret;
	}
		
	*listenfd = mylistenfd;

	return 0;
}

int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
	int ret=0;
	socklen_t addrlen = sizeof(struct sockaddr_in);

	if (wait_seconds > 0)
	{
		fd_set accept_fdset;
		struct timeval timeout;
		FD_ZERO(&accept_fdset);
		FD_SET(fd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == -1)
			return -1;
		else if (ret == 0)
		{
			errno = ETIMEDOUT;
			return -1;
		}
	}
	if (addr != NULL)
		ret = accept(fd, (struct sockaddr*)addr, &addrlen);
	else
		ret = accept(fd, NULL, NULL);
		if (ret == -1)
		{
			ret = errno;
			printf("func accept() err:%d \n", ret);
			return ret;
		}
	return ret;
}

int sckServer_accept(int listenfd, int *connfd,  int timeout)
{
	int	ret = 0;
    struct sockaddr_in peeraddr;
	memset(&peeraddr,0,sizeof(peeraddr));
	ret = accept_timeout(listenfd, &peeraddr, (unsigned int) timeout);
	if (ret < 0)
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = Sck_ErrTimeOut;
			printf("func accept_timeout() timeout err:%d \n", ret);
			return ret;
		}
		else
		{
			ret = errno;
			printf("func accept_timeout() err:%d \n", ret);
			return ret;
		}
	}	
	*connfd = ret;
	printf("a client online ip= %s port= %d\n ", inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
	return 0;
}
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
void  not_found(int client)
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
	 strcat(echo_line,"content-Type:text/html;Charset=utf-8");
	 strcat(echo_line,"\r\n\r\n");
     send(client,echo_line,strlen(echo_line),0);
	 send(client,"\r\n",strlen("\r\n"),0);
     printf("%s\n",echo_line);
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
   printf("a clinet is oline........\n");
   pthread_detach(pthread_self()); 
	int sock_client = (int)arg;
	char buf[BUF_SIZE];
	char method[BUF_SIZE/10];
	char url[BUF_SIZE];
    char path[BUF_SIZE];
	int cgi = 0;
    char* query_string=NULL;
	memset(buf,0,sizeof(buf));
	memset(url,0,sizeof(url));
	memset(method,0,sizeof(method));
	memset(path,0,sizeof(path));
//#ifdef _DEBUG_
//	 while(get_line(sock_client,buf,sizeof(buf)) >0 )
//	 {
//             printf("%s\n",buf);
//			 fflush(stdout);
//	 }
//     //     printf("\n");
//#endif
     if(get_line(sock_client,buf,sizeof(buf)) <0)
     {
		 echo_error_to_client();
            return (void*)-1;
	 }

     printf("buf %s\n",buf);
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
	if( stat(path,&st) <0)//failed return not zero
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

void  print_log(const char*fun,int line,int errno,const  char*strerrno)
{
   printf("func [%s] line [%d] errno [%d] strerrno [%s] \n",fun,line,errno,strerrno);
}



void usage(const char* proc)
{
      printf("uage %s [ip][port]\n",proc);
}
int main(int argc,char* argv[])
{
	//if(argc != 3)
	//{
     //usage(argv[0]);
	 //exit(1);
	//}
//    short port = atoi(argv[2]);
  // const char* ip=argv[1];
  daemon(1,1);
  short port =9000;
  const char *ip="121.42.180.114";
   int sock=0;
    int  ret = sckServer_init(ip,port,&sock);
	 if(ret != 0)
	 {
		 printf("func sckServer_init\n");
		 return ret;
	 }
	while(1)
	{
		int wait_second =10;
		int conn=0;
        ret =sckServer_accept(sock,&conn,wait_second);
		  if( ret == Sck_ErrTimeOut)
		  {
			  printf("timeout......");
              continue;
		  }
		  pthread_t new_thread;
		 int ret = pthread_create( &new_thread,NULL,accept_request,conn);
               if(ret < 0)
			   { 
			   }
	}
    
return  0;
}

