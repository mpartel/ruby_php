require 'ruby_php/ruby_php'

module PHP
  VERSION = '0.0.1'
  
  @default_instance = Proc.new do
    obj = Object.new
    class << obj
      include PHP
    end
    obj
  end.call
  
  def self.method_missing(*args)
    @default_instance.send(*args)
  end
end

