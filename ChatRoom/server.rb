require_relative 'pool.rb'
require_relative 'room.rb'
require_relative 'client.rb'

class MyServer
        #requirements for class 'MyServer'
        # Poolsize was arbitrarily chosen to be 10,
        # this can be changed by updating variable 'psize'
        require 'socket'

        def initialize(port)
            @port = port
            #hardcoded 10
            @pool = Pool.new(10)   
            @active =  true
            @server = TCPServer.new port
            @portcount = 8001
            @usernumber = 0;
            @roomcount
            #create hashtable for users;
            @users = Hash.new ()

            #create hashtable for chatrooms
            @chats = Hash.new ();
            @cname = Hash.new();
        end
        
        #check name to check if valid user
        def namecheck(name)
           # puts("looking for name")
            if @users.has_key?(name) == true
                return true
            else
          #      puts("name not found?")
                return false
            end
        end

        #add user to table
        def registerUser(name, usernumber)
         #   puts ("adding user")
            @users.store(name, usernumber)
        end

        # check if chatroom already exists
        def roomexists(rname)
            if @cname.has_key?(rname) == true
                return true
            else
                puts("room not found?")
                return false
            end
        end
        #register new room
        def createRoom(rname, username, pcount)
            userID = @users.fetch username
            @cname.store(rname, pcount)
            puts ("create the rooom")
            #puts(userID)
            room = Room.new(rname, pcount, userID, username)     #create new room
            #room.printMe
            @chats.store(rname, room)                  #add record of the chat
            joinRoom(rname, username)

        end
        
        #enter a room, assign user keynumber
        def joinRoom(roomname, username)
            room = @chats.fetch roomname     #get record of "chats"
            #puts(room)
            userID = @users.fetch username
            puts("joinroom userID #{userID}")
            puts(userID)
            room.addUser(userID, username)                  #add user to that chat
            puts room.printMe
        end
        
        def getUserNum(uname)
            name = @users.fetch uname
            return name
        end


    def run()
        loop do
            client = @server.accept
                @pool.schedule do
                    while @active == true do
                        
                            msg = client.gets
                            #check for join
                            if msg.start_with?("JOIN_CHATROOM:") == true
                               client.puts("up for da chats")
                               parsemsg = msg.split            # parsemsg = {[JOIN_CHATROOM:] [chatroom name] [CLIENT_IP:] [0] [PORT:] [0] [CLIENT_NAME:] [name]}
                               client.puts(parsemsg[7]) 
                               #puts("checking username")
                                   if namecheck(parsemsg[7]) == true        #check user exists      works
                                        puts("valid user") 
                                   else
                                #        client.puts("user doesnt exist")
                                        @usernumber = @usernumber + 1
                                        client.puts("user nummmmm: #{@usernumber}")
                                        registerUser(parsemsg[7], @usernumber)           # add user to userTable    works
                                   end                             

                                    #join a room
                                   if roomexists(parsemsg[1]) == true
                                        puts("valid room.. joining..")
                                        joinRoom(parsemsg[1], parsemsg[7])        #join a rooom     ????
                                        roomno = @cname.fetch parsemsg[1]
                                        puts "rooooom #{roomno}"
                                        client.puts "----Response-----"
                                        client.puts("JOINED_CHATROOM: #{parsemsg[1]} SERVER_IP: [roomIPaddress] PORT: #{@roomcount} ROOM_REF: #{roomno} JOIN_ID: #{getUserNum(parsemsg[7])}
")
                                   else
                                        puts("room does not exist")
                                        puts(parsemsg[1])
                                        @portcount = @portcount +1
                                        createRoom(parsemsg[1], parsemsg[7], @portcount)        #create a room      works

                                   end 
                            #check for leave
                            elsif msg.start_with?("LEAVE_CHATROOM:") == true
                                   client.puts("Leaving")                  #do later

                            #check for message
                            elsif msg.start_with?("CHAT:") == true
                                    client.puts("sending..")

                                    puts(@chats)
                                    puts("\n\n")
                                    puts(@users)
                                    #CHAT: [ROOM_REF] JOIN_ID: [clientIDnumber] CLIENT_NAME: [string] MESSAGE: [string+'\n\n']
                                    room = @chats.fetch parsemsg[2]
                                    usr = @users.fetch parsemsg[5]
                                    client.puts("Message to send:" + parsemsg[7])
                                    tosend = parsemsg[7]
                                    room.sendmessage()


                            else
                                client.puts("ERROR_CODE: [integer] ERROR_DESCRIPTION: [error]")

                            end
                    end
                end
            end
    end
#class end
end

# create and run server taking args from commandline
s = MyServer.new(ARGV[0])
s.run()


