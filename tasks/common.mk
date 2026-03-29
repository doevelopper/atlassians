ROOT_DIR := $(abspath .)
ARTIFACTS_DIR := $(ROOT_DIR)/artifacts
BUILD_DIR := $(ARTIFACTS_DIR)/build
REPORTS_DIR := $(ARTIFACTS_DIR)/reports
COVERAGE_DIR := $(ARTIFACTS_DIR)/coverage

CMAKE ?= cmake
CTEST ?= ctest
RM ?= rm

.PHONY: dirs clean

dirs: ## Create artifacts directories
	@mkdir -p $(ARTIFACTS_DIR) $(BUILD_DIR) $(REPORTS_DIR) $(COVERAGE_DIR)

clean: ## Remove build artifacts
	@$(RM) -rf $(ARTIFACTS_DIR)