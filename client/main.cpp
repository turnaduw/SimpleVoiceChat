#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

#include <portaudio.h>
#include <boost/array.hpp>


//pick random number
#include <cstdlib>
#include <ctime>



//boost socket & thread
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::udp;


std::string server_ip="127.0.0.1";
unsigned short server_text_port=8888;
unsigned short server_voice_port=8889;

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1024

int port_text,port_voice;

boost::asio::io_service ioService;
boost::asio::ip::udp::socket socket_voice(ioService);


std::string draft_text, main_chat_text;


//setup window and UI elements
sf::Vector2f viewSize(600,600);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Chat", sf::Style::Default);




sf::Font font;
sf::Text text,chat;


bool microphone_is_muted=false;
bool speaker_is_muted=false;

static int send_voice(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData)
{
    if(!microphone_is_muted)
    {
      // Cast the input buffer to the desired format
      const float* in = static_cast<const float*>(inputBuffer);

      boost::asio::ip::udp::endpoint udpEndpoint(boost::asio::ip::address::from_string(server_ip), server_voice_port);

      // Send the audio data to the server
      socket_voice.send_to(boost::asio::buffer(in, sizeof(float) * framesPerBuffer), udpEndpoint);
    }
    return paContinue;
}


void send_text(udp::socket& socket,std::string send_message)
{
    udp::endpoint send_endpoint(boost::asio::ip::address::from_string(server_ip), server_text_port);
    socket.send_to(boost::asio::buffer(send_message), send_endpoint);
}









void receive_text(udp::socket& socket)
{
    while (true)
    {
        char receive_data[1024];
        udp::endpoint receive_endpoint;
        size_t receive_length = socket.receive_from(boost::asio::buffer(receive_data), receive_endpoint);

        std::string rdata(receive_data,receive_length);
        switch (rdata[0])
        {
          case 'Y':
          {
            microphone_is_muted=true;
          }break;
          case 'Z':
          {
            microphone_is_muted=false;
          }break;

          case 'Q':
          {
            speaker_is_muted=true;
          }break;
          case 'R':
          {
            speaker_is_muted=false;
          }break;
          default:
          {
            std::cout << "can not undrestand rdata[0]\n";
          }
        }

        main_chat_text += (std::string(receive_data, receive_length));
    }
}


static int receive_voice(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData)
{
    if(!speaker_is_muted)
    {
      // Cast the output buffer to the desired format
      float* out = static_cast<float*>(outputBuffer);

      // Receive audio data from client
      boost::array<float, FRAMES_PER_BUFFER> recvBuffer;
      boost::asio::ip::udp::endpoint senderEndpoint;
      socket_voice.receive_from(boost::asio::buffer(recvBuffer), senderEndpoint);

      for(int i=0;i<=99;i++)
        std::cout << recvBuffer[i] << " ";

      // Copy received audio data to the output buffer
      std::copy(recvBuffer.begin(), recvBuffer.end(), out);

    }
    return paContinue;
}

//init Texts
void init()
{

  //init chat
  if(!font.loadFromFile("arial.ttf"))
    std::cout << "error while loading font. arial.ttf not found..\n";

  text.setFont(font);
  text.setCharacterSize(17);
  text.setString("");
  text.setFillColor(sf::Color::White);
  text.setStyle(sf::Text::Bold);
  text.setPosition(25,viewSize.y-70);

  chat.setFont(font);
  chat.setString("");
  chat.setCharacterSize(14);
  chat.setFillColor(sf::Color::White);
  chat.setStyle(sf::Text::Bold);
  chat.setPosition(25,0);
}

void keyboardKeyPressed(sf::Event event,udp::socket* socket)
{
  if(event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Return)
  {
        // std::cout << "draft=" << draft_text.length() <<"?\n";

        send_text(*socket,draft_text);

        draft_text = "";
  }
  if(event.key.code == sf::Keyboard::BackSpace)
  {
    draft_text = draft_text.substr(0, draft_text.length()-2);
  }
}

void keyboardTextEntered(sf::Event event)
{
  if (event.text.unicode < 128)
  {
      //do wrap text when its too long
      draft_text += static_cast<char>(event.text.unicode);
  }
}


void inputs(udp::socket* socket)
{
	sf::Event event;
	while(window.pollEvent(event))
	{
    switch(event.type)
    {
      //keyboard
      case sf::Event::KeyPressed:
        keyboardKeyPressed(event,socket);
        break;
      case sf::Event::TextEntered:
        keyboardTextEntered(event);
        break;
    }
  }
}




