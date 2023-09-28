#include "svc-database.h"

//file existant include
#include <boost/filesystem.hpp>

//network includes
#include <boost/asio.hpp>
#include <boost/thread.hpp>
using boost::asio::ip::udp;

//to save received audio as buffer
#include <boost/array.hpp>


void sendData(const boost::asio::ip::address& _ip, const unsigned short& _port, const std::string& _message, udp::socket& _socket)
{
  udp::endpoint send_endpoint(_ip, _port);
  _socket.send_to(boost::asio::buffer(_message), send_endpoint);
}

void receive_text(udp::socket& socket,SimpleVoiceChat::SVCdb& dB)
{
    while (true)
    {
        //Text buffer.
        char receive_data[1024];

        //sender information
        udp::endpoint receive_endpoint;


        //set received data into buffer.
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);
        std::cout << "text data = " << std::string(receive_data, receive_length) << std::endl;
        std::cout << "\n\n\n";


        const unsigned short c_port = receive_endpoint.port();
        const boost::asio::ip::address c_ip = receive_endpoint.address();


        // std::string ip_port = c_ip.to_string() + ":" + std::to_string(c_port);
        // if(dB.is_client_ip_port_exists(ip_port))

    }
}



void receive_voice(udp::socket& socket)
{
    while (true)
    {
        //Audio buffer.
        boost::array<float, SVC_VOICE_FRAMES_PER_BUFFER> audioBuffer;


        //sender information
        udp::endpoint receive_endpoint;

        std::cout << "voice received.\n";
        std::cout << "\n\n\n";


        //set received data into buffer.
        socket.receive_from(boost::asio::buffer(audioBuffer), receive_endpoint);
    }
}

int main()
{
  try
  {
    //read server port from config file beside application.
    //...


    //init or open database and handel INSERT/DELETE/UPDATE/... data by functions.
    SimpleVoiceChat::SVCdb db(boost::filesystem::exists(SVC_DATABASE_NAME));

    db.create_channel("channel a","",1);
    db.create_channel("channel c","1234",2);
    db.create_channel("channel b","78944ABCD",3);

    db.update_channel(1,"channel a","password 1",1);
    db.delete_channel(1);
    std::cout << db.get_channels() << std::endl;


    db.new_client("Max","identitiyMax","127.0.0.0:45612");
    db.new_client("Paul","identitiyPaul","127.0.0.0:10001");
    db.remove_client(2);
    std::cout << db.get_saved_clients() << std::endl;




    db.new_admin(1);
    std::cout << db.get_admins() << std::endl;




    db.ban_client(2,1,"you are not allowed.",1024);
    std::cout << db.get_ban_list() << std::endl;

    std::cout << "clinet 2 banned status = " << db.is_client_banned(2) << std::endl;
    std::cout << "clinet 1 banned status = " << db.is_client_banned(1) << std::endl;

    std::cout << "clinet 1 is_client_id_exists status = " << db.is_client_id_exists(1) << std::endl;
    std::cout << "clinet 10 is_client_id_exists status = " << db.is_client_id_exists(10) << std::endl;

    std::cout << "clinet identitiyMax status = " << db.is_client_identity_exists("identitiyMax") << std::endl;
    std::cout << "clinet identitiyMax2 status = " << db.is_client_identity_exists("identitiyMax2") << std::endl;


    std::cout << "clinet A is_client_ip_port_exists status = " << db.is_client_ip_port_exists("127.0.0.0:45612") << std::endl;
    std::cout << "clinet B is_client_ip_port_exists status = " << db.is_client_ip_port_exists("127.0.0.1:45612") << std::endl;



    std::cout << "channel lock status = " << (db.is_channel_password_correct(2,"1234") ? "allowed" : "denied") << std::endl;


    //network to receive data
    boost::asio::io_context io_context;
    udp::socket socket_text(io_context, udp::endpoint(udp::v4(), SVC_SERVER_TEXT_PORT));
    boost::thread receive_text_thread(boost::bind(receive_text, boost::ref(socket_text), boost::ref(db)));


    boost::asio::io_service io_service;
    udp::socket socket_voice(io_service, udp::endpoint(udp::v4(), SVC_SERVER_VOICE_PORT));
    boost::thread receive_voice_thread(boost::bind(receive_voice, boost::ref(socket_voice)));



    // Wait for threads to finish
    receive_text_thread.join();
    receive_voice_thread.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
