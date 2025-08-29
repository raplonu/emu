# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive CI/CD pipeline with GitHub Actions and GitLab CI
- Automated testing with multiple compiler versions (GCC 11/12)
- Static analysis integration with clang-tidy
- Automated code formatting checks with clang-format
- Security vulnerability scanning with Trivy
- Automated documentation building with Sphinx + Doxygen
- Conan package validation and testing
- Coverage reporting integration with Coveralls
- Dependency management and auditing workflows
- Automated release management with GitHub Releases
- Performance regression testing framework (template)
- Comprehensive CI/CD documentation

### Changed
- Updated CI triggers to support development workflows (develop, release/*, hotfix/* branches)
- Enhanced GitLab CI configuration to match GitHub Actions capabilities
- Updated README with CI/CD status badges and documentation

### Fixed
- Improved error handling and artifact collection in CI workflows

## [1.0.0] - 2024-08-29

### Added
- Initial release of emu library
- Core C++ utilities
- CUDA extensions
- Python bindings
- Basic CI/CD setup
- Documentation with Sphinx + Doxygen
- Conan package support
- CMake build system with presets