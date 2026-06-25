#include "vigil/handlers/get_new_account_handler.h"

#include "pulse/html/attributes.h"
#include "pulse/html/element.h"
#include "pulse/html/tags.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

pulse::http::Response GetNewAccountHandler::operator()(
    const pulse::http::Request&) const {
  return pulse::http::Response{
      .content_type = "text/html",
      .status = 200,
      .body = pulse::html::Render(pulse::html::Make<pulse::html::Html>(
          pulse::html::Make<pulse::html::Head>(
              pulse::html::Make<pulse::html::Title>("new account")),
          pulse::html::Make<pulse::html::Body>(
              pulse::html::Make<pulse::html::H1>("new account"),
              pulse::html::Make<pulse::html::Form>(
                  pulse::html::Attributes{pulse::html::Action{"/accounts"},
                                          pulse::html::Method{"post"}},
                  pulse::html::Make<pulse::html::Label>(
                      pulse::html::Attributes{pulse::html::For{"name"}},
                      "name"),
                  pulse::html::Make<pulse::html::Input>(pulse::html::Attributes{
                      pulse::html::Type{"text"}, pulse::html::Name{"name"},
                      pulse::html::Placeholder{"account name"}}),
                  pulse::html::Make<pulse::html::Label>(
                      pulse::html::Attributes{pulse::html::For{"type"}},
                      "type"),
                  pulse::html::Make<pulse::html::Select>(
                      pulse::html::Attributes{pulse::html::Name{"type"}},
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{
                              pulse::html::Value{"CHECKING"}},
                          "CHECKING"),
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{
                              pulse::html::Value{"SAVINGS"}},
                          "SAVINGS"),
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{
                              pulse::html::Value{"BROKERAGE"}},
                          "BROKERAGE"),
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{pulse::html::Value{"401K"}},
                          "401K"),
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{
                              pulse::html::Value{"ROTH_IRA"}},
                          "ROTH_IRA"),
                      pulse::html::Make<pulse::html::Option>(
                          pulse::html::Attributes{
                              pulse::html::Value{"TRADITIONAL_IRA"}},
                          "TRADITIONAL_IRA")),
                  pulse::html::Make<pulse::html::Button>(
                      pulse::html::Attributes{pulse::html::Type{"submit"}},
                      "create")))))};
}

}  // namespace vigil
