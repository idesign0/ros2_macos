# class_loader

The **class_loader** package is a ROS-independent package for loading plugins during runtime and the foundation of the higher level ROS `pluginlib` library. **class_loader** utilizes the host operating system's runtime loader to open runtime libraries (e.g. .so/.dll/dylib files), introspect the library for exported plugin classes, and allows users to instantiate objects of said exported classes without the explicit declaration (i.e. header file) for those classes.

## class_loader VS pluginlib
`class_loader` is used in the implementation of the higher-level ROS package `pluginlib` which is the encouraged method for loading plugins in the ROS ecosystem. You should use class_loader when creating plugins intended for non-ROS packages and pluginlib when exporting plugins to ROS packages.

## Quality Declaration

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](./QUALITY_DECLARATION.md) for more details.

# Usage

The interface is provided through two classes, `class_loader::ClassLoader` and `class_loader::MultiLibraryClassLoader`. Both provide similar interfaces but the former only binds to a single runtime library whereas the latter can associate with multiple libraries. The typical workflow is as follows:

 - Include `class_loader/class_loader.h` in your source code
 - Instantiate a `class_loader::ClassLoader` object passing the path and name of the library to open
```
class_loader::ClassLoader loader("libMyLibrary.so");
```
 - Query the class for exported classes which have an interface defined by some base class (MyBase in the example)
```
   std::vector<std::string> classes = loader.getAvailableClasses<MyBase>()
```
 - Create/destroy objects of said exported classes
```
 for(unsigned int c = 0; c < classes.size(); ++c)
 {
   boost::shared_ptr<MyBase> plugin = loader.createInstance<MyBase>(classes[c]);
   plugin->someMethod();
   //'plugin' will automatically be deleted when it goes out of scope
 }
```
 - Destroy the ClassLoader object to shutdown the library.

# Example: Basic Workflow for ClassLoader
```
#include <class_loader/class_loader.h>
#include "MyBase.h" //Defines class MyBase

int main()
{
  class_loader::ClassLoader loader("libMyLibrary.so");
  std::vector<std::string> classes = loader.getAvailableClasses<MyBase>();
  for(unsigned int c = 0; c < classes.size(); ++c)
  {
    boost::shared_ptr<MyBase> plugin = loader.createInstance<MyBase>(classes[c]);
    plugin->someMethod();
  }
}
```

Visit the [class_loader](https://wiki.ros.org/class_loader) API documentation for a complete list of its main components.
