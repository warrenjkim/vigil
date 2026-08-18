#pragma once

#include <string>
#include <string_view>

#include "pulse/core/result.h"

namespace vigil {

class ConduitService {
 public:
  explicit ConduitService(std::string_view vigil_base_url,
                          std::string_view conduit_path,
                          std::string_view plaid_config_path,
                          std::string_view vigil_seed_file_path)
      : vigil_base_url_(vigil_base_url),
        conduit_path_(conduit_path),
        plaid_config_path_(plaid_config_path),
        vigil_seed_file_path_(vigil_seed_file_path) {}

  pulse::Result<int> Sync();

 private:
  const std::string vigil_base_url_;
  const std::string conduit_path_;
  const std::string plaid_config_path_;
  const std::string vigil_seed_file_path_;
};

}  // namespace vigil
