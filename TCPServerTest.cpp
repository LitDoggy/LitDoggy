#include "TCPServerTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include <iostream>
#include <list>
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include <time.h>
#include <stdio.h>
#include<string>
//开线程
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

using namespace std;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServerConnectionFactoryImpl;
using Poco::Net::TCPServerParams;
using Poco::Net::StreamSocket;
using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Thread;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
int ID=-1;
int onlinelist[100]={0};//0为在线
int messagelist[100]={0};//0为不需要发消息
int clienttime[100]={0};//各个客户端的时间
struct MES
{
	char groupmessage[1024];
	char user[100];
	int post[100];
};
struct user 
{
	char IP[20];
	char PORT[20];
	char USERNAME[20];
	//char HOSTNAME[20];
};
struct socketname
{
	StreamSocket mysocket;
	char name[20];
	int id;

};

struct socketcon
{
	StreamSocket *mysocket;
	int id;
	char name[20];

};

list<MES> meslist;
list<user> userlist;
list<socketname> socketlist;
list<socketcon> socketconlist;
	char groupmessage[1024];
	char messuser[100];
class EchoConnection: public TCPServerConnection
{
public:
	char receive_message[1024];
	char send_message[1024];
	string AddPort;
	int status;
	int myid;
	long sec;
	char myname[10];
	EchoConnection(const StreamSocket& s): TCPServerConnection(s)
	{
	}	
	void run() 
	{
		ID++;
		sec=0;
		myid=ID;
		status=1;
		StreamSocket& ss = socket();
		AddPort=ss.peerAddress().toString();
		cout<<ss.peerAddress().toString()<<endl;
		try
		{
			//接收握手信息（mi*协议）
			ss.receiveBytes(receive_message, 1024);
			fixmessage(receive_message);
			
			//处理握手信息
			if (shake_hand(receive_message,send_message)==true)		
				{
					ss.sendBytes(send_message, strlen(send_message)+1);
					printf("%s",send_message);
					ss.receiveBytes(receive_message, 1024);
					if(login(receive_message,send_message)==true)
					{
						clienttime[myid]=10;
						ss.sendBytes(send_message, strlen(send_message)+1);
						//跟新socketlist
						printf("%s",send_message);
						socketname tmp;
						strcpy(tmp.name,myname);
						tmp.mysocket=ss;
						socketlist.push_front(tmp);
						printf("%s",send_message);

						socketcon temp;
						temp.mysocket=&ss;
						temp.id=myid;
						strcpy(temp.name,myname);
						socketconlist.push_front(temp);

						//登录成功群发消息
						updateup(receive_message,send_message);
						for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
						{  

							(*iter).mysocket.sendBytes(send_message, strlen(send_message)+1);
							printf("%s",send_message);
						}
						while ((status))
						{
							//beat
							/**/
							if (beat(receive_message,send_message)==true)
							{
								clienttime[myid]=10;
							}
							/////
							if (status==0)
							{
								updatedown(receive_message,send_message);
								if (socketlist.size()>0)
								{
								for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
								{  

									(*iter).mysocket.sendBytes(send_message, strlen(send_message)+1);
									printf("%s",send_message);
								}
								}
							}
							if (status==1)
							{
							int t=ss.receiveBytes(receive_message, 1024);
							receive_message[t]='\0';
							printf("receive===%s",receive_message);
							
							//离开函数
							leave(receive_message,send_message);
							if (status==0)
							{
								updatedown(receive_message,send_message);
								if (socketlist.size()>0)
								{
								for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
								{  

									(*iter).mysocket.sendBytes(send_message, strlen(send_message)+1);
									printf("%s",send_message);
								}
								}
							}
							}
							//请求列表函数
							if (getlist(receive_message,send_message)==true)
							{
								pushlist(receive_message,send_message);
								ss.sendBytes(send_message, strlen(send_message)+1);
								printf("%s",send_message);
							}

							//是否需要upgrade

							//群发消息
							if (c2smessage(receive_message,send_message)==true)
							{
								s2cmessage(receive_message,send_message);
								for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
								{  

									(*iter).mysocket.sendBytes(send_message, strlen(send_message)+1);
								}
							}

						}
					}
					else
					ss.sendBytes(send_message, strlen(send_message)+1);
	
				}
			}
			catch (Poco::Exception& exc)
			{
				for (int i=0;i<100;i++)
					onlinelist[i]=0;
				for (int i=0;i<ID%100;i++)
					onlinelist[i]=1;
				if (userlist.size()>1)
				{
				for(list<user>::iterator iter = userlist.begin(); iter != userlist.end(); iter++)   
				{  
			     if(strcmp(	(*iter).USERNAME ,myname	)==0)
				       userlist.erase(iter); 
				 break;
				}
				}
				else
				{
					userlist.clear();
				}

				if (socketlist.size()>1)
				{
				for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
				{  
						if (strcmp((*iter).name,myname)==0)
							socketlist.erase(iter);
						break;
				}
				}
				else
				{
					socketlist.clear();
				}
								updatedown(receive_message,send_message);
								for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
								{  
									if (strcmp((*iter).name,myname)!=0)
									{
									(*iter).mysocket.sendBytes(send_message, strlen(send_message)+1);
									printf("%s",send_message);
									}
								}
				std::cerr << "EchoConnection: " << exc.displayText() << std::endl;
			}
		}
	bool shake_hand(char *treceive_messgae,char *tsend_message);
	void substr(char* sonstr,char* fatherstr,int start,int end);//截取字符串到sonstr
	void fixmessage(char* message);
	bool login(char *treceive_messgae,char *tsend_message);
	int firchar(char *target,char find);
	void leave(char *treceive_message,char *tsend_message);
	bool getlist(char *treceive_message,char *tsend_message);
	void  pushlist(char *treceive_message,char *tsend_message);
	void  updatedown(char *treceive_message,char *tsend_message);
	bool  c2smessage(char *treceive_message,char *tsend_message);
	void  s2cmessage(char *treceive_message,char *tsend_message);
	bool  beat(char *treceive_message,char *tsend_message);
	void leaveself(char *treceive_message,char *tsend_message);
	void updateup(char *treceive_message,char *tsend_message);
};
void EchoConnection::leaveself(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];
	int start=0;


			  	if (userlist.size()>1)
				{
				for(list<user>::iterator iter = userlist.begin(); iter != userlist.end(); iter++)   
				{  
			     if(strcmp(	(*iter).USERNAME ,myname)==0)
				       userlist.erase(iter); 
				 break;
				}
				}
				else
				{
					userlist.clear();
				}

				if (socketlist.size()>1)
				{
				for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
				{  
						if (strcmp((*iter).name,myname)==0)
							socketlist.erase(iter);
						break;
				}
				}
				else
				{
					socketlist.clear();
				}

			  status=0;
			  //更新在线状态
			  for (int i=0;i<ID%100;i++)
			onlinelist[i]=1;
		
	


}
bool EchoConnection::beat(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];
	strcpy(tmpmessage,treceive_message);
	int start=0;
	int end=firchar(tmpmessage,' ')-1;//第一个提取字的末尾
	substr(substring,treceive_message,start,end);
	//判断是否是miro
	if (strcmp(substring,"CS1.0")==0)
	{
		start=end+2;
		tmpmessage[start-1]='*';
		end=firchar(tmpmessage,' ')-1;
		substr(substring,tmpmessage,start,end);//得到beat
		if (strcmp(substring,"BEAT")==0)
		{
				return true;
		}
	}
	return false;

}
void  EchoConnection::s2cmessage(char *treceive_message,char *tsend_message)
{

					tsend_message[0]='\0';
					time_t t = time(0);
					char tmp[64];
					strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
					char sendmess[256]="";
					strcat(tsend_message,"CS1.0 CSMESSAGE ");
					strcat(tsend_message,messuser);
					strcat(tsend_message,"\r\nDATA ");
					strcat(tsend_message,tmp);
					strcat(tsend_message,"\r\nContent-Length ");
					int len=strlen(groupmessage);
					char strlength[20];
					itoa(len, strlength, 10);
					strcat(tsend_message,strlength);
					strcat(tsend_message,"\r\n\r\n");
					strcat(tsend_message,groupmessage);

}
bool  EchoConnection::c2smessage(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char bodylengthstr[20],username[20];
	int bodylength;
	char ip[20];
	char port[20];
	char tempipport[30];
	strcpy(tmpmessage,treceive_message);
	int start=0;
	int end=firchar(tmpmessage,' ')-1;//第一个提取字的末尾
	substr(substring,treceive_message,start,end);
	//判断是否是miro
	if (strcmp(substring,"CS1.0")==0)
	{
		start=end+2;
		tmpmessage[start-1]='*';
		end=firchar(tmpmessage,' ')-1;
		substr(substring,tmpmessage,start,end);//得到login
		if (strcmp(substring,"MESSAGE")==0)
		{
			//得到name
			start=end+2;
			tmpmessage[start-1]='*';
			end=firchar(tmpmessage,'\r')-1;
			substr(substring,treceive_message,start,end);//
			strcpy(username,substring);  
			end=firchar(tmpmessage,' ')-1;
			//得到日期
			start=end+2;
			tmpmessage[start-1]='*';
			end=firchar(tmpmessage,' ')-1;
 
			//得到主体大小
			start=end+2;
			tmpmessage[start-1]='*';
			end=firchar(tmpmessage,'\r')-1;
			tmpmessage[end+1]='*';
			end=firchar(tmpmessage,'\r')-1;
			tmpmessage[end+1]='*';
			end=firchar(tmpmessage,'\r')-1;
			substr(substring,treceive_message,start,end);//
			strcpy(bodylengthstr,substring);  
			bodylength=atoi(bodylengthstr);
			substr(substring,treceive_message,end+5,end+5+bodylength);//  得到要发的字符串
			MES tmp;
			groupmessage[0]='\0';
			messuser[0]='\0';
			strcpy(groupmessage,substring);
			strcpy(messuser,username);
			return true;

		}
	}
		return false;
}
void  EchoConnection::updatedown(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];



			time_t t = time(0);
			char tmp[64];
			strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
			tsend_message[0]='\0';
			strcat(tsend_message,"CS1.0 UPDATE 0 ");
			strcat(tsend_message,myname);
			strcat(tsend_message,"\r\nDATE ");
			strcat(tsend_message,tmp);
			strcat(tsend_message,"\r\nContent-Length ");
			strcat(tsend_message,"0\r\n\r\n");

}
void  EchoConnection::updateup(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];



			time_t t = time(0);
			char tmp[64];
			strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
			tsend_message[0]='\0';
			strcat(tsend_message,"CS1.0 UPDATE 1 ");
			strcat(tsend_message,myname);
			strcat(tsend_message,"\r\nDATE ");
			strcat(tsend_message,tmp);
			strcat(tsend_message,"\r\nContent-Length ");
			strcat(tsend_message,"0\r\n\r\n");

}

