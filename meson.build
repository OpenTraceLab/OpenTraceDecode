project('opentracedecode', 'c',
  version: '0.1.0',
  default_options: ['warning_level=2','c_std=c11'])

# Auto-detect version from git tags (latest by date)
git_version_cmd = run_command('git', 'describe', '--tags', '--abbrev=0', check: false)
git_version_from_tag = git_version_cmd.returncode() == 0 ? git_version_cmd.stdout().strip() : ''

# Version handling - priority: git tag > project version
if git_version_from_tag != ''
  # Remove 'v' prefix if present
  otd_version_raw = git_version_from_tag.startswith('v') ? git_version_from_tag.substring(1) : git_version_from_tag
  # Strip pre-release suffix (e.g., "0.1.0-alpha.3" -> "0.1.0")
  otd_version = otd_version_raw.contains('-') ? otd_version_raw.split('-')[0] : otd_version_raw
else
  otd_version = meson.project_version()
endif

cc = meson.get_compiler('c')

# --- Dependencies ---
dep_glib = dependency('glib-2.0', required: true)

py_mod = import('python')
py = py_mod.find_installation('python3', required: get_option('python').enabled())
dep_py = disabler()
if py.found()
  dep_py = py.dependency(version: '>=' + get_option('python_minver'), embed: true, required: get_option('python').enabled())
endif

# --- Configuration ---
conf_data = configuration_data()
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
conf_data.set_quoted('PACKAGE_NAME', meson.project_name())
conf_data.set_quoted('PACKAGE_TARNAME', 'opentracedecode')
conf_data.set('HAVE_PYTHON', dep_py.found())

# Version components
version_parts = meson.project_version().split('.')
conf_data.set('OTD_PACKAGE_VERSION_MAJOR', version_parts[0])
conf_data.set('OTD_PACKAGE_VERSION_MINOR', version_parts[1])
conf_data.set('OTD_PACKAGE_VERSION_MICRO', version_parts[2])
conf_data.set_quoted('OTD_PACKAGE_VERSION_STRING', meson.project_version())

# Library version info
conf_data.set('OTD_LIB_VERSION_CURRENT', '1')
conf_data.set('OTD_LIB_VERSION_REVISION', '0')
conf_data.set('OTD_LIB_VERSION_AGE', '0')
conf_data.set_quoted('OTD_LIB_VERSION_STRING', '1:0:0')

# Build info
conf_data.set_quoted('CONF_HOST', host_machine.system() + '-' + host_machine.cpu_family())

config_h = configure_file(
  output: 'config.h',
  configuration: conf_data
)

version_h = configure_file(
  input: 'version.h.in',
  output: 'version.h',
  configuration: conf_data
)

# --- Includes ---
inc_pub = include_directories('include')        # public
inc_src = include_directories('src')            # private
inc_build = include_directories('.')            # build directory for generated headers

# --- Sources (start small; add as you port) ---
# Move/rename upstream sources into src/, then list them here:
src_core = files(
  'src/decoder.c',
  'src/error.c',
  'src/exception.c',
  'src/instance.c',
  'src/log.c',
  'src/module_opentracedecode.c',
  'src/session.c',
  'src/otd.c',
  'src/type_decoder.c',
  'src/util.c',
  'src/version.c',
)

# Build lib
libdeps = [dep_glib]
if dep_py.found()
  libdeps += dep_py
endif

lib_args = []
if host_machine.system() == 'windows'
  lib_args += '-DDLL_EXPORT'
endif
libname = 'opentracedecode'

lib_shared = library(libname, src_core, config_h, version_h,
  include_directories: [inc_pub, inc_src, inc_build],
  dependencies: libdeps,
  c_args: lib_args,
  install: get_option('build_shared'),
  version: meson.project_version(),
  soversion: '0')

# Optional static build
lib_static = static_library(libname, src_core, config_h, version_h,
  include_directories: [inc_pub, inc_src, inc_build],
  dependencies: libdeps,
  install: get_option('build_static'))

# Install public headers (umbrella + others)
install_subdir('include/opentracedecode', install_dir: get_option('includedir'))

# Install protocol decoders
install_subdir('decoders', install_dir: get_option('datadir') / 'opentracedecode')

# pkg-config
pkg = import('pkgconfig')
pkg.generate(
  name: 'opentracedecode',
  description: 'OpenTraceDecode protocol decoding core (Python-embedded)',
  version: meson.project_version(),
  libraries: lib_shared,       # ok even if static also exists
  requires: ['glib-2.0']
)

# Optional Python extension modules for testing
if get_option('python_shims') and dep_py.found()
  py_ext = py.extension_module('_opentracedecode',
    src_core,
    config_h,
    version_h,
    include_directories: [inc_pub, inc_src, inc_build],
    dependencies: libdeps,
    install: false
  )
endif

# Simple test executable (smoke)
test_env = environment()
if get_option('decoders_path') != ''
  test_env.set('PYTHONPATH', get_option('decoders_path'))
endif

# Test executable - use static lib on Windows to avoid DLL export issues
test_lib = host_machine.system() == 'windows' ? lib_static : lib_shared
test_exe = executable('otd-smoke',
  ['tests/smoke.c'],
  include_directories: inc_pub,
  dependencies: libdeps,
  link_with: test_lib)
test('smoke', test_exe, env: test_env)

# Feature summary
summary({
  'glib-2.0': true,
  'python embed': dep_py.found(),
}, section: 'Dependencies', bool_yn: true)
