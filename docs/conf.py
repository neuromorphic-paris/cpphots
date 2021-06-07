# -- Path setup --------------------------------------------------------------

import subprocess, os

def configureDoxyfile(input_dir, output_dir):

    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@CMAKE_CURRENT_SOURCE_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

breathe_projects = {}
if read_the_docs_build:
    output_dir = 'build'
    configureDoxyfile(".", output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['cpphots'] = output_dir + '/xml'

# -- Project information -----------------------------------------------------

project = 'cpphots'
copyright = '2021, Lorenzo Vannucci'
author = 'Lorenzo Vannucci'

# project version
release = '0.5.0'


# -- General configuration ---------------------------------------------------

extensions = ["breathe", "exhale"]

templates_path = ['_templates']

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

breathe_default_project = "cpphots"

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./api",
    "rootFileName":          "library_root.rst",
    "rootFileTitle":         "cpphots API",
    "doxygenStripFromPath":  "..",
    # Suggested optional arguments
    "createTreeView":        True
}