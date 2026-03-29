.PHONY: package release release-dry-run

package: ## Create distributable source package
	@bash ci/pipelines/package.sh

release: ## Run release stage
	@CI_RELEASE=true bash ci/pipelines/release.sh

release-dry-run: ## Simulate release without publishing
	@DRY_RUN=true bash ci/pipelines/release.sh