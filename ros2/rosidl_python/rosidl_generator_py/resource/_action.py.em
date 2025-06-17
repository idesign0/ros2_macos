@# Included from rosidl_generator_py/resource/_idl.py.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore

action_name = '_' + convert_camel_case_to_lower_case_underscore(action.namespaced_type.name)
module_name = '_' + convert_camel_case_to_lower_case_underscore(interface_path.stem)

TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=action.goal, import_statements=import_statements)
TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=action.result, import_statements=import_statements)
TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback, import_statements=import_statements)
TEMPLATE(
    '_srv.py.em',
    package_name=package_name, interface_path=interface_path,
    service=action.send_goal_service, import_statements=import_statements)
TEMPLATE(
    '_srv.py.em',
    package_name=package_name, interface_path=interface_path,
    service=action.get_result_service, import_statements=import_statements)
TEMPLATE(
    '_msg.py.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback_message, import_statements=import_statements)
}@


class Metaclass_@(action.namespaced_type.name)(type):
    """Metaclass of action '@(action.namespaced_type.name)'."""

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
                '@('.'.join(action.namespaced_type.namespaced_name()))')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._TYPE_SUPPORT = module.type_support_action__@('__'.join(action.namespaced_type.namespaces[1:]))_@(action_name)

            from action_msgs.msg import _goal_status_array
            if _goal_status_array.Metaclass_GoalStatusArray._TYPE_SUPPORT is None:
                _goal_status_array.Metaclass_GoalStatusArray.__import_type_support__()
            from action_msgs.srv import _cancel_goal
            if _cancel_goal.Metaclass_CancelGoal._TYPE_SUPPORT is None:
                _cancel_goal.Metaclass_CancelGoal.__import_type_support__()

            from @('.'.join(action.namespaced_type.namespaces)) import @(module_name)
            if @(module_name).Metaclass_@(action.send_goal_service.namespaced_type.name)._TYPE_SUPPORT is None:
                @(module_name).Metaclass_@(action.send_goal_service.namespaced_type.name).__import_type_support__()
            if @(module_name).Metaclass_@(action.get_result_service.namespaced_type.name)._TYPE_SUPPORT is None:
                @(module_name).Metaclass_@(action.get_result_service.namespaced_type.name).__import_type_support__()
            if @(module_name).Metaclass_@(action.feedback_message.structure.namespaced_type.name)._TYPE_SUPPORT is None:
                @(module_name).Metaclass_@(action.feedback_message.structure.namespaced_type.name).__import_type_support__()


class @(action.namespaced_type.name)(metaclass=Metaclass_@(action.namespaced_type.name)):

    # The goal message defined in the action definition.
    from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.goal.structure.namespaced_type.name) as Goal
    # The result message defined in the action definition.
    from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.result.structure.namespaced_type.name) as Result
    # The feedback message defined in the action definition.
    from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.feedback.structure.namespaced_type.name) as Feedback

    class Impl:

        # The send_goal service using a wrapped version of the goal message as a request.
        from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.send_goal_service.namespaced_type.name) as SendGoalService
        # The get_result service using a wrapped version of the result message as a response.
        from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.get_result_service.namespaced_type.name) as GetResultService
        # The feedback message with generic fields which wraps the feedback message.
        from @('.'.join(action.namespaced_type.namespaces)).@(module_name) import @(action.feedback_message.structure.namespaced_type.name) as FeedbackMessage

        # The generic service to cancel a goal.
        from action_msgs.srv._cancel_goal import CancelGoal as CancelGoalService
        # The generic message for get the status of a goal.
        from action_msgs.msg._goal_status_array import GoalStatusArray as GoalStatusMessage

    def __init__(self):
        raise NotImplementedError('Action classes can not be instantiated')
