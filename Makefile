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

##@ Static Analysis & Quality

CC_SOURCES := $(shell find src/main/cpp src/test/cpp src/it \
	-regex '.*\.\(c\|cc\|cpp\|cxx\|h\|hh\|hpp\|hxx\)' 2>/dev/null)
CC_MAIN_SOURCES := $(shell find src/main/cpp \
	-regex '.*\.\(c\|cc\|cpp\|cxx\|h\|hh\|hpp\|hxx\)' 2>/dev/null)

.PHONY: clang-format-check
clang-format-check: ## Check C/C++ formatting (dry-run, no changes)
	@echo "$(CYAN)>>> clang-format check...$(NC)"
	@if command -v clang-format >/dev/null 2>&1 && [ -n "$(CC_SOURCES)" ]; then \
		clang-format --dry-run --Werror $(CC_SOURCES); \
		echo "$(GREEN)>>> clang-format passed.$(NC)"; \
	else \
		echo "$(YELLOW)>>> clang-format not found or no sources — skipped.$(NC)"; \
	fi

.PHONY: clang-format-fix
clang-format-fix: ## Auto-fix C/C++ formatting in-place
	@echo "$(CYAN)>>> clang-format fix...$(NC)"
	@if command -v clang-format >/dev/null 2>&1 && [ -n "$(CC_SOURCES)" ]; then \
		clang-format -i $(CC_SOURCES); \
		echo "$(GREEN)>>> clang-format applied.$(NC)"; \
	fi

.PHONY: cpplint
cpplint: ## Run cpplint style checks
	@echo "$(CYAN)>>> cpplint...$(NC)"
	@if command -v cpplint >/dev/null 2>&1 && [ -n "$(CC_SOURCES)" ]; then \
		cpplint --quiet --filter=-legal/copyright,-build/include_subdir $(CC_SOURCES); \
		echo "$(GREEN)>>> cpplint passed.$(NC)"; \
	else \
		echo "$(YELLOW)>>> cpplint not found — skipped.$(NC)"; \
	fi

.PHONY: cppcheck
cppcheck: ## Run cppcheck static analysis
	@echo "$(CYAN)>>> cppcheck...$(NC)"
	@if command -v cppcheck >/dev/null 2>&1 && [ -n "$(CC_SOURCES)" ]; then \
		cppcheck --error-exitcode=1 --enable=warning,style,performance,portability \
			--std=c++20 --suppress=missingIncludeSystem $(CC_SOURCES); \
		echo "$(GREEN)>>> cppcheck passed.$(NC)"; \
	else \
		echo "$(YELLOW)>>> cppcheck not found — skipped.$(NC)"; \
	fi

.PHONY: clang-tidy
clang-tidy: ## Run clang-tidy static analysis (main sources only)
	@echo "$(CYAN)>>> clang-tidy...$(NC)"
	@if command -v clang-tidy >/dev/null 2>&1 && [ -n "$(CC_MAIN_SOURCES)" ]; then \
		clang-tidy -p . $(CC_MAIN_SOURCES) 2>&1 | head -100 || true; \
		echo "$(GREEN)>>> clang-tidy completed.$(NC)"; \
	else \
		echo "$(YELLOW)>>> clang-tidy not found — skipped.$(NC)"; \
	fi

.PHONY: iwyu
iwyu: ## Run include-what-you-use analysis (advisory)
	@echo "$(CYAN)>>> include-what-you-use...$(NC)"
	@if command -v iwyu >/dev/null 2>&1 || command -v include-what-you-use >/dev/null 2>&1; then \
		IWYU=$$(command -v iwyu || command -v include-what-you-use); \
		for src in $(CC_MAIN_SOURCES); do \
			case "$$src" in *.cpp|*.cc|*.c|*.cxx) \
				$$IWYU -std=c++20 -I src/main/cpp "$$src" 2>&1 || true ;; \
			esac; \
		done | head -50; \
		echo "$(GREEN)>>> iwyu completed.$(NC)"; \
	else \
		echo "$(YELLOW)>>> iwyu not found — skipped.$(NC)"; \
	fi

.PHONY: static-analysis
static-analysis: clang-format-check cpplint cppcheck clang-tidy ## Run all static analysis checks (format, cpplint, cppcheck, clang-tidy)
	@echo "$(GREEN)>>> All static analysis checks completed.$(NC)"

