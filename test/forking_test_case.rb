require 'method_call_recorder'

# We want to fork a clean process for each of our tests
# because PHP fatal errors leave the PHP interpreter in an unusable state.
class ForkingTestCase < Test::Unit::TestCase
  def run(result)
    return if self.class == ForkingTestCase # This superclass contains no tests
    
    yield(STARTED, name)
    
    pipe_in, pipe_out = IO.pipe
    
    pid = fork do
      begin
        pipe_in.close
        result_recorder = MethodCallRecorder.new
        super(result_recorder) do
          # no progress reporting in the child process
        end
        pipe_out.write(Marshal.dump(result_recorder.recorded_method_calls))
        
        # minimize memory leaks
        GC.stress = true
        GC.start
      rescue Exception
        pipe_out.write(Marshal.dump($!))
      end
      exit!(0)
    end
    
    pipe_out.close
    
    output = pipe_in.read
    pipe_in.close
    
    Process.waitpid(pid)
    
    @_result = result
    if $? == 0
      output = Marshal.load(output)
      if output.is_a? Exception
        raise output
      else
        output.each do |args|
          @_result.__send__(*args)
        end
      end
    else
      add_error(RuntimeError.new("Process #{pid} crashed with status #{$?.inspect}."))
    end
    
    yield(FINISHED, name)
  end
end
