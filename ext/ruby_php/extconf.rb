require 'mkmf'
LIBDIR      = Config::CONFIG['libdir']
INCLUDEDIR  = Config::CONFIG['includedir']

HEADER_DIRS = [
  '/opt/local/include', # macports
  '/usr/local/include',
  INCLUDEDIR,
  '/usr/include'
]

LIB_DIRS = [
  '/opt/local/lib', # macports
  '/usr/local/lib',
  LIBDIR,
  '/usr/lib'
]

php_dirs = dir_config('php', HEADER_DIRS, LIB_DIRS)

unless php_dirs == ["", ""]
  php_inc = php_dirs[0]
  php_lib = php_dirs[1]
  php_subdirs = ["main", "Zend", "TSRM"]
  $INCFLAGS << " " << "-I#{php_inc}/php".quote
  for subdir in php_subdirs
    $INCFLAGS << " " << "-I#{php_inc}/php/#{subdir}".quote
  end
end

php_installation_instructions =
<<EOS
Please compile your PHP with `--enable-embed`.
EOS
php_installation_instructions.strip!

unless find_header('php.h', *HEADER_DIRS)
  abort php_installation_instructions
end

unless find_header('php/sapi/embed/php_embed.h', *HEADER_DIRS)
  abort php_installation_instructions
end

unless find_library('php5', 'php_embed_init', *LIB_DIRS)
  abort php_installation_instructions
end

create_makefile('ruby_php/ruby_php')

