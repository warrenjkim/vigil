#include "vigil/handlers/pages/get_new_account_handler.h"

#include "pulse/html/attributes.h"
#include "pulse/html/element.h"
#include "pulse/html/tags.h"
#include "pulse/http/request.h"
#include "pulse/http/response.h"

namespace vigil {

using pulse::html::Action;
using pulse::html::Attributes;
using pulse::html::Body;
using pulse::html::Button;
using pulse::html::For;
using pulse::html::Form;
using pulse::html::H1;
using pulse::html::Head;
using pulse::html::Html;
using pulse::html::Input;
using pulse::html::Label;
using pulse::html::Make;
using pulse::html::Method;
using pulse::html::Name;
using pulse::html::Option;
using pulse::html::Placeholder;
using pulse::html::Render;
using pulse::html::Select;
using pulse::html::Style;
using pulse::html::Title;
using pulse::html::Type;
using pulse::html::Value;

pulse::http::Response GetNewAccountHandler::operator()(
    const pulse::http::Request&) const {
  return pulse::http::Response{
      .content_type = "text/html",
      .status = 200,
      .body = Render(Make<Html>(
          Make<Head>(Make<Title>("new account"), Make<Style>(R"css(
                * {
                  box-sizing: border-box;
                  margin: 0;
                  padding: 0;
                  font-family: "Computer Modern Sans", sans-serif;
                }
                body {
                  font-size: 18px;
                  line-height: 1.5;
                  color: #dddddd;
                  background-color: #151515;
                  padding: 4rem 6rem;
                }
                h1 {
                  font-size: 28px;
                  text-transform: uppercase;
                  font-weight: lighter;
                  color: #dddddd;
                  margin-bottom: 2rem;
                }
                label {
                  font-size: 14px;
                  text-transform: uppercase;
                  letter-spacing: 0.08em;
                  color: #a89984;
                  display: block;
                  margin-bottom: 0.25rem;
                  margin-top: 0.75rem;
                }
                input,
                select {
                  background: #1d2021;
                  border: 1px solid #3c3836;
                  color: #dddddd;
                  padding: 0.3rem 0.6rem;
                  font-size: 0.9rem;
                  display: block;
                  width: 280px;
                }
                input:focus,
                select:focus {
                  outline: none;
                  border-color: #668373;
                }
                button {
                  margin-top: 1.5rem;
                  background: #1d2021;
                  border: 1px solid #98971a;
                  color: #98971a;
                  padding: 0.3rem 0.8rem;
                  font-size: 0.85rem;
                  cursor: pointer;
                }
                button:hover {
                  color: #b8bb26;
                  border-color: #b8bb26;
                })css")),
          Make<Body>(
              Make<H1>("new account"),
              Make<Form>(
                  Attributes{Action{"/accounts"}, Method{"post"}},
                  Make<Label>(Attributes{For{"name"}}, "name"),
                  Make<Input>(Attributes{Type{"text"}, Name{"name"},
                                         Placeholder{"account name"}}),
                  Make<Label>(Attributes{For{"type"}}, "type"),
                  Make<Select>(
                      Attributes{Name{"type"}},
                      Make<Option>(Attributes{Value{"CHECKING"}}, "CHECKING"),
                      Make<Option>(Attributes{Value{"SAVINGS"}}, "SAVINGS"),
                      Make<Option>(Attributes{Value{"BROKERAGE"}}, "BROKERAGE"),
                      Make<Option>(Attributes{Value{"401K"}}, "401K"),
                      Make<Option>(Attributes{Value{"ROTH_IRA"}}, "ROTH_IRA"),
                      Make<Option>(Attributes{Value{"TRADITIONAL_IRA"}},
                                   "TRADITIONAL_IRA")),
                  Make<Button>(Attributes{Type{"submit"}}, "create")))))};
}

}  // namespace vigil
