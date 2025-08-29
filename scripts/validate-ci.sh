#!/bin/bash

# CI/CD Validation Script
# This script validates the CI/CD configuration and tests key components locally

set -e

echo "=== CI/CD Configuration Validation ==="
echo

# Check if all workflow files exist and are valid YAML
echo "🔍 Checking GitHub Actions workflows..."
for workflow in .github/workflows/*.yml; do
    if [ -f "$workflow" ]; then
        echo "  ✅ Found: $workflow"
        # Basic YAML syntax check
        python3 -c "import yaml; yaml.safe_load(open('$workflow'))" 2>/dev/null && \
            echo "    ✅ Valid YAML syntax" || \
            echo "    ❌ Invalid YAML syntax"
    fi
done

echo
echo "🔍 Checking GitLab CI configuration..."
if [ -f ".gitlab-ci.yml" ]; then
    echo "  ✅ Found: .gitlab-ci.yml"
    python3 -c "import yaml; yaml.safe_load(open('.gitlab-ci.yml'))" 2>/dev/null && \
        echo "    ✅ Valid YAML syntax" || \
        echo "    ❌ Invalid YAML syntax"
fi

echo
echo "🔍 Checking code quality configuration..."
[ -f ".clang-format" ] && echo "  ✅ Found: .clang-format" || echo "  ❌ Missing: .clang-format"
[ -f ".clang-tidy" ] && echo "  ✅ Found: .clang-tidy" || echo "  ❌ Missing: .clang-tidy"

echo
echo "🔍 Checking build configuration..."
[ -f "CMakeLists.txt" ] && echo "  ✅ Found: CMakeLists.txt" || echo "  ❌ Missing: CMakeLists.txt"
[ -f "CMakePresets.json" ] && echo "  ✅ Found: CMakePresets.json" || echo "  ❌ Missing: CMakePresets.json"
[ -f "conanfile.py" ] && echo "  ✅ Found: conanfile.py" || echo "  ❌ Missing: conanfile.py"

echo
echo "🔍 Checking documentation..."
[ -d "docs" ] && echo "  ✅ Found: docs directory" || echo "  ❌ Missing: docs directory"
[ -f "docs/conf.py" ] && echo "  ✅ Found: docs/conf.py (Sphinx)" || echo "  ❌ Missing: docs/conf.py"
[ -f "docs/Doxyfile" ] && echo "  ✅ Found: docs/Doxyfile (Doxygen)" || echo "  ❌ Missing: docs/Doxyfile"
[ -f "docs/ci-cd.md" ] && echo "  ✅ Found: docs/ci-cd.md" || echo "  ❌ Missing: docs/ci-cd.md"

echo
echo "🔍 Checking project files..."
[ -f "README.md" ] && echo "  ✅ Found: README.md" || echo "  ❌ Missing: README.md"
[ -f "CHANGELOG.md" ] && echo "  ✅ Found: CHANGELOG.md" || echo "  ❌ Missing: CHANGELOG.md"
[ -f "justfile" ] && echo "  ✅ Found: justfile" || echo "  ❌ Missing: justfile"

echo
echo "🔍 Checking CI/CD coverage..."

# Check if main CI tasks are covered
echo "  Build and Test:"
grep -q "build-and-test" .github/workflows/ci.yml && echo "    ✅ Build matrix testing" || echo "    ❌ Missing build matrix"
grep -q "coverage" .github/workflows/ci.yml && echo "    ✅ Coverage reporting" || echo "    ❌ Missing coverage"

echo "  Code Quality:"
grep -q "static-analysis" .github/workflows/ci.yml && echo "    ✅ Static analysis" || echo "    ❌ Missing static analysis"
grep -q "format-check" .github/workflows/code-quality.yml && echo "    ✅ Format checking" || echo "    ❌ Missing format check"

echo "  Security:"
grep -q "security-scan" .github/workflows/ci.yml && echo "    ✅ Security scanning" || echo "    ❌ Missing security scan"

echo "  Documentation:"
grep -q "documentation" .github/workflows/ci.yml && echo "    ✅ Documentation building" || echo "    ❌ Missing documentation"

echo "  Package Management:"
grep -q "package-validation" .github/workflows/ci.yml && echo "    ✅ Package validation" || echo "    ❌ Missing package validation"

echo "  Release Management:"
[ -f ".github/workflows/release.yml" ] && echo "    ✅ Release workflow" || echo "    ❌ Missing release workflow"

echo
echo "🔍 Checking trigger configurations..."

# Check trigger coverage
grep -A 20 "^on:" .github/workflows/ci.yml | grep -q "develop" && echo "  ✅ Develop branch trigger" || echo "  ❌ Missing develop trigger"
grep -A 20 "^on:" .github/workflows/ci.yml | grep -q "release" && echo "  ✅ Release branch trigger" || echo "  ❌ Missing release trigger"
grep -A 20 "^on:" .github/workflows/ci.yml | grep -q "hotfix" && echo "  ✅ Hotfix branch trigger" || echo "  ❌ Missing hotfix trigger"
grep -A 20 "^on:" .github/workflows/ci.yml | grep -q "tags" && echo "  ✅ Tag trigger" || echo "  ❌ Missing tag trigger"

echo
echo "=== Validation Summary ==="

# Count successes vs total checks
total_workflows=$(ls .github/workflows/*.yml 2>/dev/null | wc -l)
echo "📊 Workflows configured: $total_workflows"

if [ -f ".github/workflows/ci.yml" ] && [ -f ".github/workflows/code-quality.yml" ] && [ -f ".github/workflows/release.yml" ]; then
    echo "✅ Core CI/CD workflows: Complete"
else
    echo "❌ Core CI/CD workflows: Incomplete"
fi

if [ -f ".clang-format" ] && [ -f ".clang-tidy" ]; then
    echo "✅ Code quality configuration: Complete"
else
    echo "❌ Code quality configuration: Incomplete"
fi

if [ -f "docs/ci-cd.md" ] && [ -f "CHANGELOG.md" ]; then
    echo "✅ Documentation: Complete"
else
    echo "❌ Documentation: Incomplete"
fi

echo
echo "🎉 CI/CD validation complete!"
echo "See docs/ci-cd.md for detailed documentation."