.PHONY: bootstrap lint build ci

bootstrap: ## Run bootstrap checks
	@bash ci/pipelines/bootstrap.sh

lint: ## Run lint pipeline stage
	@bash ci/pipelines/lint.sh

build: ## Run build pipeline stage
	@bash ci/pipelines/build.sh

ci: bootstrap lint build test package sbom provenance ## Run full CI sequence