## Summary

<!-- Describe WHAT this PR does and WHY -->

## Type of change

- [ ] `feat` — New feature
- [ ] `fix` — Bug fix
- [ ] `refactor` — Code restructuring (no behavior change)
- [ ] `perf` — Performance improvement
- [ ] `test` — Adding or updating tests
- [ ] `docs` — Documentation only
- [ ] `build` — Build system / dependencies
- [ ] `ci` — CI/CD pipeline changes
- [ ] `chore` — Maintenance (no production code change)

## Related Issues

<!-- Link to JIRA/GitHub issues: Closes #123, Relates to PROJ-456 -->

## Checklist

### Quality

- [ ] PR title follows [Conventional Commits](https://www.conventionalcommits.org/) format
- [ ] Code compiles without warnings (`bazelisk build //src/...`)
- [ ] All existing tests pass (`bazelisk test //src/test/cpp/... //src/it/...`)
- [ ] New code has corresponding unit tests
- [ ] Coverage meets threshold (≥ 80% on changed code)

### Review

- [ ] Self-reviewed my code
- [ ] Removed debugging artifacts (prints, TODOs, commented-out code)
- [ ] Updated documentation if public API changed

### Security

- [ ] No hardcoded secrets, tokens, or credentials
- [ ] No new dependencies with known CVEs
- [ ] Input validation added for new external interfaces

## Testing Evidence

<!-- Paste relevant test output, screenshots, or describe manual testing -->

## Notes for Reviewers

<!-- Any context that helps reviewers understand the changes -->
