#ifndef H_GAURD_SVC_CONFIG
#define H_GAURD_SVC_CONFIG


#define SVC_DATABASE_NAME "svc_server.db"
#define SVC_SERVER_TEXT_PORT 8888
#define SVC_SERVER_VOICE_PORT 8889


//if identity length is less will not add that client.
#define SVC_SERVER_MINIMUM_ACCEPTED_IDENTITY_LENGTH 5



/*
  for example each TEXT_data received on sever char codes are like: 10 13 120 ..
  two of first is idk why but there is it. so to know what is our first letter what
  is do text[3] is same upside char(120)
  and this number 3 is replaced with this SVC_SERVER_REQUEST_CODE_INDEX
*/
#define SVC_SERVER_REQUEST_CODE_INDEX 3



#define SVC_SERVER_REQUEST_DELIMITER ';'


#define SVC_VOICE_FRAMES_PER_BUFFER 1024
// #define SVC_VOICE_SAMPLE_RATE 44100



//developer tools

#define DEBUG_TOOL_PRINT_DATA 1
#define DEBUG_TOOL_MAKE_ALL_CLIENTS_ADMIN 1

#endif
