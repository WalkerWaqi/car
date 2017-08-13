#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "OptionPrinter.hpp"
#include "common.h"


class sender
{
public:
  sender(boost::asio::io_service& io_service,
      const boost::asio::ip::address& multicast_address,
      unsigned short multicast_port,
      unsigned int multicast_timeout)
    : endpoint_(multicast_address, multicast_port), timeout_(multicast_timeout),
      socket_(io_service, endpoint_.protocol()),
      timer_(io_service),
      message_count_(0)
  {
    std::ostringstream os;
    os << "pi " << message_count_++;
    message_ = os.str();

    socket_.async_send_to(
        boost::asio::buffer(message_), endpoint_,
        boost::bind(&sender::handle_send_to, this,
          boost::asio::placeholders::error));
  }

  void handle_send_to(const boost::system::error_code& error)
  {
    if (!error)
    {
      timer_.expires_from_now(boost::posix_time::seconds(timeout_));
      timer_.async_wait(
          boost::bind(&sender::handle_timeout, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw std::runtime_error("handle_send_to error");
    }
  }

  void handle_timeout(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::ostringstream os;
      os << "pi " << message_count_++;
      message_ = os.str();

      socket_.async_send_to(
          boost::asio::buffer(message_), endpoint_,
          boost::bind(&sender::handle_send_to, this,
            boost::asio::placeholders::error));
    }
    else
    {
      throw std::runtime_error("handle_timeout error");
    }
  }

private:
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer timer_;
  unsigned int message_count_;
  std::string message_;
  unsigned int timeout_;
};

void init_daemon(void);

int main(int argc, char* argv[])
{
    try
    {
        std::string appName = boost::filesystem::basename(argv[0]);
        std::string     strAddr;
        unsigned short  nPort;
        unsigned int    nTimeout;

        /** Define and parse the program options
         */
        namespace po = boost::program_options;
        po::options_description desc("Options");
        desc.add_options()
                ("help,h", "Print help messages")
                ("address,a", po::value<std::string>(&strAddr)->required(), "multicast address")
                ("port,p", po::value<unsigned short>(&nPort)->required(), "multicast port")
                ("timeout,t", po::value<unsigned int>(&nTimeout)->required(), "send timeout")
                ("daemon,d", "daemon mode")
                ;

        po::positional_options_description positionalOptions;
        positionalOptions.add("out", 1);

        po::variables_map vm;

        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc)
                              .positional(positionalOptions).run(),
                      vm);

            if ( vm.count("help") )
            {
                std::cout << "generate license file" << std::endl << std::endl;
                rad::OptionPrinter::printStandardAppDesc(appName,
                                                         std::cout,
                                                         desc,
                                                         &positionalOptions);
                return SUCCEED;
            }

            po::notify(vm);

            {
                // todo
                if ( vm.count("daemon") )
                {
                  init_daemon();
                }

                boost::asio::io_service io_service;
                sender s(io_service, boost::asio::ip::address::from_string(strAddr), nPort, nTimeout);
                io_service.run();
            }
        }
        catch(boost::program_options::required_option& e)
        {
            rad::OptionPrinter::formatRequiredOptionError(e);
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            rad::OptionPrinter::printStandardAppDesc(appName,
                                                     std::cout,
                                                     desc,
                                                     &positionalOptions);
            return ERROR_IN_COMMAND_LINE;
        }
        catch(boost::program_options::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            rad::OptionPrinter::printStandardAppDesc(appName,
                                                     std::cout,
                                                     desc,
                                                     &positionalOptions);
            return ERROR_IN_COMMAND_LINE;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Unhandled Exception reached the top of main: "
                  << e.what() << ", application will now exit" << std::endl;
        return FAILED;
    }

    return 0;
}
