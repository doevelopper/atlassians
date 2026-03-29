# test.mk — Test targets for the project build system.
#
# Included by the top-level Makefile via:
#   include tasks/test.mk
#
# Assumes the following variables are set in common.mk:
#   BUILD_DIR     — out-of-source build directory
#   COVERAGE_DIR  — directory for coverage data and reports
#   REPORTS_DIR   — directory for test result XML/HTML files
#   CMAKE         — path to cmake binary
#   CTEST         — path to ctest binary (or $(CMAKE) --build ... --target test)

.PHONY: test test-unit test-integration test-bdd test-fuzz test-benchmark \
        test-coverage test-coverage-report test-clean

## ─── Unit tests ─────────────────────────────────────────────────────────────
test-unit: ## Run unit tests (GTest/GMock via CTest)
	$(CTEST) --test-dir $(BUILD_DIR) \
	          --output-on-failure \
	          --label-regex "^unit$$" \
	          --output-junit $(REPORTS_DIR)/unit-test-results.xml

## ─── Integration / BDD tests ─────────────────────────────────────────────────
test-integration: ## Run integration and BDD tests (Cucumber-cpp)
	$(CTEST) --test-dir $(BUILD_DIR) \
	          --output-on-failure \
	          --label-regex "^(integration|bdd)$$" \
	          --output-junit $(REPORTS_DIR)/integration-test-results.xml

test-bdd: test-integration ## Alias: run BDD tests

## ─── Fuzz tests ──────────────────────────────────────────────────────────────
test-fuzz: ## Run fuzz tests (LibFuzzer / AFL++ targets)
	$(CTEST) --test-dir $(BUILD_DIR) \
	          --output-on-failure \
	          --label-regex "^fuzz$$"

## ─── Benchmark tests ─────────────────────────────────────────────────────────
test-benchmark: ## Run benchmark tests (Google Benchmark)
	$(CTEST) --test-dir $(BUILD_DIR) \
	          --output-on-failure \
	          --label-regex "^benchmark$$" \
	          --output-junit $(REPORTS_DIR)/benchmark-results.xml

## ─── Full test suite ─────────────────────────────────────────────────────────
test: test-unit test-integration ## Run the full test suite (unit + integration)

## ─── Coverage ────────────────────────────────────────────────────────────────
test-coverage: ## Build and run tests with coverage instrumentation
	$(CMAKE) --build $(BUILD_DIR) --target coverage
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

test-coverage-report: test-coverage ## Generate HTML + Cobertura coverage reports
	gcovr \
	  --config .gcovr.cfg \
	  --html-details $(COVERAGE_DIR)/coverage.html \
	  --xml $(COVERAGE_DIR)/coverage.xml \
	  --sonarqube $(COVERAGE_DIR)/coverage-sonarqube.xml \
	  --print-summary

## ─── Cleanup ─────────────────────────────────────────────────────────────────
test-clean: ## Remove all test artefacts from artifacts/
	$(RM) -rf $(REPORTS_DIR)/* $(COVERAGE_DIR)/*
