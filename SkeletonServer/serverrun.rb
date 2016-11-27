require_relative 'pool.rb'


class MyServer
        #requirements for class 'MyServer'
        # Poolsize was arbitrarily chosen to be 10,
        # this can be changed by updating variable 'psize'
        require 'socket'
        @psize = 10
        def initialize(port)
            @port = port
            #hardcoded 10
            @pool = Pool.new(10)   
            @active =  true
            @server = TCPServer.new port
        end

        chats = Hash.new ( "not found");
        
    def run()
        loop do
            client = @server.accept
                @pool.schedule do
                    while @active == true do
                         
                            msg = client.gets
                            msg.split(' ', 2)
                         if msg[0..3] == "HELO"
                                #following line is not correct when not running on localhost for some reason
                                sock_domain, remote_port, remote_hostname, remote_ip = client.peeraddr
                               #output required message
                                client.puts msg  + "IP: an ip address"  + "insert local ip address for this part"  "\nPort:" + @port + "\nStudentID: 11353766 "
                        elsif msg == "KILL_SERVICE\r\n"
                                #client.puts "Disconnect\r\n"
                                #puts "Disconnect"
                                #initiate server shutdown
                                @pool.shutdown
                                @server.close
                                client.close
                                abort
                                #included to ensure client doesnt print out before thread has been shut
                                @active = false
                        elsif msg[0..13] =  JOIN_CHATROOM:                       #join server
                                        


                        elsif msg[0..14] =  LEAVE_CHATROOM:  #leave chatroom
                            


                        else client.puts "Invalid\r\n"
                        end
                    end
                end
            end
    end
end

# create and run server taking args from commandline
s = MyServer.new(ARGV[0])
s.run()