void  EchoConnection::pushlist(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];



			time_t t = time(0);
			char tmp[64];
			strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
			char body[1024]="";
			char myrecord[1024]="";
			for(list<user>::iterator iter = userlist.begin(); iter != userlist.end();iter++ )   
			{  
				if (strcmp((*iter).USERNAME,myname)==0)
				{
					strcat(body,(*iter).USERNAME);
					strcat(body," ");
					strcat(body,(*iter).IP);
					strcat(body," ");
					strcat(body,(*iter).PORT);
					strcat(body,"\r\n");
				}
				else
				{
				strcat(myrecord,(*iter).USERNAME);
				strcat(myrecord," ");
				strcat(myrecord,(*iter).IP);
				strcat(myrecord," ");
				strcat(myrecord,(*iter).PORT);
				strcat(myrecord,"\r\n");
				}
			}
			char strlength[25];
			int len=strlen(body);
			itoa(len, strlength, 10);
			strcat(body,myrecord);
			strcat(body,"\r\n\0");
			tsend_message[0]='\0';
			strcat(tsend_message,"CS1.0 LIST\r\nDate ");
			strcat(tsend_message,tmp);
			strcat(tsend_message,"\r\nContent-Length ");
			strcat(tsend_message,strlength);
			strcat(tsend_message,"0\r\n\r\n");
			strcat(tsend_message,body);


}
int	EchoConnection::firchar(char *target,char find)
{
	int i;
	for (i=0;i<strlen(target);i++)
		if (target[i]==find)
			return i;
	return -1;
}
void EchoConnection::leave(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];
	strcpy(tmpmessage,treceive_message);
	int start=0;
	int end=firchar(tmpmessage,' ')-1;//第一个提取字的末尾
	substr(substring,treceive_message,start,end);
	//判断是否是miro
	if (strcmp(substring,"CS1.0")==0)
	{
		start=end+2;
		tmpmessage[start-1]='*';
		end=firchar(tmpmessage,' ')-1;
		substr(substring,tmpmessage,start,end);//得到login
		if (strcmp(substring,"LEAVE")==0)
		{
			start=end+2;
			tmpmessage[start-1]='*';
			end=firchar(tmpmessage,' ')-1;
			substr(substring,treceive_message,start,end);//得到name
			strcpy(username,substring);  


			  	if (userlist.size()>1)
				{
				for(list<user>::iterator iter = userlist.begin(); iter != userlist.end(); iter++)   
				{  
			     if(strcmp(	(*iter).USERNAME ,username)==0)
				       userlist.erase(iter); 
				 break;
				}
				}
				else
				{
					userlist.clear();
				}

				if (socketlist.size()>1)
				{
				for(list<socketname>::iterator iter = socketlist.begin(); iter != socketlist.end();iter++ )   
				{  
						if (strcmp((*iter).name,myname)==0)
							socketlist.erase(iter);
						break;
				}
				}
				else
				{
					socketlist.clear();
				}

			  status=0;
			  //更新在线状态
			  for (int i=0;i<ID%100;i++)
			onlinelist[i]=1;
		}
	}

}
bool EchoConnection::login(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];
	strcpy(tempipport,AddPort.c_str());
	//char tmp[1024];
	//截取信息版本和信息头(CS1.0 LOGIN)
	strcpy(tmpmessage,treceive_message);
	int start=0;
	int end=firchar(tmpmessage,' ')-1;//第一个提取字的末尾
	substr(substring,treceive_message,start,end);
	//判断是否是miro
	if (strcmp(substring,"CS1.0")==0)
	{
		start=end+2;
		tmpmessage[start-1]='*';
		end=firchar(tmpmessage,' ')-1;
		substr(substring,tmpmessage,start,end);//得到login
		if (strcmp(substring,"LOGIN")==0)
		{
			start=end+2;
			tmpmessage[start-1]='*';
			end=firchar(tmpmessage,'\r')-1;
			substr(substring,treceive_message,start,end);//得到name
			strcpy(username,substring);  
			//得到ip
			substr(substring,tempipport,0,firchar(tempipport,':')-1);
			strcpy(ip,substring); 

			//得到port
			substr(substring,tempipport,firchar(tempipport,':')+1,strlen(tempipport)-1);//得到login
			strcpy(port,substring); 

			user temp;
			strcpy(temp.IP,ip);
			strcpy(temp.PORT,port);
			strcpy(temp.USERNAME,username);
			strcpy(myname,username);
			tsend_message[0]='\0';
			userlist.push_back(temp);
			//返回报文的代码
			time_t t = time(0);
			char tmp[64];
			strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
			char sendmess[256]="";
			strcat(tsend_message,"CS1.0 STATUS 1 \r\nDate ");
			strcat(tsend_message,tmp);
			strcat(tsend_message,"\r\nContent-Length 0\r\n\r\n\0");
			return true;
		}
	}
			tsend_message[0]='\0';
			time_t t = time(0);
			char tmp[64];
			strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
			char sendmess[256]="";
			strcat(tsend_message,"CS1.0 STATUS 0 \r\nDate ");
			strcat(tsend_message,tmp);
			strcat(tsend_message,"\r\nContent-Length 5\r\n\r\nError\0");
	return false;
}
void EchoConnection::fixmessage(char *message)
{
	int i=0;
	while (message[i]!=10)
	i++;
	message[i+1]='\0';
	return ;
}

