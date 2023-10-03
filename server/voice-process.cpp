#include "svc-database.h"

//to save received audio as buffer
#include <boost/array.hpp>

//udp
#include <boost/asio.hpp>
using boost::asio::ip::udp;


int whatIs_index_vector_byIP(
  const std::vector<SimpleVoiceChat::CONNECTED_CLIENTS>& connected_clients,
  const std::string& ip, const unsigned int& port)
{
  for (int i = 0; i < connected_clients.size(); i++)
  {
    if(connected_clients[i].client_ip == ip &&
          connected_clients[i].client_voice_port == port)
    {
      return i;
    }
  }
  return -1;
}


void voice_process (
  const boost::array<float, SVC_VOICE_FRAMES_PER_BUFFER>& audioBuffer,
  const std::vector<SimpleVoiceChat::CONNECTED_CLIENTS>& connected_clients,
  std::string ip, unsigned int port,
  bool connected, udp::socket& _socket)
{
  connected=true;
  if(connected==false)
  {
    std::cout << "you are not allowed to send voice. first send ur identity to server.\n";
  }
  else
  {
    // std::cout << "size audiobuffer = " << audioBuffer.size() << std::endl;
    int index = whatIs_index_vector_byIP(connected_clients,ip,port);
    int targeted_channel_id=0;
    if(index!=-1)
      targeted_channel_id = connected_clients[index].client_connected_channel_id;
    else
    {
      // std::cout << "invalid client vectors index.\n";
      return;
    }


    for (auto i: connected_clients)
    {
      if(i.client_speaker_muted == true)
      {
        // std::cout << "speaker client is muted.. next person les go check\n";
        continue;
      }
      if(i.client_connected_channel_id == targeted_channel_id)
      {
        if(i.client_ip != ip && i.client_voice_port != port)
        {
          boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(i.client_ip);
          udp::endpoint send_endpoint(ip_address, i.client_voice_port);
          _socket.send_to(boost::asio::buffer(audioBuffer), send_endpoint);
        }
      }
    }
  }


}
