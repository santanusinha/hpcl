#include <iostream>
#include <unistd.h>

#include "data_component_factory.h"
#include "name_service_client.h"
#include "socket_factory.h"

using namespace Hpcl;

int main() {
    DataComponentFactoryPtr factory
                    = DataComponentFactory::create_factory();
    NameServiceClientPtr data_client
                    = factory->create_name_service_client();
    data_client->connect("localhost", 1027);
    if( data_client->register_service("test",100) ) {
        std::cout<<"Service registered"<<std::endl;
    }
    ServiceInfoPtr info;
    if( data_client->service_info_query("test",info) ) {
        std::cout<<"Service found"<<std::endl;
    }
    if( data_client->unregister_service("test",100) ) {
        std::cout<<"Service unregistered"<<std::endl;
    }
    data_client->disconnect();
    return 0;
}
