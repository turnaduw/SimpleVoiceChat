#include "svc-database.h"
#include "server-response.h"

//udp
#include <boost/asio.hpp>
using boost::asio::ip::udp;



int findIndexOfChar(const std::string& str, const char& c)
{
  int index = str.find(c);
  if (index != std::string::npos)
    return index;
  else
    return -1;
}

bool spliter(char delimiter, std::string text, std::string& a, std::string& b, unsigned int& f)
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

  std::string temp = text.substr(2,index-1);
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






enum SendTarget { CLIENT, CHANNEL, ALL };

void send_text(
  std::string message, char responseCode,
  SendTarget target,
  int targeted_channel_id,
  udp::socket& _socket,
  const std::vector<SimpleVoiceChat::CONNECTED_CLIENTS>& connected_clients,
  const std::string& ip, const unsigned int& port)
{
  message.insert(message.begin(),responseCode);

  switch (target)
  {
    case CLIENT:
    {
      boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(ip);
      udp::endpoint send_endpoint(ip_address, port);
      _socket.send_to(boost::asio::buffer(message), send_endpoint);
    }break;
    case CHANNEL:
    {
      for (auto i: connected_clients)
      {
        if(i.client_connected_channel_id == targeted_channel_id)
        {
          boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(i.client_ip);
          udp::endpoint send_endpoint(ip_address, i.client_text_port);
          _socket.send_to(boost::asio::buffer(message), send_endpoint);
        }
      }
    }break;
    case ALL:
    {
      for (auto i: connected_clients)
      {
        boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(i.client_ip);
        udp::endpoint send_endpoint(ip_address, i.client_text_port);
        _socket.send_to(boost::asio::buffer(message), send_endpoint);
      }
    }break;
    default:
      std::cout << "failed to undrestand to send where, CLIENT,CHANNEL,SERVER??\n";
  }

}

