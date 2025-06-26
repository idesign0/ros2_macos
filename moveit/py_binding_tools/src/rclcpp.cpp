/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2020, Bielefeld University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Bielefeld University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <py_binding_tools/initializer.h>
#include <rclcpp/node.hpp>
#include <rclcpp/node_options.hpp>

namespace py = pybind11;
using namespace py_binding_tools;

namespace py_binding_tools
{
void add_node(const rclcpp::Node::SharedPtr& node);
}

namespace
{
template <typename Class>
auto InitFromKwargs()
{
  return py::init([](py::kwargs kwargs) {
    Class obj{};

    // set all passed kwargs as attributes
    py::object py_obj = py::cast(&obj, py::return_value_policy::reference);
    for (auto it : kwargs)
      py::setattr(py_obj, it.first, it.second);

    return obj;
  });
}
}  // namespace

PYBIND11_MODULE(rclcpp, m)
{
  m.doc() = "C++ Python binding tools";

  m.def(
      "init",
      [](py::object args) {
        if (args.is_none())  // use sys.argv if no args were passed
        {
          py::module sys = py::module::import("sys");
          args = sys.attr("argv");
        }
        return init(args.cast<std::vector<std::string>>());
      },
      "Initialize rclcpp", py::arg("args") = py::none());
  m.def("shutdown", &shutdown, "Shutdown rclcpp");

  using Class = rclcpp::NodeOptions;
  py::class_<Class>(m, "NodeOptions")
      .def(InitFromKwargs<Class>())
      .def_property(
          "arguments", [](const Class& self) { return self.arguments(); },
          [](Class* self, const std::vector<std::string>& value) { self->arguments(value); })
      .def_property(
          "use_global_arguments", [](const Class& self) { return self.use_global_arguments(); },
          [](Class* self, bool value) { self->use_global_arguments(value); })
      .def_property(
          "enable_rosout", [](const Class& self) { return self.enable_rosout(); },
          [](Class* self, bool value) { self->enable_rosout(value); })
      .def_property(
          "use_intra_process_comms", [](const Class& self) { return self.use_intra_process_comms(); },
          [](Class* self, bool value) { self->use_intra_process_comms(value); })
      .def_property(
          "start_parameter_event_publisher", [](const Class& self) { return self.start_parameter_event_publisher(); },
          [](Class* self, bool value) { self->start_parameter_event_publisher(value); })
      .def_property(
          "use_clock_thread", [](const Class& self) { return self.use_clock_thread(); },
          [](Class* self, bool value) { self->use_clock_thread(value); })
      .def_property(
          "allow_undeclared_parameters", [](const Class& self) { return self.allow_undeclared_parameters(); },
          [](Class* self, bool value) { self->allow_undeclared_parameters(value); })
      .def_property(
          "automatically_declare_parameters_from_overrides",
          [](const Class& self) { return self.automatically_declare_parameters_from_overrides(); },
          [](Class* self, bool value) { self->automatically_declare_parameters_from_overrides(value); });

  py::class_<rclcpp::Node, rclcpp::Node::SharedPtr>(m, "Node")
      .def(py::init([](const std::string& name, const rclcpp::NodeOptions& opts) {
             auto node = std::make_shared<rclcpp::Node>(name, opts);
             add_node(node);
             return node;
           }),
           py::arg("name"), py::arg("options") = rclcpp::NodeOptions{})
      .def_property_readonly("name", &rclcpp::Node::get_name);
}
