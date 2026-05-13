#pragma once

#include <memory>

#include "pulse/http/handler.h"

namespace vigil {

std::unique_ptr<pulse::http::Handler> MakeHealthHandler();

}
