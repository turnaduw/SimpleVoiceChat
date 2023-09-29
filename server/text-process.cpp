#include "svc-database.h"

int findIndexOfChar(const std::string& str, const char& c)
{
  int index = str.find(c);
  if (index != std::string::npos)
    return index;
  else
    return -1;
}

bool spliter(char delimiter, std::string text, std::string& a, std::string& b, int& f)
{

  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;
  a = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);

  text = text.substr(index+1,text.length());
  index = findIndexOfChar(text,delimiter); //Œ
  if(index==-1)
    return false;

  b = text.substr(0, index);
  std::string c = text.substr(index+1, text.length());

  const char* cxx = c.c_str();
  f = std::atoi(cxx);

  return true;

}

bool spliter(char delimiter, std::string text, std::string& a, std::string& b)
{
  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;
  a = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);

  text = text.substr(index+1,text.length());
  index = findIndexOfChar(text,delimiter); //Œ
  if(index==-1)
    return false;

  b = text.substr(0, index);

  return true;
}

bool spliter(char delimiter, std::string& text, std::string& a)
{
  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;
  a = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);
  return true;
}

bool spliter(char delimiter, std::string& text, int& a)
{
  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;

  std::string temp = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);
  const char* cxx = temp.c_str();
  a = std::atoi(cxx);
  return true;
}


bool spliter(char delimiter, std::string& text, int& a, std::string& b)
{
  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;

  std::string temp = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);
  const char* cxx = temp.c_str();
  a = std::atoi(cxx);

  text = text.substr(index+1,text.length());
  index = findIndexOfChar(text,delimiter); //Œ
  if(index==-1)
    return false;

  b = text.substr(0, index);


  return true;
}


bool spliter(char delimiter, std::string& text, int& a, std::string& b, std::string& c,  int& d)
{
  int index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;

  std::string temp = text.substr(SVC_SERVER_REQUEST_CODE_INDEX,index-SVC_SERVER_REQUEST_CODE_INDEX);
  const char* cxx = temp.c_str();
  a = std::atoi(cxx);

  text = text.substr(index+1,text.length());
  index = findIndexOfChar(text,delimiter); //Œ
  if(index==-1)
    return false;

  b = text.substr(0, index);


  text = text.substr(index+1,text.length());
  index = findIndexOfChar(text,delimiter); //Œ
  if(index==-1)
    return false;

  c = text.substr(0, index);


  text = text.substr(index+1,text.length());

  index = findIndexOfChar(text,delimiter);
  if(index==-1)
    return false;

  temp = text.substr(0,index);
  cxx = temp.c_str();
  d = std::atoi(cxx);

  return true;
}