bool EchoConnection::getlist(char *treceive_message,char *tsend_message)
{
	char substring[1024],tmpmessage[1024];
	char username[20];
	char ip[20];
	char port[20];
	char tempipport[30];
	strcpy(tmpmessage,treceive_message);
	int start=0;
	int end=firchar(tmpmessage,' ')-1;//第一个提取字的末尾
	substr(substring,treceive_message,start,end);
	//判断是否是miro
	if (strcmp(substring,"CS1.0")==0)
	{
		start=end+2;
		tmpmessage[start-1]='*';
		end=firchar(tmpmessage,'\r')-1;
		substr(substring,tmpmessage,start,end);//得到login
		if (strcmp(substring,"GETLIST")==0)
		{
			return true;
		}
	}
	return false;

}


bool EchoConnection::shake_hand(char *treceive_message,char *tsend_message)
{
	char substring[1024];
	//char tmp[1024];
	//截取信息头(miro)
	substr(substring,treceive_message,0,4);
	//判断是否是miro
	if (strcmp(substring,"MINET")==0)
	{
		for (int i=1;i<=strlen(treceive_message);i++)
			tsend_message[i-1]=treceive_message[i];
		tsend_message[0]='M';
		tsend_message[1]='I';
		tsend_message[2]='R';
		tsend_message[3]='O';
		return true;
	}
	 return false;
}