bool isPortBusy(int randomNumber, int& _port)
{
  //lets check port is in use or not
  boost::asio::io_service ioService;
  boost::asio::ip::tcp::socket socket(ioService);
  try
  {
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), randomNumber);
      socket.open(endpoint.protocol());
      socket.bind(endpoint);
      std::cout << "Port is available port = " << _port << std::endl;
      _port = randomNumber;
      return false;
  }
  catch (std::exception& e)
  {
      std::string errorMsg = e.what();
      if (errorMsg.find("bind: Address already in use") != std::string::npos)
      {
          std::cerr << "Port is already in use port = " << _port << std::endl;
          return true;
      }
      else
      {
          std::cerr << "Exception: " << errorMsg << std::endl;
          return true;
      }
  }
}


int genrate_random_number(int min = 3000, int max = 64000)
{
    static bool seeded = false; // Flag to ensure seeding only happens once

    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }

    int randomNumber = std::rand() % (max - min + 1) + min;
    return randomNumber;
}


void pickPort(int& var)
{
  bool result=false;
  do
  {
    result = isPortBusy(genrate_random_number(),var);
  } while(result);
}



void init_connection_server_send_identity_n_ports(udp::socket* socket)
{
  std::string _name="test_username";
  std::string _identity="test_identity_random_" + std::to_string(genrate_random_number());
  std::string final = _name + ";" + _identity + ";" + std::to_string(port_voice) + ";";
  send_text(*socket,final);
  send_text(*socket,std::string("test."));
}

int main()
{
  try
  {

      pickPort(port_text);
      pickPort(port_voice);
      //text
      boost::asio::io_context io_context;
      udp::socket socket_text(io_context, udp::endpoint(udp::v4(), port_text));
      boost::thread receive_text_thread(boost::bind(receive_text, boost::ref(socket_text)));


      //voice
      Pa_Initialize();


          int numDevices = Pa_GetDeviceCount();
          if (numDevices < 0) {
              std::cerr << "Error: Pa_GetDeviceCount() failed with error code " << numDevices << std::endl;
              return 1;
          }
          std::cout << "Available input devices:" << std::endl;
             for (int i = 0; i < numDevices; i++) {
                 const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
                 if (deviceInfo->maxInputChannels > 0) {
                     std::cout << "Device ID: " << i << ", Name: " << deviceInfo->name << std::endl;
                 }
             }
             std::cout << "Available output devices:" << std::endl;
            for (int i = 0; i < numDevices; i++) {
                const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
                if (deviceInfo->maxOutputChannels > 0) {
                    std::cout << "Device ID: " << i << ", Name: " << deviceInfo->name << std::endl;
                }
            }

      PaStream* microphone_stream;
      Pa_OpenDefaultStream(&microphone_stream, 1, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, send_voice, nullptr);
      std::cout << "Recording audio from microphone and sending to server..." << std::endl;
      std::cout << "ports= " << port_text << "\t" << port_voice << std::endl;

      // socket_voice.open(boost::asio::ip::udp::v4(),port_voice);
      boost::asio::ip::udp::endpoint endpoint_voice(boost::asio::ip::udp::v4(), port_voice);
      socket_voice.open(endpoint_voice.protocol());
      socket_voice.bind(endpoint_voice);

      //play voice
      PaStream* speaker_stream;
      Pa_OpenDefaultStream(&speaker_stream, 0, 1, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, receive_voice, nullptr);


      Pa_StartStream(microphone_stream);
      Pa_StartStream(speaker_stream);


      init_connection_server_send_identity_n_ports(&socket_text);

      //window settings
      window.setFramerateLimit(60);
      window.setKeyRepeatEnabled(true);


    	init();
    	while(window.isOpen())
    	{
        inputs(&socket_text);

        //render things
        window.clear(sf::Color::Black);

        text.setString(draft_text);
        chat.setString(main_chat_text);

        window.draw(text);
        window.draw(chat);

        window.display();
    	}

      // Wait for threads to finish
      receive_text_thread.join();
  }
  catch (std::exception& e)
  {
    std::string errorMsg = e.what();
    if (errorMsg.find("bind: Address already in use") != std::string::npos)
        std::cerr << "Socket bind failed: Port Address already in." << std::endl;
    else
        std::cerr << "Exception: " << errorMsg << std::endl;
  }
  return 0;
}
