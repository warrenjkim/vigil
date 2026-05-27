#pragma once

#include <memory>

#include "pulse/http/handler.h"

namespace vigil {

class AccountsDao;

std::unique_ptr<pulse::http::Handler> MakeHealthHandler();

std::unique_ptr<pulse::http::Handler> MakeGetAccountHandler(
    AccountsDao* accounts_dao);

std::unique_ptr<pulse::http::Handler> MakeInsertAccountHandler(
    AccountsDao* dao);

}  // namespace vigil
