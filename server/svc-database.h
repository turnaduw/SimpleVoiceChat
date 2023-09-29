#ifndef H_GAURD_SVC_DATABASE
#define H_GAURD_SVC_DATABASE

#include "config.h"
#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include <cstring>

//convert string to int
#include <cstdlib>

//to figureout which type is tamplated.
#include <type_traits>

namespace SimpleVoiceChat
{
  struct CONNECTED_CLIENTS
  {
    int client_id;
    std::string client_name;
    std::string client_ip;
    unsigned int client_port;
    int client_connected_channel_id;

    // std::string last_time_activity;
    short int ping;
    bool client_speaker_muted;
    bool client_microphone_muted;
    // bool client_away;
  };

  class SVCdb
  {
    private:
      sqlite3* db;
      char* errMsg = nullptr;
      void init_tables();
      std::vector<CONNECTED_CLIENTS> connected_clients;


    public:

      SVCdb(bool);
      ~SVCdb();

      static int search_in_db_callback(void*, int, char**, char**);
      std::string search_in_db(std::string, bool);
      bool query_to_db(std::string);


      //cc-> connected clients , not Database just to vector<struct>
      void cc_print_clients();
      bool cc_connect(int,std::string,std::string,unsigned int, int, short int, bool,bool);
      bool cc_disconnect(int);
      bool cc_rename(int id, std::string new_name);
      bool cc_speaker_muted(int,bool);
      bool cc_micophone_muted(int,bool);
      bool cc_channel(int,int); //switch channel
      int cc_getClient_channel(int);
      bool cc_ping(int,short int);
      bool cc_isConnected_ip_port(std::string,unsigned int);
      int cc_what_is_id(std::string, unsigned int);
      int cc_what_is_index_vector(int);
      bool cc_channel_deleted_kick_all_out(int);

      //insert
      bool insert_text_message(int,int, std::string);
      bool create_channel(std::string, std::string , int);
      bool new_admin(int);
      bool ban_client(int, int, std::string, int);
      bool new_client(std::string, std::string,std::string,unsigned int);

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
      template <typename T>
      bool check_id_returned(T&, std::string&); //blow funcs will send query here and return this result. for check existant
      bool is_client_id_exists(int);
      bool is_client_identity_exists(std::string);
      bool is_client_banned(int);
      bool is_client_admin(int);
      bool is_channel_password_correct(int, std::string);





      std::string get_channels();
      std::string get_ban_list();
      std::string get_saved_clients();
      std::string get_admins();


  };
}

#endif