.PHONY: quality
quality: static-analysis compile test coverage ## Full quality gate: static analysis + build + test + coverage
	@echo "$(GREEN)>>> Full quality gate passed.$(NC)"

##@ Artifact Packaging

.PHONY: source-package
source-package: ## Create versioned source archive (clean, no build artifacts)
	@echo "$(CYAN)>>> Creating source package...$(NC)"
	@mkdir -p "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)"
	@git archive --format=tar.gz --prefix="atlassians-$(SEM_VERSION)/" \
		-o "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/atlassians-$(SEM_VERSION)-source.tar.gz" \
		HEAD
	@echo "$(GREEN)>>> Source package: $(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/atlassians-$(SEM_VERSION)-source.tar.gz$(NC)"

.PHONY: production-package
production-package: compile ## Create production binaries package (optimized release binaries)
	@echo "$(CYAN)>>> Creating production binaries package...$(NC)"
	@bazelisk build //src/main/cpp:com.github.doevelopper.atlassians.main.package
	@mkdir -p "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)"
	@cp -f "$$(bazelisk info bazel-bin)/src/main/cpp/com.github.doevelopper.atlassians.main.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/atlassians-$(SEM_VERSION)-binaries.tar.gz"
	@echo "$(GREEN)>>> Production package created.$(NC)"

.PHONY: quality-package
quality-package: test coverage ## Create quality/assessment package (test binaries, reports, coverage)
	@echo "$(CYAN)>>> Creating quality assessment package...$(NC)"
	@mkdir -p "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality"
	@# Test binaries
	@cp -f "$$(bazelisk info bazel-bin)/src/test/cpp/com.github.doevelopper.atlassians.test.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/" 2>/dev/null || true
	@cp -f "$$(bazelisk info bazel-bin)/src/it/com.github.doevelopper.atlassians.it.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/" 2>/dev/null || true
	@# Test reports
	@find $(TESTLOGS_DIR) -name "test.xml" -exec cp --parents {} \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/" \; 2>/dev/null || true
	@find $(TESTLOGS_DIR) -name "test.log" -exec cp --parents {} \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/" \; 2>/dev/null || true
	@# Coverage
	@cp -f "$(BAZEL_OUTPUT_PATH)/_coverage/_baseline_report.dat" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/combined-coverage.lcov" 2>/dev/null || true
	@# Static analysis reports (if they exist)
	@if [ -f "$(EVIDENCE_DIR)/static-analysis-report.txt" ]; then \
		cp -f "$(EVIDENCE_DIR)/static-analysis-report.txt" \
			"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/"; \
	fi
	@# Bundle into a single archive
	@cd "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)" && \
		tar czf "atlassians-$(SEM_VERSION)-quality.tar.gz" quality/ 2>/dev/null || true
	@rm -rf "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/quality/"
	@echo "$(GREEN)>>> Quality package: $(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/atlassians-$(SEM_VERSION)-quality.tar.gz$(NC)"

.PHONY: all-packages
all-packages: source-package production-package quality-package ## Build all three release packages (source, binaries, quality)
	@echo "$(GREEN)>>> All packages created in $(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)$(NC)"
	@ls -lh "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/"*.tar.gz

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
	@bazelisk build  $(BAZEL_BUILD_ARGS) @net_zlib_zlib//...
	@bazelisk build  $(BAZEL_BUILD_ARGS) @com_google_googletest//...
	@bazelisk build  @org_apache_xerces//:xerces
	@bazelisk build  @com_github_Tencent_rapidjson//:rapidjson
	@bazelisk build  @org_apache_apr//:apr
	@bazelisk build  @org_apache_apr_util//:apr_util
	@bazelisk build  @org_apache_log4cxx//:log4cxx
	@bazelisk build  @org_boost//...


.PHONY: main-compile
main-compile: ## Build all main target rules
	@bazelisk build  //src/main/cpp/...

.PHONY: test-compile
test-compile: ## Build all Test target rules
	@bazelisk build  //src/test/cpp/...

##@ Build & Compile

.PHONY: compile
compile: main-compile test-compile ## Build projects main and test sources
	@bazelisk build //src/...

##@ Testing

.PHONY: test
test: compile ## Build projects sources and run unit tests
	@echo "$(CYAN)>>> Running unit tests...$(NC)"
	@bazelisk test //src/test/cpp/... --test_output=all
	@echo "$(GREEN)>>> Unit tests completed.$(NC)"

