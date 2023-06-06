
default: configure build

python_version := `python --version | cut -d' ' -f2 | cut -d'.' -f1,2`

# install deps and configure project
configure *args:
    mkdir -p build
    conan install . -if build -pr default -pr:b=default --build=missing -o python_version={{python_version}} {{args}}
    conan build -c . -if build -bf build

configure_test *args: (configure "-o test=True " + args)

# Build project
build:
    cmake --build build --parallel

make *args:
    make -C build {{args}}

# export PYTHONPATH := "build/python:$PYTHONPATH"

# python:
# 	ipython -i -c "print(\"import _sardine\"); import _sardine"

test:
	ctest --test-dir build

# Clean
clean:
    @ rm -rf                \
        build/*               \
        python/build          \
        python/*.so           \
        python/*.egg-info     \
        python/*/__pycache__  \
        .tox                  \
        .coverage             \
        .mypy_cache           \
        .pytest_cache         \
        dist
