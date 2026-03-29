SHELL := /usr/bin/env bash

include tasks/common.mk
include tasks/ci.mk
include tasks/test.mk
include tasks/security.mk
include tasks/release.mk

.PHONY: help

help: ## Show available targets
	@grep -hE '^[a-zA-Z0-9_.-]+:.*##' Makefile tasks/*.mk | \
		sed 's/:.*##/: /' | sort