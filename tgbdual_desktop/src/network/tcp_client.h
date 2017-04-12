#pragma once

#include <string>

#include <link_cable_source.h>
#include "tcp_link_cable.h"

tcp_link_cable *create_client_cable(const std::string &address);