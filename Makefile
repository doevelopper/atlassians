.DEFAULT_GOAL:=help
# %W% %G% %U%
#        Makefile
#
#               Copyright (c) 2014-2023 A.H.L
#
#        Permission is hereby granted, free of charge, to any person obtaining
#        a copy of this software and associated documentation files (the
#        "Software"), to deal in the Software without restriction, including
#        without limitation the rights to use, copy, modify, merge, publish,
#        distribute, sublicense, and/or sell copies of the Software, and to
#        permit persons to whom the Software is furnished to do so, subject to
#        the following conditions:
#
#        The above copyright notice and this permission notice shall be
#        included in all copies or substantial portions of the Software.
#
#        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#        EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#        MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#        NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
#        LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#        OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#

# Color output
CYAN := \033[0;36m
GREEN := \033[0;32m
YELLOW := \033[0;33m
RED := \033[0;31m
NC := \033[0m # No Color

GIT_VERSION          ?= $(shell git describe --tags --always)

ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
    KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
    quiet =
    Q =
    QQ =
else
    quiet=quiet_
    Q = @
    QQ = > /dev/null
endif

ifneq ($(findstring s,$(filter-out --%,$(MAKEFLAGS))),)
    quiet=silent_
    tools_silent=s
endif

ifeq (0,${MAKELEVEL})
	whoami    := $(shell whoami)
	host-type := $(shell arch)
	# MAKE := ${MAKE} host-type=${host-type} whoami=${whoami}
endif

MAKEFLAGS += --no-print-directory
MAKEFLAGS += --no-print-directory

DBG_MAKEFILE ?=
ifeq ($(DBG_MAKEFILE),1)
    $(warning ***** starting Makefile for goal(s) "$(MAKECMDGOALS)")
    $(warning ***** $(shell date))
else
    # If we're not debugging the Makefile, don't echo recipes.
    MAKEFLAGS += -s
endif

export EMPTY               =
export SPACE               = $(EMPTY) $(EMPTY)
export MAKEDIR             = mkdir -p
export DOCKER              = docker
export RM                  = -rm -rf
export BIN                 := /usr/bin
export SHELL               = $(BIN)/env bash
#export SHELL = /bin/sh
export RM                  = /opt/bin/cmake -E remove -f
export PRINTF              := $(BIN)/printf
export DF                  := $(BIN)/df
export AWK                 := $(BIN)/awk
export PERL                := $(BIN)/perl
export PYTHON              := $(BIN)/python
export PYTHON3             := $(BIN)/python3
export MSG                 := @bash -c '  $(PRINTF) $(YELLOW); echo "=> $$1";  $(PRINTF) $(NC)'
export UNAME_OS            := $(shell uname -s)
export HOST_RYPE           := $(shell arch)
export DATE                := $(shell date -u "+%b-%d-%Y")
export CWD                 := $(shell pwd -P)

export quiet Q KBUILD_VERBOSE

export BUILD_DIRECTORY ?= $(shell basename $(shell git rev-parse --show-toplevel))-build
export PRJNAME := $(shell basename $(shell git rev-parse --show-toplevel))
export BRANCH := $(shell git rev-parse --abbrev-ref HEAD)
export HASH := $(shell git rev-parse HEAD)
export ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
export GIT_BRANCHES := $(shell git for-each-ref --format='%(refname:short)' refs/heads/ | xargs echo)
export GIT_REMOTES := $(shell git remote | xargs echo )
export SHA1                := $(shell git rev-parse HEAD)
export SHORT_SHA1          := $(shell git rev-parse --short=5 HEAD)
export GIT_STATUS          := $(shell git status --porcelain)
export GIT_BRANCH          := $(shell git rev-parse --abbrev-ref HEAD)
export GIT_BRANCH_STR      := $(shell git rev-parse --abbrev-ref HEAD | tr '/' '_')
export GIT_REPO            := $(shell git config --local remote.origin.url | \
                                sed -e 's/.git//g' -e 's/^.*\.com[:/]//g' | tr '/' '_' 2> /dev/null)

export GIT_ALL_COMMITS     := $(shell git rev-list --all --count)
export GIT_CUR_COMMITS     := $(shell git rev-list --count $(BRANCH))
export GIT_REPOS_URL       := $(shell git config --get remote.origin.url)
export CURRENT_BRANCH      := $(shell git rev-parse --abbrev-ref HEAD)
export GIT_BRANCHES        := $(shell git for-each-ref --format='%(refname:short)' refs/heads/ | xargs echo)
export GIT_REMOTES         := $(shell git remote | xargs echo )
export GIT_ROOTDIR         := $(shell git rev-parse --show-toplevel)
export GIT_DIRTY           := $(shell git diff --shortstat 2> /dev/null | tail -n1 )
export LAST_TAG_COMMIT     := $(shell git rev-list --tags --max-count=1 2>/dev/null)
export LAST_TAG            := $(shell git describe --tags $(LAST_TAG_COMMIT) 2>/dev/null || echo "0.0.0")
export GIT_COMMITS         := $(shell git log --oneline $(LAST_TAG)..HEAD 2>/dev/null | wc -l | tr -d ' ')
export GIT_REVISION        := $(shell git rev-parse --short=8 HEAD 2>/dev/null || echo unknown)
export GIT_COMMIT_VERSION  := $(shell git rev-parse --short=8 HEAD 2>/dev/null || echo unknown)
export GIT_DESC            := $(shell git log --format=%B -n 1 HEAD 2>/dev/null | cat -)
# total number of commits
export BUILD := $(shell git log --oneline | wc -l | sed -e "s/[ \t]*//g")


export HAS_GCC := $(shell which gcc > /dev/null 2> /dev/null && echo true || echo false)
export HAS_CC := $(shell which cc > /dev/null 2> /dev/null && echo true || echo false)
export HAS_CLANG := $(shell which clang > /dev/null 2> /dev/null && echo true || echo false)

# Extract the base branch type (e.g. "feature" from "feature/from-kitchen-sink")
BRANCH_BASE := $(firstword $(subst /, ,$(BRANCH)))

ifeq ($(BRANCH_BASE),master)
    RELEASE_LEVEL := CANDIDATE
else ifeq ($(BRANCH_BASE),main)
    RELEASE_LEVEL := CANDIDATE
else ifeq ($(BRANCH_BASE),develop)
    RELEASE_LEVEL := STABLE
else ifeq ($(BRANCH_BASE),release)
    RELEASE_LEVEL := FINAL
else ifeq ($(BRANCH_BASE),hotfix)
    RELEASE_LEVEL := BETA
else ifeq ($(BRANCH_BASE),feature)
    RELEASE_LEVEL := SNAPSHOT
else ifeq ($(BRANCH_BASE),support)
    RELEASE_LEVEL := SNAPSHOT
else ifeq ($(BRANCH_BASE),bugfix)
    RELEASE_LEVEL := ALPHA
else
    RELEASE_LEVEL := DEV
    $(warning Branch '$(BRANCH)' does not follow gitflow naming; defaulting RELEASE_LEVEL to DEV)
endif

ifeq ($(HAS_CC),true)
    DEFAULT_CC = cc
    DEFAULT_CXX = c++
else
    ifeq ($(HAS_GCC),true)
        DEFAULT_CC = gcc
        DEFAULT_CXX = g++
    else
        ifeq ($(HAS_CLANG),true)
            DEFAULT_CC = clang
            DEFAULT_CXX = clang++
        else
            DEFAULT_CC = no_c_compiler
            DEFAULT_CXX = no_c++_compiler
        endif
    endif
endif

export BAZEL_BIN         := $(shell bazelisk info bazel-bin 2>/dev/null)/external
export BAZEL_OUTPUT_BASE := $(shell bazelisk info output_base 2>/dev/null)
export BAZEL_SERVER_PID  := $(shell bazelisk info server_pid 2>/dev/null)
export BAZEL_TESTLOGS    := $(shell bazelisk info bazel-testlogs 2>/dev/null)
export BAZEL_GENFILES    := $(shell bazelisk info bazel-genfiles 2>/dev/null)
export BAZEL_EXTERNAL    := $(shell bazelisk info output_base 2>/dev/null)/external

export VERSIONFILE     = VERSION_FILE
export SEM_VERSION     := $(shell [ -f $(VERSIONFILE) ] && head $(VERSIONFILE) || echo "0.0.1")
HOST_PLATFORM   =
COMPILER        =
TARGET_PLATFORM =
ARCH            =
# ARCH             ?=$(shell uname -m | sed "s/^i.86$$/i686/")
RTOS            =
LKR             =

ifeq ($(OS),Windows_NT)

	CCFLAGS += -D WIN32
	RTOS = Windows
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        CCFLAGS += -D AMD64
		ARCH = AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CCFLAGS += -D AMD64
			ARCH = AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CCFLAGS += -D IA32
			ARCH = IA32
        endif
    endif
else

    UNAME_S := $(shell sh -c 'uname 2>/dev/null || echo Unknown') #$(shell uname -s)

    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
    endif

    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
    endif

    UNAME_P := $(shell uname -p)

    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif

    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif

    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

ARCH_OS_LINKER := $(ARCH)-$(RTOS)-$(LKR)  #/tmp/bazel_output_root
# TARGET_TRIPLE :=$(shell $DEFAULT_CC -dumpmachine)

export BAZEL_BUILD_ARGS = \
    --define=VERSION=$(RELEASE_LEVEL) --define=GIT_BRANCH=$(CURRENT_BRANCH)  --define=DATE=$(DATE)  --define=HASH=$(HASH) --define=GIT_COMMIT_VERSION=$(GIT_COMMIT_VERSION) \
    --define=BUILD=$(BUILD) --define=GIT_CUR_COMMITS=$(GIT_CUR_COMMITS) --define=GIT_ALL_COMMITS=$(GIT_ALL_COMMITS)

.PHONY: style
style: ## Apply clang-format and clang-tidy naming checks
	@for src in $(SOURCES) ; do \
		echo "Formatting $$src..." ; \
		clang-format -i "$(SRC_DIR)/$$src" ; \
		clang-tidy -checks='-*,readability-identifier-naming' \
		    -config="{CheckOptions: [ \
		    { key: readability-identifier-naming.NamespaceCase, value: lower_case },\
		    { key: readability-identifier-naming.ClassCase, value: CamelCase  },\
		    { key: readability-identifier-naming.StructCase, value: CamelCase  },\
		    { key: readability-identifier-naming.FunctionCase, value: camelBack },\
		    { key: readability-identifier-naming.VariableCase, value: lower_case },\
		    { key: readability-identifier-naming.GlobalConstantCase, value: UPPER_CASE }\
		    ]}" "$(SRC_DIR)/$$src" ; \
	done
	@echo "Done"

.PHONY: check-style
check-style: ## Verify sources conform to coding style
	@for src in $(SOURCES) ; do \
		var=`clang-format "$(SRC_DIR)/$$src" | diff "$(SRC_DIR)/$$src" - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			echo "$$src does not respect the coding style (diff: $$var lines)" ; \
			exit 1 ; \
		fi ; \
	done
	@echo "Style check passed"



.PHONY: format
format:
	@echo "Formating codes"
	@find src/main/cpp/com/github/doevelopper/rules/sdlc  -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format-15 -style=file -i -fallback-style=none {} \;
	@find src/test/cpp/com/github/doevelopper/rules/sdlc  -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format-15 -style=file -i -fallback-style=none {} \;
	@find src/main/cpp  -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format-15 -style=file -i -fallback-style=none {} \;
	@find src/test/cpp  -regex '.*\.\(cpp\|hpp\|cu\|c\|h\)' -exec clang-format-15 -style=file -i -fallback-style=none {} \;
#	C_CHANGED_FILES = $(git diff --cached --name-only --diff-filter=ACM | grep -Ee "\.[ch]$")
#	CXX_CHANGED_FILES = $(git diff --cached --name-only --diff-filter=ACM | grep -Ee "\.([chi](pp|xx)|(cc|hh|ii)|[CHI])$")
# /usr/bin/clang-format -i -style=file ${CXX_CHANGED_FILES}
	@echo "Formating codes done"

.PHONY: tidy
tidy:## Check with clang-tidy
	@for src in $(SOURCES) ; do \
		echo "Running tidy on $$src..." ; \
		clang-tidy -checks="-*,modernize-use-auto,modernize-use-nullptr,\
			readability-else-after-return,readability-simplify-boolean-expr,\
			readability-redundant-member-init,modernize-use-default-member-init,\
			modernize-use-equals-default,modernize-use-equals-delete,\
			modernize-use-using,modernize-loop-convert,\
			cppcoreguidelines-no-malloc,misc-redundant-expression" \
			-header-filter=".*" \
			"$(SRC_DIR)/$$src" ; \
	done
	@echo "Done"

.PHONY: analyzer
analyzer:## Check with clang static analyzer'
	@for src in $(SOURCES) ; do \
		echo "Running analyzer on $$src..." ; \
		clang --analyze -Xanalyzer -analyzer-output=text "$(SRC_DIR)/$$src" ; \
	done
	@echo "Done"

.PHONY: aol
aol:
	@echo "Query possibles AOL"
	@bazelisk query @bazel_tools//src/conditions/...
	@bazelisk query //src/main/cpp/...
	@bazelisk query //src/test/cpp/...
	@bazelisk query //src/it/...

.PHONY: queryall
queryall: ## List all targets
	@bazelisk query @bazel_tools//src/conditions:all
	@bazelisk query @bazel_tools//tools/cpp/...
	@bazelisk query //...
	@bazelisk query //... --output label_kind | sort | column -t

.PHONY: querybin
querybin: ## List binary target
	@bazelisk query 'kind(cc_binary, //...)'

.PHONY: querygrpcdeps
querygrpcdeps: ## for all targets in src find all dependencies (a transitive closure set), then tell me which ones depend on the gtest
	@bazelisk query "rdeps(deps(//src/...), " :gtest")" --output graph | dot -Tpng -O

.PHONY: querybuild
querybuild: ## List buildable targets
	@bazelisk query --keep_going --noshow_progress "kind(.*_binary, rdeps(//..., set(${files[*]})))"

.PHONY: deps
deps:
	@bazelisk build --config linux --config gnu-gcc $(BAZEL_BUILD_ARGS) @net_zlib_zlib//...
	@bazelisk build --config linux --config gnu-gcc $(BAZEL_BUILD_ARGS) @com_google_googletest//...
	@bazelisk build --config linux --config gnu-gcc @org_apache_xerces//:xerces
	@bazelisk build --config linux --config gnu-gcc @com_github_Tencent_rapidjson//:rapidjson
	@bazelisk build --config linux --config gnu-gcc @org_apache_apr//:apr
	@bazelisk build --config linux --config gnu-gcc @org_apache_apr_util//:apr_util
	@bazelisk build --config linux --config gnu-gcc @org_apache_log4cxx//:log4cxx
	@bazelisk build --config linux --config gnu-gcc @org_boost//...


.PHONY: main-compile
main-compile: ## Build all main target rules
	@bazelisk build  //src/main/cpp/...

.PHONY: test-compile
test-compile: ## Build all Test target rules
	@bazelisk build  //src/test/cpp/...

.PHONY: compile
compile: main-compile test-compile ## Build projects main and test sources
	@bazelisk build  //...

.PHONY: test
test: compile ## Build projects sources and run unit tests
	@bazelisk test //src/test/cpp/... --test_output=all

.PHONY: integration-test
integration-test: test ## Build projects sources and run integration/BDD tests
	@bazelisk test //src/it/... --test_output=all

.PHONY: coverage
coverage:  ## Generates code coverage report
	@bazelisk coverage -s --combined_report=lcov --instrumentation_filter=//... --coverage_report_generator=@bazel_tools//tools/test:coverage_report_generator  //...

.PHONY: clean
clean: ## Cleaned up the objects and intermediary files
	@bazelisk clean

.PHONY: expunge
expunge: ## Removes the entire working tree for this bazel instance
	@bazelisk clean --expunge --async

.PHONY: versioninfo
versioninfo: ## Display informations about the image.
	$(Q)echo "Version file: $(VERSIONFILE)"
	$(Q)echo "Current version: $(SEM_VERSION)"
	$(Q)echo "(major: $(MAJOR), minor: $(MINOR), patch: $(PATCH))"
	$(Q)echo "Last tag: $(LAST_TAG)"
	$(Q)echo "Build: $(BUILD) (total number of commits)"
	$(Q)echo "next major version: $(NEXT_MAJOR_VERSION)"
	$(Q)echo "next minor version: $(NEXT_MINOR_VERSION)"
	$(Q)echo "next patch version: $(NEXT_PATCH_VERSION)"
	$(Q)echo "--------------"
	$(Q)echo "Previous version file '$(VERSIONFILE)' commit: $(PREVIOUS_VERSIONFILE_COMMIT)"
	$(Q)echo "Previous version **from** version file: '$(PREVIOUS_VERSION)'"
##@ Help

help: ## Display this help message
# 	@echo "$(CYAN)Build System - Maven-like Goals$(NC)"
# 	@echo ""
# 	@echo "$(GREEN)Usage:$(NC)"
	$(Q)echo "$@ ->"
	$(Q)echo '---------------$(CURDIR)------------------'
	$(Q)echo '+----------------------------------------------------------------------+'
	$(Q)echo '|                        Available Commands                            |'
	$(Q)echo '+----------------------------------------------------------------------+'
	$(Q)echo
	$(Q)awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {sub("\\\\n",sprintf("\n%22c"," "), $$2);printf " \033[36m%-20s\033[0m  %s\n", $$1, $$2}' $(MAKEFILE_LIST)
	$(Q)echo ""
	$(Q)echo "For more details on a specific command, run 'make <command> --help'"