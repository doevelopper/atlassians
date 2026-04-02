bazel query @org_apache_apr_util//... 2>&1 | head -30
bazel query 'deps(//src/main/cpp:com.github.doevelopper.atlassians.main)' --output=package 2>&1 | grep -E 'apr|expat' | sort -u


This repository is leveraging bazel as build tools.
I want this goal **name = "com.github.doevelopper.atlassians.main",** to be linked agains
    **buildsys/bazel/off_the_shelf_software/apr**.
    **buildsys/bazel/off_the_shelf_software/apr_util**
    **buildsys/bazel/off_the_shelf_software/log4cxx**

    code structure in ****buildsys/bazel/off_the_shelf_software/** came from another project.

Help me integrate it in this code source.
Note that **Expat** `bazel_dep(name = "libexpat", version = "2.7.1.bcr.1")`   library is already added to **MODULE.bazel**

and there is also **buildsys/bazel/off_the_shelf_software/expat**. Dont know with Expat to use as **apr_util**'s dependency

bazel query @org_apache_log4cxx//... 2>&1 | head -30
bazel build @org_apache_log4cxx//:log4cxx
bazel build @org_apache_apr_util//:apr_util

bazel_dep(name = "libexpat", version = "2.7.1.bcr.1")



bazel query @cucumber-cpp//... 2>&1 | head -30
