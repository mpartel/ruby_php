require 'rubygems'
require 'hoe'
require 'rake/extensiontask'

# Hoe.plugin :compiler
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :inline
# Hoe.plugin :racc
# Hoe.plugin :rubyforge

if ENV['PHP_EMBED_DIR']
  ENV['PATH'] += ':' + File.join(ENV['PHP_EMBED_DIR'], 'bin')
  ENV['LD_LIBRARY_PATH'] = ':' + File.join(ENV['PHP_EMBED_DIR'], 'lib')
end

Hoe.spec 'ruby_php' do
  developer('Martin Pärtel', 'martin.partel@gmail.com')
  self.version = "0.0.1"
  self.readme_file = 'README.markdown'
  
  self.extra_dev_deps << ['rake-compiler', '>= 0']
  self.spec_extras = { :extensions => ["ext/ruby_php/extconf.rb"] }

  Rake::ExtensionTask.new('ruby_php', spec) do |ext|
    ext.lib_dir = File.join('lib', 'ruby_php')
    if ENV['PHP_EMBED_DIR']
      ext.config_options << "--with-php-dir=#{ENV['PHP_EMBED_DIR']}"
    end
  end
end

Rake::Task[:test].prerequisites << :compile


desc "Run tests under valgrind. Set VALGRIND_OPTS to give options to valgrind."
task "test:valgrind" => :compile do
  valgrind_opts = [
    "--error-exitcode=1",
    "--suppressions=valgrind.supp",
    "--leak-check=no",
    "-q"
  ].join(' ') + ' ' + ENV['VALGRIND_OPTS'].to_s
  
  test_with_tool("valgrind #{valgrind_opts}")
end

def test_with_tool(tool_cmd)
  ruby_cmd = "ruby #{Hoe::RUBY_FLAGS}"
  requires = []
  requires << 'rubygems'
  requires << 'test/unit'
  requires += Dir.glob('test/test_*.rb')
  requires.map! {|f| "require \"#{f}\"" }
  
  cmd = "#{tool_cmd} #{ruby_cmd} -e '#{requires.join("; ")}'"
  puts cmd
  system(cmd)
end


