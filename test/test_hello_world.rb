require 'ruby_php'

class TestHelloWorld < Test::Unit::TestCase
  
  def test_hello_world
    assert_equal('hello world', PHP.hello_world)
  end
  
end

