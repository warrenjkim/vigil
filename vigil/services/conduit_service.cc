#include "vigil/services/conduit_service.h"

#include <cstdint>
#include <string>
#include <vector>

#include "pulse/core/error.h"
#include "pulse/core/log.h"
#include "pulse/core/result.h"
#include "pulse/json/parse.h"
#include "pulse/json/value.h"
#include "pulse/process/output.h"
#include "pulse/process/run.h"
#include "pulse/strings/cat.h"

namespace vigil {

pulse::Result<int> ConduitService::Sync() {
  if (conduit_path_.empty()) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "could not resolve conduit path"};
  }

  pulse::Log() << "ConduitService: running sync (bin=" << conduit_path_ << ")";

  pulse::Result<pulse::process::Output> synced = pulse::process::Run(
      conduit_path_,
      {"--vigil_mode",
       pulse::strings::Cat("--vigil_base_url=", vigil_base_url_),
       pulse::strings::Cat("--plaid_config=", plaid_config_path_),
       pulse::strings::Cat("--vigil_seed_file=", vigil_seed_file_path_)});
  if (!synced.ok()) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "failed to spawn conduit"};
  }

  if (!synced->Ok()) {
    pulse::Log() << "ConduitService: conduit exited abnormally: "
                 << synced->StdErr();
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "conduit exited with non-zero status"};
  }

  pulse::Result<pulse::json::Value> parsed =
      pulse::json::Parse(synced->StdOut());
  if (!parsed.ok()) {
    pulse::Log() << "ConduitService: parsing output: "
                 << parsed.error().message;
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "cannot parse conduit output"};
  }

  if (!parsed->is<pulse::json::Object>() ||
      !(*parsed)["imported"].is<int64_t>()) {
    return pulse::Error{.code = pulse::Error::Code::kInternal,
                        .message = "conduit output missing 'imported'"};
  }

  const int imported = static_cast<int>((*parsed)["imported"].as<int64_t>());
  pulse::Log() << "ConduitService: sync complete (imported=" << imported << ")";

  return imported;
}

}  // namespace vigil
