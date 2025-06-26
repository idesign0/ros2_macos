# launch_param_builder

Python library for loading parameters in launch files

## Testing and Linting

To test the packages, use the following command with colcon.

```bash
export TEST_PACKAGES="PROJECT_PACKAGE_NAMES"
colcon build --packages-select launch_param_builder
colcon test --packages-select launch_param_builder --event-handlers console_direct+
colcon test-result
```

To add a copyright for a new file

```bash
ament_copyright --add-missing PickNik bsd_3clause .
```
