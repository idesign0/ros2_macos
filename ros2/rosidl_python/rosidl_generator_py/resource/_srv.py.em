@# Included from rosidl_generator_py/resource/_idl.py.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore

service_name = '_' + convert_camel_case_to_lower_case_underscore(service.namespaced_type.name)
module_name = '_' + convert_camel_case_to_lower_case_underscore(interface_path.stem)

TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=service.request_message, import_statements=import_statements)
TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=service.response_message, import_statements=import_statements)
}@


class Metaclass_@(service.namespaced_type.name)(type):
    """Metaclass of service '@(service.namespaced_type.name)'."""

    _TYPE_SUPPORT = None

    @@classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('@(package_name)')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                '@('.'.join(service.namespaced_type.namespaced_name()))')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._TYPE_SUPPORT = module.type_support_srv__@('__'.join(service.namespaced_type.namespaces[1:]))_@(service_name)

            from @('.'.join(service.namespaced_type.namespaces)) import @(module_name)
            if @(module_name).Metaclass_@(service.request_message.structure.namespaced_type.name)._TYPE_SUPPORT is None:
                @(module_name).Metaclass_@(service.request_message.structure.namespaced_type.name).__import_type_support__()
            if @(module_name).Metaclass_@(service.response_message.structure.namespaced_type.name)._TYPE_SUPPORT is None:
                @(module_name).Metaclass_@(service.response_message.structure.namespaced_type.name).__import_type_support__()


class @(service.namespaced_type.name)(metaclass=Metaclass_@(service.namespaced_type.name)):
    from @('.'.join(service.namespaced_type.namespaces)).@(module_name) import @(service.request_message.structure.namespaced_type.name) as Request
    from @('.'.join(service.namespaced_type.namespaces)).@(module_name) import @(service.response_message.structure.namespaced_type.name) as Response

    def __init__(self):
        raise NotImplementedError('Service classes can not be instantiated')
