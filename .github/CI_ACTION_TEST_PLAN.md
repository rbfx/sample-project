# CI action branch test plan

All scenario branches start from `rk/ci-reusable-workflow4`. SDK scenarios force
`RBFX_FRAMEWORK_BUILD_TYPE=sdk`; they use the repository, release, and credentials
already configured for the sample-project repository.

| Branch | Expected jobs | Purpose |
|---|---:|---|
| `rk/ci-test-sdk-all-grouped` | 1 planner + 20 builds | All supported platform tags, canonical build types, grouped sequential builds, runner/host mapping, multiline prefixes, explicit project/build/install/Android paths |
| `rk/ci-test-sdk-split-custom` | 1 planner + 4 builds | `separate_build_types=true`, custom short names, qualified Android Gradle tasks, one-entry matrix build maps, unique target/host job names |
| `rk/ci-test-sdk-selectors` | 1 planner + 2 builds | Platform-family selection, `@build-type` selection, exclusions, grouped one-entry maps |
| `rk/ci-test-sdk-direct-defaults` | 1 build | No matrix action, explicit platform input, JSON prefix list, canonical build-project defaults, default build directory, disabled non-Android installation |
| `rk/ci-test-sdk-explicit-override` | 1 planner + 1 build | Explicit build-project map overriding the matrix map, custom build/install paths, typed and untyped CMake cache variables |
| `rk/ci-test-wait-options` | 1 producer + 1 waiter | Exact job-name polling, JSON artifact-name input, explicit timeout/grace/token, ordered artifact IDs, caller-owned downloads |
| `rk/ci-test-source-smoke` | 1 planner + 1 build | One native Linux Debug source build; deliberately the only engine-source compilation in this suite |

## Coverage by action

### ci-prepare-matrix

- Optional use: `rk/ci-test-sdk-direct-defaults`
- `platforms=all`: `rk/ci-test-sdk-all-grouped`
- Exact tags: split, direct, and explicit-override scenarios
- Platform selector, `@build-type`, and exclusions: selector scenario
- Default and custom `build_types`: grouped and split scenarios
- Grouped and separate layouts: grouped and split scenarios

### ci-prepare-platform

- Matrix-derived and explicit `ci_platform_tag`
- `profile=downstream`
- Multiline and JSON `cmake_prefix_path`
- Native and cross-platform SDK layouts
- Source-tree prefix in the single source smoke scenario

The engine profile remains covered by the rbfx engine workflow. Repeating it in every
sample-project scenario would build the engine instead of testing downstream action
composition.

### ci-build-project

- Explicit and default project/build/install paths
- Matrix, explicit, and canonical-default `build_types`
- Sequential multi-configuration builds and one-configuration split jobs
- Empty non-Android install path
- `cmake_cache_vars`
- Android `android_dir` and sequential Gradle tasks
- Build/install outputs used by verification and artifact upload steps

### ci-wait-for-build

- `job_name`, JSON `artifact_names`, `timeout_seconds`,
  `artifact_grace_seconds`, and explicit `token`
- Two artifacts to verify input/output ordering and caller-owned download

## Cost policy

- SDK branches may cover all platforms because they compile only the sample project.
- Source coverage is limited to one native Linux Debug job.
- The wait action uses a synthetic producer instead of a source cross-build, avoiding
  two additional engine compilations while still exercising every wait input.
