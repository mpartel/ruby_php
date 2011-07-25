require 'rubygems'
require 'hoe'
require 'rake/extensiontask'

# Hoe.plugin :compiler
# Hoe.plugin :gem_prelude_sucks
# Hoe.plugin :inline
# Hoe.plugin :racc
# Hoe.plugin :rubyforge

Hoe.spec 'ruby_php' do
  developer('Martin Pärtel', 'martin.partel@gmail.com')
  self.readme_file = 'README.markdown'
  
  self.extra_dev_deps << ['rake-compiler', '>= 0']
  self.spec_extras = { :extensions => ["ext/ruby_php/extconf.rb"] }

  Rake::ExtensionTask.new('ruby_php', spec) do |ext|
    ext.lib_dir = File.join('lib', 'ruby_php')
  end
end

Rake::Task[:test].prerequisites << :compile