.PHONY: integration-test
integration-test: test ## Build projects sources and run integration/BDD tests
	@echo "$(CYAN)>>> Running BDD/integration tests...$(NC)"
	@bazelisk test //src/it/... --test_output=all
	@echo "$(GREEN)>>> BDD/integration tests completed.$(NC)"

.PHONY: coverage
coverage: ## Generates code coverage report (lcov)
	@echo "$(CYAN)>>> Generating code coverage report...$(NC)"
	@bazelisk coverage -s --keep_going \
		--combined_report=lcov \
		--instrumentation_filter="//src/main/cpp[/:]" \
		--coverage_report_generator=@bazel_tools//tools/test:coverage_report_generator \
		//src/test/cpp/... || true
	@echo "$(GREEN)>>> Coverage report generated.$(NC)"

##@ Evidence & QMS Report Collection

# Evidence output directories
EVIDENCE_DIR            := $(ROOT_DIR)/src/site/compliance/evidence
EVIDENCE_UT_DIR         := $(EVIDENCE_DIR)/unit-tests
EVIDENCE_BDD_DIR        := $(EVIDENCE_DIR)/bdd-tests
EVIDENCE_COVERAGE_DIR   := $(EVIDENCE_DIR)/coverage
EVIDENCE_PACKAGES_DIR   := $(EVIDENCE_DIR)/packages
EVIDENCE_BUILD_DIR      := $(EVIDENCE_DIR)/build-logs
TESTLOGS_DIR            := $(shell bazelisk info bazel-testlogs 2>/dev/null)
BAZEL_OUTPUT_PATH       := $(shell bazelisk info output_path 2>/dev/null)

.PHONY: collect-unit-test-reports
collect-unit-test-reports: test ## Collect unit test XML reports and logs as QMS evidence
	@echo "$(CYAN)>>> Collecting unit test reports...$(NC)"
	@mkdir -p "$(EVIDENCE_UT_DIR)/$(DATE)-$(SHORT_SHA1)"
	@for t in $(TESTLOGS_DIR)/src/test/cpp/*/; do \
		test_name=$$(basename "$$t"); \
		mkdir -p "$(EVIDENCE_UT_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name"; \
		cp -f "$$t/test.xml" "$(EVIDENCE_UT_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name/" 2>/dev/null || true; \
		cp -f "$$t/test.log" "$(EVIDENCE_UT_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name/" 2>/dev/null || true; \
	done
	@echo "$(GREEN)>>> Unit test reports saved to $(EVIDENCE_UT_DIR)/$(DATE)-$(SHORT_SHA1)$(NC)"

.PHONY: collect-bdd-test-reports
collect-bdd-test-reports: integration-test ## Collect BDD test reports and logs as QMS evidence
	@echo "$(CYAN)>>> Collecting BDD test reports...$(NC)"
	@mkdir -p "$(EVIDENCE_BDD_DIR)/$(DATE)-$(SHORT_SHA1)"
	@for t in $(TESTLOGS_DIR)/src/it/*/; do \
		test_name=$$(basename "$$t"); \
		mkdir -p "$(EVIDENCE_BDD_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name"; \
		cp -f "$$t/test.xml" "$(EVIDENCE_BDD_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name/" 2>/dev/null || true; \
		cp -f "$$t/test.log" "$(EVIDENCE_BDD_DIR)/$(DATE)-$(SHORT_SHA1)/$$test_name/" 2>/dev/null || true; \
	done
	@echo "$(GREEN)>>> BDD test reports saved to $(EVIDENCE_BDD_DIR)/$(DATE)-$(SHORT_SHA1)$(NC)"