void EchoConnection::substr(char* sonstr,char* fatherstr,int start,int end)
{
	int i;
	for (i=start;i<=end;i++)
		sonstr[i-start]=fatherstr[i];
	sonstr[i-start]='\0';
	return ;
}


TCPServerTest::TCPServerTest(const std::string& name)
{
}


TCPServerTest::~TCPServerTest()
{
}


DWORD WINAPI run(LPVOID p)
{
time_t t = time(0);
printf("打开心跳..\n",p);
char start[64];
strftime( start, sizeof(start), "%Y/%m/%d %X",localtime(&t) );
while (1)
{
if (socketconlist.size()>1)
{
	for(list<socketcon>::iterator iter = socketconlist.begin(); iter != socketconlist.end();iter++ )   
		{  
			if (clienttime[(*iter).id]<=0)
			{
			(*(*iter).mysocket).close();
					socketconlist.erase(iter);


			if (userlist.size()>1)
				{
				for(list<user>::iterator itr = userlist.begin(); itr != userlist.end(); itr++)   
				{  
					if(strcmp(	(*itr).USERNAME ,(*iter).name)==0)
				       userlist.erase(itr); 
				 break;
				}
				}
				else
				{
					userlist.clear();
				}


				if (socketlist.size()>1)
				{
				for(list<socketname>::iterator itr = socketlist.begin(); itr != socketlist.end();itr++ )   
				{  
						if (strcmp((*itr).name,(*iter).name)==0)
							socketlist.erase(itr);
						break;
				}
				}
				else
				{
					socketlist.clear();
				}
				break;
			}
		}
}
else
if (socketconlist.size()==1)
{
		for(list<socketcon>::iterator iter = socketconlist.begin(); iter != socketconlist.end();iter++ )   
		{  
			if (clienttime[(*iter).id]<=0)
			{
			(*(*iter).mysocket).close();
					socketconlist.clear();


			if (userlist.size()>1)
				{
				for(list<user>::iterator itr = userlist.begin(); itr != userlist.end(); itr++)   
				{  
					if(strcmp(	(*itr).USERNAME ,(*iter).name)==0)
				       userlist.erase(itr); 
				 break;
				}
				}
				else
				{
					userlist.clear();
				}


				if (socketlist.size()>1)
				{
				for(list<socketname>::iterator itr = socketlist.begin(); itr != socketlist.end();itr++ )   
				{  
						if (strcmp((*itr).name,(*iter).name)==0)
							socketlist.erase(itr);
						break;
				}
				}
				else
				{
					socketlist.clear();
				}
				break;
			}
		}
}
//减少时间
					char tmp[64];
					t = time(0);
					strftime( tmp, sizeof(tmp), "%Y/%m/%d %X",localtime(&t) );
					if (strcmp(tmp,start)!=0&&socketconlist.size()>0)
					{
						for (int i=0;i<=ID%100;i++)
						{
							if (clienttime[i]>=1)
								clienttime[i]--;
						}
					}
					start[0]='\0';
					strcpy(start,tmp);
}
return (DWORD)p;
}

int main (int argc, const char * argv[])
{
	cout << "服务器" << endl;
	HANDLE three1;    //线程
	DWORD dwThId;    // 线程id号
	three1=CreateThread(NULL,0,run,(LPVOID)1,0,&dwThId);
  // _getch();
    ServerSocket svs(8080);
	TCPServer srv(new TCPServerConnectionFactoryImpl<EchoConnection>(), svs);
	srv.start();	
	while(1);
	CloseHandle(three1);
    return 0;
}