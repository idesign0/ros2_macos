// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from @(package_name):@(interface_path)
// generated code does not contain a copyright notice
@
@#######################################################################
@# EmPy template for generating _<idl>_s.c files
@#
@# Context:
@#  - package_name (string)
@#  - interface_path (Path relative to the directory named after the package)
@#  - content (IdlContent, list of elements, e.g. Messages or Services)
@#######################################################################
@
@#######################################################################
@# Handle messages
@#######################################################################
@{
from rosidl_parser.definition import Message

include_directives = set()
}@
@[for message in content.get_elements_of_type(Message)]@
@{

TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=message, include_directives=include_directives)
}@
@[end for]@
@
@#######################################################################
@# Handle services
@#######################################################################
@{
from rosidl_parser.definition import Service
}@
@[for service in content.get_elements_of_type(Service)]@
@{

TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=service.request_message, include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=service.response_message, include_directives=include_directives)
}@
@[end for]@
@
@#######################################################################
@# Handle actions
@#######################################################################
@{
from rosidl_parser.definition import Action
}@
@[for action in content.get_elements_of_type(Action)]@
@{

TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.goal, include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.result, include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback, include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.send_goal_service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.send_goal_service.response_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.get_result_service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.get_result_service.response_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    '_msg_support.c.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback_message, include_directives=include_directives)
}@
@[end for]@
