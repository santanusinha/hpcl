#include <iostream>
#include <map>
#include <thread>

#include <boost/make_shared.hpp>

#include "data_server.h"
#include "service_info.h"
#include "socket_factory.h"

using namespace Hpcl;

class ServiceRegistry : public DataServer {
    public:
        typedef std::map<std::string, ServiceInfoPtr> Services;

        ServiceRegistry()
            :DataServer( SocketFactoryPtr( SocketFactory::create_factory() ) ),
            m_services() {
        }

        ~ServiceRegistry() {
        }

        virtual bool
        on_service_register_request( const std::string &in_service_name,
                   const std::string &in_hostname, int32_t in_port ) {
            std::lock_guard<std::mutex> l(m_mutex);
            auto it = m_services.find( in_service_name );
            ServiceInfoPtr service;
            if( it != m_services.end() ) {
                service = (*it).second;
            }
            else {
                service = ServiceInfoPtr(
                            boost::make_shared<ServiceInfo>());
                service->set_service_name( in_service_name );
                auto status = m_services.insert(
                                std::make_pair( in_service_name, service ) );
                if( false == status.second ) {
                    return false;
                }
            }
            HostInfoPtr host_info( boost::make_shared<HostInfo>() );
            host_info->set_hostname( in_service_name );
            host_info->set_port( in_port );
            service->add_host_info( host_info );
            std::cout<<"Registered service "<< in_service_name
                        <<" at "<<in_hostname<<":"<<in_port<<std::endl;
            return true;
        }

        virtual bool
        on_service_unregister_request( const std::string &in_service_name,
                           const std::string &in_hostname, int32_t in_port ) {
            std::lock_guard<std::mutex> l(m_mutex);
            auto it = m_services.find( in_service_name );
            ServiceInfoPtr service;
            if( it != m_services.end() ) {
                service = (*it).second;
            }
            else {
                return false;
            }
            HostInfoPtr host_info( boost::make_shared<HostInfo>() );
            host_info->set_hostname( in_hostname );
            host_info->set_port( in_port );
            service->erase_host_info( host_info );
            if( !service->has_host_info() ) {
                m_services.erase( in_service_name );
            }
            std::cout<<"Unregistered service "<< in_service_name
                        <<" at "<<in_hostname<<":"<<in_port<<std::endl;
            return true;
        }

        virtual bool
        on_service_info_request( const std::string &in_service_name,
                               ServiceInfoPtr &out_service_info ) {
            std::lock_guard<std::mutex> l(m_mutex);
            auto it = m_services.find( in_service_name );
            if( it != m_services.end() ) {
                out_service_info = (*it).second;
            }
            else {
                return false;
            }
            std::cout<<"Found service "<< in_service_name<<std::endl;
            return true;
        }

    private:
        Services m_services;
        std::mutex m_mutex;
};

int main() {
    ServiceRegistry registry;
    std::exception_ptr error;
    registry.listen( 1027, error );
    return 0;
}

