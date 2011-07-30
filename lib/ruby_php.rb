require 'ruby_php/ruby_php'

module PHP

  def throw_errors_as_exceptions!
    if !PHP.eval_expr("function_exists('exception_error_handler')")
      error_handler_definition = <<EOS
function exception_error_handler($errno, $errstr, $errfile, $errline ) {
    throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
}
EOS
      self.eval_stmt(error_handler_definition)
    end
    
    self.eval_stmt("set_error_handler(\"exception_error_handler\");")
  end
  
  def capture_output(&block)
    begin
      eval_stmt("ob_start();")
      block.call
    ensure
      return eval_expr("ob_get_clean()")
    end
  end
  
  # TODO: find a way to capture fatal error output
  
  
  extend PHP # Make everything callable as PHP.method_name
end

