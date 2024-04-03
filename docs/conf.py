# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#

import os
import re
import subprocess
import sys
from pathlib import Path

DIR = Path(__file__).parent.resolve()

# sys.path.insert(0, os.path.abspath('../python'))


# -- Project information -----------------------------------------------------

project = 'emu'
copyright = '2022, Julien Bernard'
author = 'Julien Bernard'

# The full version, including alpha/beta/rc tags
release = '0.2.0'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'breathe',
    'sphinx.ext.autodoc',
    'sphinx_rtd_dark_mode'
]

breathe_projects = {"emu": os.path.abspath('../build/docs/doxygen/xml')}
breathe_default_project = "emu"
breathe_domain_by_extension = {"cuh": "cpp"}


# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']


primary_domain = "cpp"
highlight_language = "cpp"

def generate_doxygen_xml(app):


    build_dir = os.path.join(app.confdir, "../build/docs/doxygen")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # with open('Doxyfile.in', 'r') as file :
    #     filedata = file.read()

    # filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', build_dir)

    # with open('Doxyfile', 'w') as file:
    #     file.write(filedata)

    try:
        subprocess.call(["doxygen", "--version"])
        retcode = subprocess.call(["doxygen"], cwd=app.confdir)
        if retcode < 0:
            sys.stderr.write(f"doxygen error code: {-retcode}\n")
    except OSError as e:
        sys.stderr.write(f"doxygen execution failed: {e}\n")


# def prepare(app):
#     with open(DIR.parent / "README.rst") as f:
#         contents = f.read()

#     if app.builder.name == "latex":
#         # Remove badges and stuff from start
#         contents = contents[contents.find(r".. start") :]

#         # Filter out section titles for index.rst for LaTeX
#         contents = re.sub(r"^(.*)\n[-~]{3,}$", r"**\1**", contents, flags=re.MULTILINE)

#     with open(DIR / "readme.rst", "w") as f:
#         f.write(contents)


def clean_up(app, exception):
    ...
    # (DIR / "Doxyfile").unlink()
    # (DIR / "readme.rst").unlink()


def setup(app):

    # Add hook for building doxygen xml when needed
    app.connect("builder-inited", generate_doxygen_xml)

    # Copy the readme in
    # app.connect("builder-inited", prepare)

    # Clean up the generated readme
    app.connect("build-finished", clean_up)
