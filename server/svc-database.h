#ifndef H_GAURD_SVC_DATABASE
#define H_GAURD_SVC_DATABASE

#include "config.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include <cstring>
#include <boost/filesystem.hpp>


namespace SimpleVoiceChat
{
  class SVCdb
  {
    private:
      sqlite3* db;
      char* errMsg = nullptr;


      struct CONNECTED_CLIENTS
      {
        int client_id;
        std::string client_ip;
        unsigned int client_port;
        int client_connected_channel_id;
        std::string client_random_session_code;
        // std::string last_time_activity;
        short int ping;
        bool client_speaker_muted;
        bool client_microphone_muted;
        // bool client_away;
      };

      std::vector<CONNECTED_CLIENTS> connected_clients;

    public:
      SVCdb(bool);
      ~SVCdb();
      void init_tables();

      static int search_in_db_callback(void*, int, char**, char**);
      std::string search_in_db(std::string, bool);
      bool query_to_db(std::string);

      //insert
      bool insert_text_message(int,int, std::string);
      bool create_channel(std::string, std::string , int);
      bool new_admin(int);
      bool ban_client(int, int, std::string, int);
      bool new_client(std::string, std::string);

      //delete
      bool delete_message(int);
      bool delete_channel(int);
      bool remove_admin(int);
      bool unban_client(int);
      bool remove_client(int);


      //update
      bool update_channel(int, std::string, std::string , int);
      bool update_banned_client(int, std::string, int);
      bool update_client(int, std::string);


      //search
      bool is_client_id_exists(int);
      bool is_client_identity_exists(std::string);
      bool is_client_banned(int);
      bool is_client_admin(int);
      bool is_channel_password_correct(int, std::string);
      std::string get_channels();
      std::string get_ban_list();

  };
}

#endif
