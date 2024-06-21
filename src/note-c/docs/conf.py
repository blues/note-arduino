# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import sys
import os
from pathlib import Path
from sphinx.cmd.build import get_parser
import sphinx_rtd_theme

args = get_parser().parse_args()

NOTE_C_BASE = Path(__file__).resolve().parents[1]
NOTE_C_BUILD = Path(args.outputdir).resolve()

# Add the '_extensions' directory to sys.path, to enable finding Sphinx
# extensions within.
sys.path.insert(0, str(NOTE_C_BASE / 'docs' / '_extensions'))

# Project information

project = 'note-c'
copyright = '2023, Blues'
author = 'Blues'

# General configuration

extensions = [
    'doxyrunner',
    'breathe',
    'sphinx.ext.autosectionlabel',
    'sphinxcontrib.jquery',
]
suppress_warnings = ['autosectionlabel.*']
templates_path = ['_templates']
exclude_patterns = ['build/']
master_doc = 'index'

# Options for autosectionlabel

autosectionlabel_prefix_document = True

# Options for doxyrunner

doxyrunner_doxygen = os.environ.get('DOXYGEN_EXECUTABLE', 'doxygen')
doxyrunner_doxyfile = NOTE_C_BASE / 'docs' / 'Doxyfile.in'
doxyrunner_outdir = NOTE_C_BUILD / 'doxygen'
doxyrunner_fmt = True
doxyrunner_fmt_vars = {'NOTE_C_BASE': str(NOTE_C_BASE)}
doxyrunner_outdir_var = 'DOXY_OUT'
doxyrunner_silent = False

# Options for breathe

breathe_projects = {'note-c': str(doxyrunner_outdir / 'xml')}
breathe_default_project = 'note-c'
breathe_domain_by_extension = {
    'h': 'c',
    'c': 'c',
}

# Options for HTML output

html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
html_logo = str(NOTE_C_BASE / 'assets' / 'blues_logo_no_text.png')
html_theme_options = {
    'logo_only': True,
    'prev_next_buttons_location': None
}
