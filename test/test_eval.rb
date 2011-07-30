require 'test_helper'

class TestEval < ForkingTestCase
  
  def test_eval_stmt
    output = PHP.capture_output do
      PHP.eval_stmt('echo "Hello from PHP!";')
    end
    assert_equal("Hello from PHP!", output)
  end
  
  def test_eval_expr
    assert_equal(2, PHP.eval_expr('1 + 1'))
  end
  
  def test_syntax_error_in_eval_stmt
    assert_raise PHPError do
      PHP.eval_stmt("oopsie!!")
    end
  end
  
  def test_fatal_error_makes_php_unusable_in_this_process
    assert_raise PHPError do
      PHP.eval_stmt("oopsie!!")
    end
    
    begin
      PHP.eval_stmt('echo 123;')
    rescue PHPError => e
    end
    
    assert e.is_a?(PHPError)
    assert e.message.include?("PHP has suffered a fatal exception")
  end
  
  def test_exception_from_eval_stmt
    assert_raise PHPException do
      PHP.eval_stmt('throw new Exception("oops");')
    end
  end
  
  def test_errors_as_exceptions_from_eval
    PHP.throw_errors_as_exceptions!
    assert_raise PHPException do
      PHP.eval_stmt('strpos();')
    end
  end
  
end