void text_process(std::string& text, SimpleVoiceChat::SVCdb& db,
  std::string ip, unsigned int port,
  bool connected)
{
  if(connected==false)
    if(text[2] != 'A')
      text[2] = 'A';


  switch (text[2])
  {
    case 'A':
    {
      // A.send identity
      if(db.cc_isConnected_ip_port(ip,port))
        return;

      std::string nickname,identity;
      if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,nickname,identity))
      {
          if(identity.length() < SVC_SERVER_MINIMUM_ACCEPTED_IDENTITY_LENGTH)
          {
            std::cout << "client has sent an invalid identity. rejected.\n";
            return;
          }
          db.new_client(nickname,identity,ip,port);
          std::cout << "clients saved list=:\n" << db.get_saved_clients() << std::endl;
          db.cc_print_clients();
      }
      else
      {
        std::cout << "A: invaild syntax because of deleimited cant..\n";
      }



    }break;
    case 'B':
    {
      // B.disconnect from server
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
        if(db.cc_disconnect(cid))
          std::cout << "client disconnnecetd succsesfully.\n";
        else
          std::cout << "failed to disconnect client.\n";
      }

    }break;
    case 'C':
    {
      // C.connect to the channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
        int channel_id=-1;
        std::string entered_password;
        if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,channel_id,entered_password))
        {
          std::cout << "entered_password=" << entered_password << ";";
            if(db.is_channel_password_correct(channel_id, entered_password))
            {
              if(db.cc_channel(cid,channel_id))
                std::cout << "client joint to the channel succsesfully.\n";
              else
                std::cout << "failed join client to the channel.\n";
            }
            else
              std::cout << "incorrect password to join channel.\n";
        }
        else
          std::cout << "invaild syntax because of deleimited cant..\n";
      }

    }break;
    case 'D':
    {
      // D.leave the channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
        if(db.cc_channel(cid,0))
          std::cout << "success to leave client from channel.\n";
        else
          std::cout << "failed to ledave client from channel\n";
      }

    }break;
    case 'E': {} break;
    case 'F':
    {
      // F.send text message to the channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
        std::string message;
        if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,message))
        {
          int channel_id = db.cc_getClient_channel(cid);
          if(channel_id == -1)
            std::cout << "invalid channel id\n";
          else
          {
            if(db.insert_text_message(cid,channel_id,message))
              std::cout << "client has been sent message to channel fine.\n";
            else
              std::cout << "failed to send message from client to channel\n";
          }
        }
        else
          std::cout << "invaild syntax because of deleimited cant..\n";
      }


    }break;
    case 'G':
    {
      // G.create channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
          if(db.is_client_admin(cid))
          {
            std::string title,password;
            int pos;
            if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,title,password,pos))
            {
              db.create_channel(title,password,pos);
              std::cout << "channel created.." << db.get_channels() << std::endl;
            }
            else
            {
              std::cout << "G: invaild syntax because of deleimited cant..\n";
            }
          }
          else
            std::cout << "client is not admin to create channel.\n";
      }
    }break;
    case 'H':
    {
      // H.delete channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
          if(db.is_client_admin(cid))
          {
            int channel_id;
            if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,channel_id))
            {
              if(db.delete_channel(channel_id))
              {
                if(db.cc_channel_deleted_kick_all_out(channel_id))
                  std::cout << "channel deleted.." << db.get_channels() << std::endl;
              }
            }
            else
            {
              std::cout << "invaild syntax because of deleimited cant..\n";
            }
          }
          else
            std::cout << "client is not admin to create channel.\n";
      }


    }break;
    case 'I':
    {
      // I.update channel
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "client is not connected.\n";
      else
      {
          if(db.is_client_admin(cid))
          {
            //int channel_id, std::string title, std::string password, int pos
            int channel_id,pos;
            std::string title,password;
            if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,channel_id,title,password,pos))
            {
               if(db.update_channel(channel_id,title,password,pos))
                std::cout << "channel has been updated.\n";
               else
                std::cout << "channel has been not updated.\n";
            }
            else
            {
              std::cout << "invaild syntax because of deleimited cant..\n";
            }
          }
          else
            std::cout << "client is not admin to create channel.\n";
      }


    }break;
    case 'J':
    {
      // J.rename nickname
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
        std::string new_nickname;
        if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,new_nickname))
        {
          if(db.cc_rename(cid,new_nickname))
            std::cout << "client renamed.\n";
          else
            std::cout << "client can not be rename..\n";
        }
        else
        {
          std::cout << "A: invaild syntax because of deleimited cant..\n";
        }


      }
    }break;
    case 'K':
    {
      // K.speaker mute
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
        db.cc_speaker_muted(cid,true);
        std::cout << "client speaker_muted.\n";
      }
    }break;
    case 'k':
    {
      // k.speaker unmute
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
        db.cc_speaker_muted(cid,false);
        std::cout << "client speaker_unmuted.\n";
      }
    }break;
    case 'L':
    {
      // L.microphone mute
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
        db.cc_micophone_muted(cid,true);
        std::cout << "client microphone_nmuted.\n";
      }
    }break;
    case 'l':
    {
      // l.microphone unmute
      int cid = db.cc_what_is_id(ip,port);
      if(cid == -1)
        std::cout << "G:client is not connected.\n";
      else
      {
        db.cc_micophone_muted(cid,false);
        std::cout << "client microphone_unmuted.\n";
      }
    }break;



    #if DEBUG_TOOL_PRINT_DATA == 1
    case 'V':
    {
      std::cout << "admin list:\n" << db.get_admins() << std::endl;
    }break;
    case 'W':
    {
      std::cout << "channels print:\n" << db.get_channels() << std::endl;
    }break;
    case 'X':
    {
      std::cout << "get_saved_clients list:\n" << db.get_saved_clients() << std::endl;
    }break;
    case 'Y':
    {
      std::cout << "get_ban_list list:\n" << db.get_ban_list() << std::endl;
    }break;
    case '1':
    {
      std::cout << "cc_print_clients list:\n";
      db.cc_print_clients();
    }break;
    #endif





    default:
    {
      std::cout << "unknwon requset code\n;";
    }
  }
}
