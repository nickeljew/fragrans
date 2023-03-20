#include "AppComponent.hpp"
#include "controller/Uploader.hpp"
#include "controller/Downloader.hpp"

#include "oatpp/network/Server.hpp"

#include "iostream"

void run()
{
    AppComponent components;
    
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    router->addController(std::make_shared<Uploader>());
    router->addController(std::make_shared<Downloader>());

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);
    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);
    OATPP_LOGI("App", "Server running on localhost:%s", connectionProvider->getProperty("port").getData());
    server.run();
}

int main(int argc, const char * argv[])
{
    oatpp::base::Environment::init();

    run();

    /* Print how much objects were created during app running, and what have left-probably leaked */
    /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
    std::cout << "\nEnvironment:\n";
    std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
    std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

    oatpp::base::Environment::destroy();
    return 0;
}