.PHONY: collect-coverage-report
collect-coverage-report: coverage ## Collect coverage report as QMS evidence
	@echo "$(CYAN)>>> Collecting coverage report...$(NC)"
	@mkdir -p "$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)"
	@# Collect combined report if present
	@cp -f "$(BAZEL_OUTPUT_PATH)/_coverage/_baseline_report.dat" \
		"$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)/combined-coverage.lcov" 2>/dev/null || true
	@# Also collect per-test coverage.dat files
	@for t in $(TESTLOGS_DIR)/src/test/cpp/*/; do \
		test_name=$$(basename "$$t"); \
		if [ -f "$$t/coverage.dat" ]; then \
			cp -f "$$t/coverage.dat" \
				"$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)/$${test_name}-coverage.dat" 2>/dev/null || true; \
		fi; \
	done
	@if command -v genhtml >/dev/null 2>&1 && \
	   [ -f "$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)/combined-coverage.lcov" ]; then \
		genhtml "$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)/combined-coverage.lcov" \
			--output-directory "$(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)/html" \
			--title "Atlassians Coverage - $(DATE) $(SHORT_SHA1)" \
			--legend --show-details 2>/dev/null || true; \
		echo "$(GREEN)>>> HTML coverage report generated.$(NC)"; \
	else \
		echo "$(YELLOW)>>> genhtml not found or no combined report; LCOV data files saved.$(NC)"; \
	fi
	@echo "$(GREEN)>>> Coverage report saved to $(EVIDENCE_COVERAGE_DIR)/$(DATE)-$(SHORT_SHA1)$(NC)"

.PHONY: generate-test-summary
generate-test-summary: ## Generate a human-readable test summary from collected evidence
	@echo "$(CYAN)>>> Generating test summary...$(NC)"
	@mkdir -p "$(EVIDENCE_DIR)"
	@SUMMARY="$(EVIDENCE_DIR)/test-summary-$(DATE)-$(SHORT_SHA1).md"; \
	echo "# Test Evidence Summary" > "$$SUMMARY"; \
	echo "" >> "$$SUMMARY"; \
	echo "| Field | Value |" >> "$$SUMMARY"; \
	echo "|-------|-------|" >> "$$SUMMARY"; \
	echo "| **Date** | $(DATE) |" >> "$$SUMMARY"; \
	echo "| **Branch** | $(GIT_BRANCH) |" >> "$$SUMMARY"; \
	echo "| **Commit** | $(SHA1) |" >> "$$SUMMARY"; \
	echo "| **Short SHA** | $(SHORT_SHA1) |" >> "$$SUMMARY"; \
	echo "| **Release Level** | $(RELEASE_LEVEL) |" >> "$$SUMMARY"; \
	echo "| **Last Tag** | $(LAST_TAG) |" >> "$$SUMMARY"; \
	echo "| **SemVer** | $(SEM_VERSION) |" >> "$$SUMMARY"; \
	echo "" >> "$$SUMMARY"; \
	echo "## Unit Test Results" >> "$$SUMMARY"; \
	echo "" >> "$$SUMMARY"; \
	echo "| Test Suite | Status | Duration |" >> "$$SUMMARY"; \
	echo "|------------|--------|----------|" >> "$$SUMMARY"; \
	for xml in $(TESTLOGS_DIR)/src/test/cpp/*/test.xml; do \
		suite=$$(basename $$(dirname "$$xml")); \
		tests=$$(grep -oP 'tests="\K[0-9]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		failures=$$(grep -oP 'failures="\K[0-9]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		duration=$$(grep -oP 'time="\K[0-9.]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		if [ "$$failures" = "0" ]; then \
			echo "| $$suite | PASSED ($$tests tests) | $${duration}s |" >> "$$SUMMARY"; \
		else \
			echo "| $$suite | FAILED ($$failures/$$tests failed) | $${duration}s |" >> "$$SUMMARY"; \
		fi; \
	done; \
	echo "" >> "$$SUMMARY"; \
	echo "## BDD/Integration Test Results" >> "$$SUMMARY"; \
	echo "" >> "$$SUMMARY"; \
	echo "| Feature Test | Status | Duration |" >> "$$SUMMARY"; \
	echo "|--------------|--------|----------|" >> "$$SUMMARY"; \
	for xml in $(TESTLOGS_DIR)/src/it/*/test.xml; do \
		suite=$$(basename $$(dirname "$$xml")); \
		tests=$$(grep -oP 'tests="\K[0-9]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		failures=$$(grep -oP 'failures="\K[0-9]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		errors=$$(grep -oP 'errors="\K[0-9]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		duration=$$(grep -oP 'time="\K[0-9.]+' "$$xml" 2>/dev/null | head -1 || echo "?"); \
		if [ "$$failures" = "0" ] && [ "$$errors" = "0" ]; then \
			echo "| $$suite | PASSED ($$tests tests) | $${duration}s |" >> "$$SUMMARY"; \
		else \
			echo "| $$suite | FAILED ($$failures failures, $$errors errors) | $${duration}s |" >> "$$SUMMARY"; \
		fi; \
	done; \
	echo "" >> "$$SUMMARY"; \
	echo "---" >> "$$SUMMARY"; \
	echo "_Generated by CI pipeline on $$(date -u '+%Y-%m-%dT%H:%M:%SZ')_" >> "$$SUMMARY"; \
	echo "$(GREEN)>>> Test summary written to $$SUMMARY$(NC)"

##@ Packaging & Delivery

.PHONY: package
package: compile ## Package all deliverables (binaries, tests, BDD)
	@echo "$(CYAN)>>> Packaging deliverables...$(NC)"
	@bazelisk build \
		//src/main/cpp:com.github.doevelopper.atlassians.main.package \
		//src/test/cpp:com.github.doevelopper.atlassians.test.package \
		//src/it:com.github.doevelopper.atlassians.it.package
	@echo "$(GREEN)>>> Packages built successfully.$(NC)"

.PHONY: collect-packages
collect-packages: package ## Collect built packages as QMS evidence
	@echo "$(CYAN)>>> Collecting package artifacts...$(NC)"
	@mkdir -p "$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)"
	@cp -f "$$(bazelisk info bazel-bin)/src/main/cpp/com.github.doevelopper.atlassians.main.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/" 2>/dev/null || true
	@cp -f "$$(bazelisk info bazel-bin)/src/test/cpp/com.github.doevelopper.atlassians.test.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/" 2>/dev/null || true
	@cp -f "$$(bazelisk info bazel-bin)/src/it/com.github.doevelopper.atlassians.it.package.tar.gz" \
		"$(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)/" 2>/dev/null || true
	@echo "$(GREEN)>>> Packages collected to $(EVIDENCE_PACKAGES_DIR)/$(DATE)-$(SHORT_SHA1)$(NC)"

.PHONY: collect-evidence
collect-evidence: collect-unit-test-reports collect-bdd-test-reports collect-coverage-report collect-packages generate-test-summary ## Collect all QMS evidence (tests, BDD, coverage, packages)
	@echo "$(GREEN)>>> All evidence collected under $(EVIDENCE_DIR)$(NC)"

##@ Full Delivery Pipeline

.PHONY: deliver
deliver: collect-evidence ## Full delivery pipeline: build, test, coverage, BDD, package, collect evidence
	@echo ""
	@echo "$(GREEN)+----------------------------------------------------------------------+$(NC)"
	@echo "$(GREEN)|              Delivery Pipeline Completed Successfully                |$(NC)"
	@echo "$(GREEN)+----------------------------------------------------------------------+$(NC)"
	@echo "$(CYAN)  Branch:        $(GIT_BRANCH)$(NC)"
	@echo "$(CYAN)  Commit:        $(SHORT_SHA1)$(NC)"
	@echo "$(CYAN)  Release Level: $(RELEASE_LEVEL)$(NC)"
	@echo "$(CYAN)  Date:          $(DATE)$(NC)"
	@echo "$(CYAN)  Evidence:      $(EVIDENCE_DIR)$(NC)"
	@echo "$(GREEN)+----------------------------------------------------------------------+$(NC)"

.PHONY: verify
verify: compile test integration-test coverage ## Verify: build + unit test + BDD + coverage (no evidence collection)
	@echo "$(GREEN)>>> All verification steps passed.$(NC)"

##@ Cleanup

.PHONY: clean
clean: ## Cleaned up the objects and intermediary files
	@bazelisk clean

.PHONY: expunge
expunge: ## Removes the entire working tree for this bazel instance
	@bazelisk clean --expunge --async

.PHONY: clean-evidence
clean-evidence: ## Remove all collected evidence artifacts
	@echo "$(YELLOW)>>> Removing evidence from $(EVIDENCE_DIR)$(NC)"
	@find "$(EVIDENCE_DIR)" -mindepth 1 -not -name '.gitkeep' -delete 2>/dev/null || true
	@echo "$(GREEN)>>> Evidence cleaned.$(NC)"

##@ Info

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
	$(Q)echo "$@ ->"
	$(Q)echo '---------------$(CURDIR)------------------'
	$(Q)echo '+----------------------------------------------------------------------+'
	$(Q)echo '|                        Available Commands                            |'
	$(Q)echo '+----------------------------------------------------------------------+'
	$(Q)echo
	$(Q)awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {sub("\\\\n",sprintf("\n%22c"," "), $$2);printf " \033[36m%-20s\033[0m  %s\n", $$1, $$2}' $(MAKEFILE_LIST)
	$(Q)echo ""
	$(Q)echo "For more details on a specific command, run 'make <command> --help'"