void text_process(std::string& text, SimpleVoiceChat::SVCdb& db,
  std::string ip, unsigned int port,
  bool connected, udp::socket& _socket)
{
  const int INDEX_REQUEST_CODE = 1;
  if(connected==false)
    if(text[INDEX_REQUEST_CODE] != 'A')
      text[INDEX_REQUEST_CODE] = 'A';

  switch (text[INDEX_REQUEST_CODE])
  {
    case 'A':
    {
      // A.send identity
      if(db.cc_isConnected_text_ip_port(ip,port)) //client is exists.
        return;

      std::string nickname,identity;
      unsigned int port_voice;

      if(spliter(SVC_SERVER_REQUEST_DELIMITER,text,nickname,identity,port_voice))
      {
          if(identity.length() < SVC_SERVER_MINIMUM_ACCEPTED_IDENTITY_LENGTH)
          {
            send_text(MCF_INVAILD_IDENTITY, RC_CONNECTION_REFEUSED, SendTarget::CLIENT, 0,
                  _socket,db.connected_clients,ip,port);
            // std::cout << "client has sent an invalid identity. rejected.\n";
            return;
          }
          db.new_client(nickname,identity,ip,port,port_voice);
          // std::cout << "clients saved list=:\n" << db.get_saved_clients() << std::endl;
          // db.cc_print_clients();
          send_text(MAS_CLIENT_CONNECTED, RC_CONNECTED, SendTarget::ALL, 0,
                _socket,db.connected_clients,ip,port);
      }
      else
      {
        // std::cout << "A: invaild syntax because of deleimited cant..\n";
        send_text(MCF_INVAILD_REQUEST_SYNTAX, RC_INVAILD_REQUEST_CODE_OR_SYNTAX, SendTarget::CLIENT, 0,
              _socket,db.connected_clients,ip,port);
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
        {
          send_text(MAS_CLIENT_DISCONNECTED, RC_DISCONNECTED, SendTarget::ALL, 0,
                _socket,db.connected_clients,ip,port);
          // std::cout << "client disconnnecetd succsesfully.\n";
        }
        else
        {
          // send_text(MCF_CLIENT_ISNT_DISCONNECT, RC_DISCONNECTED, SendTarget::CLIENT, 0,
          //       _socket,db.connected_clients,ip,port);
          std::cout << "failed to disconnect client.\n";
        }
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
            if(db.is_channel_password_correct(channel_id, entered_password))
            {
              if(db.cc_channel(cid,channel_id))
              {
                send_text(MAS_JOINT_CHANNEL, RC_JOINT_TO_CHANNEL, SendTarget::ALL, 0,
                      _socket,db.connected_clients,ip,port);
                // std::cout << "client joint to the channel succsesfully.\n";
              }
              else
                std::cout << "failed join client to the channel.\n";
            }
            else
            {
              send_text(MCF_CHANNEL_PASSWORD_INCORRECT, RC_INCORRECT_CHANNEL_PASSWORD, SendTarget::CLIENT, 0,
                    _socket,db.connected_clients,ip,port);
              // std::cout << "incorrect password to join channel.\n";
            }
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
        if(db.cc_getClient_channel(cid) > 0)
        {
          if(db.cc_channel(cid,0))
            send_text(MAS_LEFT_CHANNEL, RC_LEFT_FROM_CHANNEL, SendTarget::ALL, 0,
                  _socket,db.connected_clients,ip,port);
          else
            std::cout << "failed to ledave client from channel\n";
        }
        else
          std::cout << "you cant leave channel 0\n";
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
          else if(channel_id == 0)
            std::cout << "no one hears you. you are not in channel.\n";
          else
          {
            if(db.insert_text_message(cid,channel_id,message))
            {
              std::string _name = db.cc_getClient_nickname(cid);
              std::string _message = _name + message + "\n";
                send_text(_message, RC_TEXT_MESSAGE, SendTarget::CHANNEL, channel_id,
                      _socket,db.connected_clients,ip,port);
                // std::cout << "client has been sent message to channel fine.\n";
            }
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
              send_text(MAS_CHANNEL_CREATED, RC_CHANNEL_CREATED, SendTarget::ALL, 0,
                    _socket,db.connected_clients,ip,port);

              // std::cout << "channel created.." << db.get_channels() << std::endl;
            }
            else
            {
              std::cout << "G: invaild syntax because of deleimited cant..\n";
            }
          }
          else
          {
            send_text(MCF_YOU_ARE_NOT_ADMIN, RC_YOU_DONT_HAVE_PERMISSION, SendTarget::CLIENT, 0,
                  _socket,db.connected_clients,ip,port);
            // std::cout << "client is not admin to create channel.\n";
          }
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
                {
                  send_text(MAS_CHANNEL_DELETED, RC_CHANNEL_DELETED, SendTarget::ALL, 0,
                        _socket,db.connected_clients,ip,port);
                  // std::cout << "channel deleted.." << db.get_channels() << std::endl;
                }
              }
            }
            else
            {
              std::cout << "invaild syntax because of deleimited cant..\n";
            }
          }
          else
          {
            send_text(MCF_YOU_ARE_NOT_ADMIN, RC_YOU_DONT_HAVE_PERMISSION, SendTarget::CLIENT, 0,
                  _socket,db.connected_clients,ip,port);
            // std::cout << "client is not admin to delete channel.\n";
          }
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
               {
                 send_text(MAS_CHANNEL_UPDATED, RC_CHANNEL_UPDATED, SendTarget::ALL, 0,
                       _socket,db.connected_clients,ip,port);
                 // std::cout << "channel has been updated.\n";
               }
               else
                std::cout << "channel has been not updated.\n";
            }
            else
            {
              std::cout << "invaild syntax because of deleimited cant..\n";
            }
          }
          else
          {
            send_text(MCF_YOU_ARE_NOT_ADMIN, RC_YOU_DONT_HAVE_PERMISSION, SendTarget::CLIENT, 0,
                  _socket,db.connected_clients,ip,port);
            // std::cout << "client is not admin to update channel.\n";
          }
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
          {
            send_text(MAS_CLIENT_RENAMED, RC_CLIENT_RENAMED, SendTarget::ALL, 0,
                  _socket,db.connected_clients,ip,port);
            // std::cout << "client renamed.\n";
          }
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
        send_text(MAS_CLIENT_SPEAKER_MUTED, RC_CLIENT_SPEAKER_MUTED, SendTarget::CLIENT, 0,
              _socket,db.connected_clients,ip,port);
        // std::cout << "client speaker_muted.\n";
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
        send_text(MAS_CLIENT_SPEAKER_UNMUTED, RC_CLIENT_SPEAKER_UNMUTED, SendTarget::CLIENT, 0,
              _socket,db.connected_clients,ip,port);
        // std::cout << "client speaker_unmuted.\n";
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
        send_text(MAS_CLIENT_MICROPHONE_MUTED, RC_CLIENT_MICROPHONE_MUTED, SendTarget::CLIENT, 0,
              _socket,db.connected_clients,ip,port);
        // std::cout << "client microphone_nmuted.\n";
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
        send_text(MAS_CLIENT_MICROPHONE_UNMUTED, RC_CLIENT_MICROPHONE_UNMUTED, SendTarget::CLIENT, 0,
              _socket,db.connected_clients,ip,port);
        // std::cout << "client microphone_unmuted.\n";
      }
    }break;

    // case 'M':
    // {
    //   // M.ban client
    //
    // }break;




    #if DEBUG_TOOL_PRINT_DATA == 1
    case '1':
    {
      std::cout << "admin list:\n" << db.get_admins() << std::endl;
    }break;
    case '2':
    {
      std::cout << "channels print:\n" << db.get_channels() << std::endl;
    }break;
    case '3':
    {
      std::cout << "get_saved_clients list:\n" << db.get_saved_clients() << std::endl;
    }break;
    case '4':
    {
      std::cout << "get_ban_list list:\n" << db.get_ban_list() << std::endl;
    }break;
    case '5':
    {
      std::cout << "cc_print_clients list:\n";
      db.cc_print_clients();
    }break;
    #endif





    default:
    {
      send_text(MCF_UNKNOWN_REQUEST_CODE, RC_INVAILD_REQUEST_CODE_OR_SYNTAX, SendTarget::CLIENT, 0,
            _socket,db.connected_clients,ip,port);
      // std::cout << "unknwon requset code\n;";
    }
  }
}
