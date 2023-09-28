#include "svc-database.h"

namespace SimpleVoiceChat
{
  SVCdb::SVCdb(bool db_existant=false)
  {
    int rc = sqlite3_open(SVC_DATABASE_NAME, &db);
    if(rc)
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    else
    {
      std::cout << "Database opened/created fine.\n";
      if(!db_existant)
        init_tables();

    }
  }


  SVCdb::~SVCdb()
  {
    sqlite3_close(db);
  }
  void SVCdb::init_tables()
  {
    int rc;
    // Create a table
    const char* createChannelTextMessages = "CREATE TABLE IF NOT EXISTS svc_channel_text_messages("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "channel_id INTEGER NOT NULL,"
                                 "client_id INTEGER NOT NULL,"
                                 "message_text TEXT NOT NULL,"
                                 "commit_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                                 "FOREIGN KEY(channel_id) REFERENCES svc_channels(id),"
                                 "FOREIGN KEY(client_id) REFERENCES svc_clients(id));";
    rc = sqlite3_exec(db, createChannelTextMessages, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error while init table createChannelTextMessages: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }


    const char* createChannels = "CREATE TABLE IF NOT EXISTS svc_channels("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                 "title TEXT,"
                                 "password TEXT,"
                                 "position INTEGER);";
    rc = sqlite3_exec(db, createChannels, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
      std::cout << "SQL error while init table createChannels: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }

    const char* createAdmins = "CREATE TABLE IF NOT EXISTS svc_admins("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "client_id INTEGER NOT NULL,"
    "commit_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY(client_id) REFERENCES svc_clients(id));";
    rc = sqlite3_exec(db, createAdmins, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
      std::cout << "SQL error while init table createAdmins: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }

    const char* createBanList = "CREATE TABLE IF NOT EXISTS svc_banlist("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "client_id INTEGER NOT NULL,"
    "admin_id INTEGER NOT NULL,"
    "banDuration INTEGER,"
    "reason TEXT,"
    "commit_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY(admin_id) REFERENCES svc_admins(id),"
    "FOREIGN KEY(client_id) REFERENCES svc_clients(id));";
    rc = sqlite3_exec(db, createBanList, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
      std::cout << "SQL error while init table createBanList: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }

    const char* createClients = "CREATE TABLE IF NOT EXISTS svc_clients("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nickname TEXT,"
    "identity TEXT NOT NULL,"
    "ip_port TEXT,"
    "total_connection_count INTEGER DEFAULT 0,"
    "account_creation DATETIME DEFAULT CURRENT_TIMESTAMP);";
    rc = sqlite3_exec(db, createClients, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
      std::cout << "SQL error while init table createClients: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }

  }
  bool SVCdb::query_to_db(std::string _q)
  {
        bool result;
        char* final_query = new char[_q.length()+1];
        std::strcpy(final_query, _q.c_str());

        int rc = sqlite3_exec(db, final_query, nullptr, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            std::cout << "SQL error: " << errMsg << "\tquery=" << _q << std::endl;
            sqlite3_free(errMsg);
        }
        else
          result = true;
        delete[] final_query;
        return result;
  }


  int SVCdb::search_in_db_callback(void* data, int argc, char** argv, char** azColName)
  {
    std::string* _result = static_cast<std::string*>(data);
    for (int i = 0; i < argc; i++)
    {
        // std::cout << "i=" << i  << "\t"<< azColName[i] << '=' << (argv[i] ? argv[i] : "NULL") << std::endl;
        *_result += azColName[i];
        *_result += '=';
        *_result +=  (argv[i] ? argv[i] : "NULL");
        *_result += "\n";
    }
    std::cout << std::endl;
    return 0;
  }
  std::string SVCdb::search_in_db(std::string _q, bool outputType=false) // 0->existant data, 1->get data
  {
    char* final_query = new char[_q.length()+1];
    std::strcpy(final_query, _q.c_str());

    std::string result;
    int rc = sqlite3_exec(db, final_query, &SVCdb::search_in_db_callback, &result , &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "SQL search error: " << errMsg << "\tquery=" << _q << std::endl;
        sqlite3_free(errMsg);
    }
    return result;
  }

  //insert
  bool SVCdb::insert_text_message(int client_id, int channel_id, std::string client_text)
  {
    std::string basic_query = "INSERT INTO svc_channel_text_messages (channel_id,client_id,message_text) VALUES('";
    basic_query += std::to_string(channel_id) + "', '" + std::to_string(client_id) + "', '" + client_text + "');";
    return query_to_db(basic_query);
  }
  bool SVCdb::create_channel(std::string title, std::string password, int pos)
  {
    std::string basic_query = "INSERT INTO svc_channels (title,password,position) VALUES('";
    basic_query += title + "', '" + password + "', '" + std::to_string(pos) + "');";
    return query_to_db(basic_query);
  }
  bool SVCdb::new_admin(int client_id)
  {
    std::string basic_query = "INSERT INTO svc_admins (client_id) VALUES('";
    basic_query +=  std::to_string(client_id) + "');";
    return query_to_db(basic_query);
  }
  bool SVCdb::ban_client(int client_id, int admin_id, std::string reason, int banDuration)
  {
    std::string basic_query = "INSERT INTO svc_banlist (client_id,admin_id,reason,banDuration) VALUES('";
    basic_query += std::to_string(client_id) + "', '" + std::to_string(admin_id) + "', '" + reason + "', '" + std::to_string(banDuration)+ "');";
    return query_to_db(basic_query);
  }
  bool SVCdb::new_client(std::string nickname, std::string identity, std::string ip_port)
  {
    std::string basic_query = "INSERT INTO svc_clients (nickname,identity,ip_port) VALUES('";
    basic_query += nickname + "', '" + identity +  "', '" + ip_port + "');";
    return query_to_db(basic_query);
  }

  //delete
  bool SVCdb::delete_message(int message_id)
  {
    std::string basic_query = "DELETE FROM svc_channel_text_messages WHERE id=";
    basic_query += std::to_string(message_id) + ';';
    return query_to_db(basic_query);
  }
  bool SVCdb::delete_channel(int channel_id)
  {
    std::string basic_query = "DELETE FROM svc_channels WHERE id=";
    basic_query += std::to_string(channel_id) + ';';
    return query_to_db(basic_query);
  }
  bool SVCdb::remove_admin(int client_id)
  {
    std::string basic_query = "DELETE FROM svc_admins WHERE client_id=";
    basic_query += std::to_string(client_id) + ';';
    return query_to_db(basic_query);
  }
  bool SVCdb::unban_client(int client_id)
  {
    std::string basic_query = "DELETE FROM svc_banlist WHERE client_id=";
    basic_query += std::to_string(client_id) + ';';
    return query_to_db(basic_query);
  }
  bool SVCdb::remove_client(int client_id)
  {
    std::string basic_query = "DELETE FROM svc_clients WHERE id=";
    basic_query += std::to_string(client_id) + ';';
    return query_to_db(basic_query);
  }


  //update
  bool SVCdb::update_channel(int channel_id, std::string title, std::string password, int pos)
  {
    std::string basic_query = "UPDATE svc_channels SET title='";
    basic_query += title + "', password = '" + password + "', position = '" + std::to_string(pos) + "' WHERE id = " + std::to_string(channel_id) + ";";
    return query_to_db(basic_query);
  }
  bool SVCdb::update_banned_client(int client_id, std::string reason, int banDuration)
  {
    std::string basic_query = "UPDATE svc_banlist SET reason='";
    basic_query += reason + "', banDuration = '" + std::to_string(banDuration) + "' WHERE client_id = " + std::to_string(client_id) + ";";
    return query_to_db(basic_query);
  }
  bool SVCdb::update_client(int client_id, std::string nickname)
  {
    std::string basic_query = "UPDATE svc_clients SET nickname='";
    basic_query += nickname + "' WHERE client_id = " + std::to_string(client_id) + ";";
    return query_to_db(basic_query);
  }


  //search
  template <typename T>
  bool SVCdb::check_id_returned(T& id, std::string& basic_query)
  {
        std::string result_id = search_in_db(basic_query);
        /*
        the query is return a string as result_id
        that variable context is like:   id=10 or identity=fuiewrg.. or ip_port=120.0.0.0..
        so if length result_id is less than 3, query result is not vaild id and not found.
        as using template calling <T> id so maybe its be dirffrent
        */

        if (std::is_integral_v<T>)
        {
          if(result_id.length() < 4) //id=xx.. length minimum is 4
            return false;
          else
            return true;
        }
        else
        {
          if(result_id.length() > 10) //identity and ip_port length are more than 10
            return true;
          else
            return false;
        }
  }
  bool SVCdb::is_client_id_exists(int client_id)
  {
    std::string basic_query = "SELECT id FROM svc_clients WHERE id=";
    basic_query += std::to_string(client_id) + ";";
    return check_id_returned(client_id, basic_query);
  }
  bool SVCdb::is_client_identity_exists(std::string identity)
  {
    std::string basic_query = "SELECT identity FROM svc_clients WHERE identity='";
    basic_query += identity + "';";
    return check_id_returned(identity, basic_query);
  }
  bool SVCdb::is_client_ip_port_exists(std::string ip_port)
  {
    std::string basic_query = "SELECT ip_port FROM svc_clients WHERE ip_port='";
    basic_query += ip_port + "';";
    return check_id_returned(ip_port, basic_query);
  }
  bool SVCdb::is_client_banned(int client_id)
  {
    std::string basic_query = "SELECT id FROM svc_banlist WHERE client_id='";
    basic_query += std::to_string(client_id) + "';";
    return check_id_returned(client_id, basic_query);
  }
  bool SVCdb::is_client_admin(int client_id)
  {
    std::string basic_query = "SELECT id FROM svc_admins WHERE client_id='";
    basic_query += std::to_string(client_id) + "';";
    return check_id_returned(client_id, basic_query);
  }
  bool SVCdb::is_channel_password_correct(int channel_id, std::string entered_password)
  {
    std::string basic_query = "SELECT id FROM svc_channels WHERE id='";
    basic_query += std::to_string(channel_id) + "' AND password='" + entered_password + "';";
    return check_id_returned(channel_id, basic_query);
  }









  std::string SVCdb::get_channels()
  {
    std::string basic_query = "SELECT * FROM svc_channels;";
    return search_in_db(basic_query);
  }
  std::string SVCdb::get_ban_list()
  {
    std::string basic_query = "SELECT * FROM svc_banlist;";
    return search_in_db(basic_query);
  }
  std::string SVCdb::get_saved_clients()
  {
    std::string basic_query = "SELECT * FROM svc_clients;";
    return search_in_db(basic_query);
  }
  std::string SVCdb::get_admins()
  {
    std::string basic_query = "SELECT * FROM svc_admins;";
    return search_in_db(basic_query);
  }
}
