require 'thread'
require 'socket'

 hostname = 'localhost'
 port = ARGV[0]

class Pool

#test
	
	def initialize(size)
	@active = true
	@size = size		#create pool of given size
	@jobs = Queue.new	#allows us to achedule work
	@pool = Array.new(@size) do |i|
	Thread.abort_on_exception = true
		Thread.new do
		  Thread.current[:id] = i
		  #puts  " adding thread\r\n"
		  catch(:exit) do
			loop do
			 jobs, args = @jobs.pop
			 jobs.call(*args)
			end
	        end
		end
	     end
	end

	#work scheduler
	#Tell the pool that we want something done.

	def schedule (*args, &block)	
		if @active == true
		@jobs << [block, args]
		end
	end
	#Close the threads
	def shutdown
		@size.times do |i|
			Thread.kill(@pool[i])
		end
			 
	end
	
end

