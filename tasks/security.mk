.PHONY: security sbom provenance

security: ## Run security scans
	@bash ci/pipelines/security.sh

sbom: ## Generate software bill of materials
	@bash ci/pipelines/sbom.sh

provenance: ## Generate build provenance metadata
	@bash ci/pipelines/provenance.sh