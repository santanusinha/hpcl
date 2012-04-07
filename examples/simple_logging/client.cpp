#include <iostream>

#include "logging_component_factory.h"
#include "remote_logger.h"

using namespace std;
using namespace Hpcl;

int main() {
    RemoteLoggerPtr logger = LoggingComponentFactory::create_factory()->create_client();
    logger->connect("127.0.0.1", 1028);
    logger->log("Hello World\n");
    logger->disconnect();
    return 0;
}
