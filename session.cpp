#include "session.h"
#include <termios.h>
#include "HttpRequest.h"

session::session(boost::asio::io_service& io_service,
  std::map <std::string, std::string> function_mapping)
  : socket_(io_service), function_mapping(function_mapping) {}

tcp::socket& session::socket()
{
  return socket_;
}

void session::start()
{
  std::cout << "========= Starting session =========" << std::endl;
  
  boost::asio::async_read_until(socket_, buffer, "\r\n\r\n",
      boost::bind(&session::handle_request, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
}

int session::handle_request(const boost::system::error_code& error,
    size_t bytes_transferred){

  std::vector<char>message_request = convert_buffer();
  HttpRequest request(message_request);

  std::string url = request.getUrl();
  std::string function = get_function_from_url(url);

  std::cout << "Function: " << function << std::endl;

  if (!error){
    if (function == "echo_dir")
      handle_write(error, bytes_transferred);
    else if (function == "static_dir"){
      //stuff to handle static file handling goes here. 
      //we should modularize our code so there are separate classes 
      //for sending static files and for sending echos
      std::cout << "Static File Handling Yet To Be Implemented" << std::endl;
    }
    else{
      //error case send error response
    }

  } 
  else{
    std::cerr << error.message() << std::endl;
    return -1;
  }
  return 0;

}


int session::handle_write(const boost::system::error_code& error,
    size_t bytes_transferred)
{

  std::cout << "========= Writing out =========" << std::endl;

  // output buffer
  boost::asio::streambuf out_streambuf;
  
  //sets up the output buffer with the correct headers
  setup_obuffer(out_streambuf, bytes_transferred);
  
  if (!error)
  {
    boost::asio::write(socket_, out_streambuf);
    
    // wait for transmission Note: this could hang forever
    tcdrain(socket_.native_handle());

    // close socket
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close();
    
    std::cout << "========= Ending Session =========" << std::endl;
  }
  else
  {
    std::cerr << error.message() << std::endl;
    return -1;
  }
  return 0;
}

void session::setup_obuffer(boost::asio::streambuf& out_streambuf, size_t bytes_transferred)
{
  std::ostream out(&out_streambuf);
  // setup headers
  out << "HTTP/1.1 200 OK\r\n";
  out << "Content-Type: text/plain\r\n";
  // minus 4 for the double carriage return
  out << "Content-Length: " << bytes_transferred - 4 << "\r\n\r\n";
  
  // echo message under headers
  out << &buffer;
}

std::vector<char> session::convert_buffer()
{
  std::vector<char>converted_vector;

  std::string s{
    buffers_begin(buffer.data()),
    buffers_end(buffer.data())
  };
  
  
  std::copy(s.begin(), s.end(), std::back_inserter(converted_vector));
  return converted_vector;
}

std::string session::get_function_from_url(std::string url)
{
  std::string function = "";
  if (url.length() == 0)
    return function;
  if (url.length() == 1 && url == "/")
    return "/";

  int second_slash_pos = url.find("/", 1);
  std::string dir = url.substr(0, second_slash_pos);

  std::map<std::string, std::string>::iterator it = function_mapping.find(dir);
  if (it != function_mapping.end()){
     function = it->second;
  }
  else{
    function = "error";
  }
  return function;
}

