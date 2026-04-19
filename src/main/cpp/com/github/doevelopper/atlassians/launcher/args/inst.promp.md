Kindly leverage OOP C++ 20, best software practices, usefull design patterns, SOLID patterns,  and implement a framework that leverage all types Command-Line Argument from boost program_options framework.

A started adding some files and implementation draft below. I am not quite sure of its content
```
src/main/cpp/com/github/doevelopper/atlassians/launcher/args
├── AbstractConfiguration.cpp
├── AbstractConfiguration.hpp
├── AbstractOption.cpp
├── AbstractOption.hpp
├── BooleanOption.cpp
├── BooleanOption.hpp
├── CommandLineParser.cpp
├── CommandLineParser.hpp
├── ConfigurationFileOption.cpp
├── ConfigurationFileOption.hpp
├── DefaultConfiguration.cpp
├── DefaultConfiguration.hpp
├── EnvironmentOption.cpp
├── EnvironmentOption.hpp
├── Grouped.cpp
├── Grouped.hpp
├── HiddenOption.cpp
├── HiddenOption.hpp
├── IConfiguration.cpp
├── IConfiguration.hpp
├── ImplicitOption.cpp
├── ImplicitOption.hpp
├── IOption.cpp
├── IOption.hpp
├── MultiValueOption.cpp
├── MultiValueOption.hpp
├── NamedOption.cpp
├── NamedOption.hpp
├── Optional.cpp
├── Optional.hpp
├── PositionalOption.cpp
├── PositionalOption.hpp
├── ProgramOption.cpp
├── ProgramOption.hpp
├── Required.cpp
└── Required.hpp
```

Unit testion shoul goe to this folder
```
src/test/cpp/com/github/doevelopper/atlassians/launcher/args
```

Some reference files that can be usefull for styleguide exist at:
- `src/main/cpp/com/github/doevelopper/atlassians/demo/Calculator.{cpp,hpp}`
- `src/test/cpp/com/github/doevelopper/atlassians/demo/CalculatorTest.{cpp,hpp}`
- Logging: `#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>`



I am usin g this command for cimpilation
```
cmake --build target --target all
```

sometime regeneration of configurations may be need. I am using this command in that case
```
cmake -E chdir target cmake .. -DCMAKE_INSTALL_PREFIX=install
cmake --build target --target all
```
