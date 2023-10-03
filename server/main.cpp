#include "svc-database.h"

//file existant include
#include <boost/filesystem.hpp>

//network includes
#include <boost/asio.hpp>
#include <boost/thread.hpp>
using boost::asio::ip::udp;

//to save received audio as buffer
#include <boost/array.hpp>

//codes are in text-process.cpp, to delimite and action received text.
void text_process(std::string&, SimpleVoiceChat::SVCdb&,
  std::string,unsigned int, bool,udp::socket&);


//codes are in voice-process.cpp
void voice_process(const boost::array<float, SVC_VOICE_FRAMES_PER_BUFFER>&,
                   const std::vector<SimpleVoiceChat::CONNECTED_CLIENTS>&,
                   std::string, unsigned int, bool connected, udp::socket&);


#include <boost/core/demangle.hpp>
void receive_text(udp::socket& socket,SimpleVoiceChat::SVCdb& db)
{
    while (true)
    {
        //Text buffer.
        char receive_data[1024];

        //sender information
        udp::endpoint receive_endpoint;

        // std::string receivedString(boost::asio::buffer_cast<char*>(buffer), boost::asio::buffer_size(buffer));
        // std::vector<float> receivedVector(reinterpret_cast<float*>(boost::asio::buffer_cast<char*>(buffer) + receivedString.size()),
                                          // reinterpret_cast<float*>(boost::asio::buffer_cast<char*>(buffer) + boost::asio::buffer_size(buffer)));





        //set received data into buffer.
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);

        std::string rdata(receive_data, receive_length);
        const unsigned short c_port = receive_endpoint.port();
        const boost::asio::ip::address c_ip = receive_endpoint.address();
        std::string _ip = c_ip.to_string();

        text_process(rdata,db, _ip,c_port,
                db.cc_isConnected_text_ip_port(_ip, c_port),socket);
    }
}



void receive_voice(udp::socket& socket,SimpleVoiceChat::SVCdb& db)
{
    while (true)
    {
        //Audio buffer.
        boost::array<float, SVC_VOICE_FRAMES_PER_BUFFER> audioBuffer;


        //sender information
        udp::endpoint receive_endpoint;

        //set received data into buffer.
        socket.receive_from(boost::asio::buffer(audioBuffer), receive_endpoint);
        const unsigned short c_port = receive_endpoint.port();
        const boost::asio::ip::address c_ip = receive_endpoint.address();

        std::string _ip = c_ip.to_string();
        voice_process(audioBuffer,db.connected_clients, _ip,c_port,
                db.cc_isConnected_voice_ip_port(_ip, c_port),socket);

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

    //network to receive data
    boost::asio::io_context io_context;
    udp::socket socket_text(io_context, udp::endpoint(udp::v4(), SVC_SERVER_TEXT_PORT));
    boost::thread receive_text_thread(boost::bind(receive_text, boost::ref(socket_text), boost::ref(db)));


    boost::asio::io_service io_service;
    udp::socket socket_voice(io_service, udp::endpoint(udp::v4(), SVC_SERVER_VOICE_PORT));
    boost::thread receive_voice_thread(boost::bind(receive_voice, boost::ref(socket_voice), boost::ref(db)));



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
