#include <boost/program_options.hpp>
#include <crow.h>
#include <iostream>
#include <sorting/Sorting.h>

#include "rankserver/PageBuilder.h"
#include "rankserver/RankServer.h"

int main(int argc, char* argv[])
{
    boost::program_options::options_description args_desc("Options");
    // clang-format off
    args_desc.add_options()
        ("help,h", "print usage")
        ("port,p", boost::program_options::value<int>(), "port to serve on (default: 4000)")
        ("data-dir,d", boost::program_options::value<std::string>(), "data directory to process (default: ./data)");
    // clang-format on

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, args_desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help"))
    {
        std::cout << args_desc << std::endl;
        return 0;
    }

    std::string data_dir = "data";
    if (vm.count("data-dir"))
    {
        data_dir = vm["data-dir"].as<std::string>();
    }
    int port = 4000;
    if (vm.count("port"))
    {
        port = vm["port"].as<int>();
    }

    crow::SimpleApp app;
    app.set_static_dir(data_dir);
    RankServer rankServer(data_dir);

    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::Get,
                                 crow::HTTPMethod::Post)([&rankServer, data_dir](const crow::request& req) {
        if (req.method == crow::HTTPMethod::Post)
        {
            if (req.body[0] == 'z')
            {
                rankServer.resetState();
            }
            else
            {
                if (req.body[0] == 'l')
                {
                    rankServer.submitChoice(sorting::ComparatorResult::LEFT_GREATER);
                }
                else
                {
                    rankServer.submitChoice(sorting::ComparatorResult::LEFT_LESS);
                }
            }
            rankServer.save();
        }
        rankServer.load();
        auto page     = crow::mustache::compile(buildRankserverPage(rankServer.getErr(),
                                                                rankServer.getDone(),
                                                                rankServer.getMsg(),
                                                                rankServer.getRankList(),
                                                                rankServer.getLeftItem(),
                                                                rankServer.getRightItem()));
        auto response = page.render();
        return response;
    });

    app.port(port).multithreaded().run();

    return 0;
}
