require 'ruby_php'

class TestEval < Test::Unit::TestCase
  
  def test_eval
    PHP.eval('echo "Hello from PHP!";')
    # TODO: capture PHP output
  end
  
  def test_simple_eval_expr
    assert_equal(2, PHP.eval_expr('1 + 1'))
  end
  
  # TODO: error handling etc.
  
end

