
require 'thread'
require 'socket'


class Room
#contains a name and a store of active clients.
def initialize(name, port ,userID ,username)
	@port = port
	@name = name
	@userTable = Hash.new()
	addUser(userID, username)
	
end


def sendmessage(message)

end

def addUser(userID, name)
	puts("Add user to chatroom")
	@userTable.store(userID, name)
	acknew(userID, name)

end


def acknew(userID, name)
	puts("#{userID}  #{name} has joined the chatroom")
end

def removeUser(userID)
	@userTable.delete userID

end

def printMe()
	puts(@port)
	puts(@name)
	puts(@userTable)
end

#end class
end