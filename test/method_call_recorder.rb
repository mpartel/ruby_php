
class MethodCallRecorder
  attr_reader :recorded_method_calls
  
  def initialize
    @recorded_method_calls = []
  end
  
  def method_missing(*args)
    @recorded_method_calls << args
  end
end

