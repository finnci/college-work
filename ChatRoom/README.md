ChatRoom
========
Implementing a centralised chat server

Building on SkeletonServer


Chat Protocol:
==============


Join chat request format:

JOIN_CHATROOM: [chatroomname] CLIENT_IP: [0] PORT: [0] CLIENT_NAME: [string]

Join chat response:

JOINED_CHATROOM: [chatroomname] SERVER_IP: [roomIPaddress] PORT: [chatroomnumber] ROOM_REF: [roomkey] JOIN_ID: [clientIDnumber]

Join chat fail:

ERROR_CODE: [integer] ERROR_DESCRIPTION: [error]



Leave chatroom request

LEAVE_CHATROOM: [roomkey] JOIN_ID: [clientIDnumber] CLIENT_NAME: [string]

Response:

DISCONNECT: [0] PORT: [0] CLIENT_NAME: [string]


Send message:

CHAT: [ROOM_REF] JOIN_ID: [clientIDnumber] CLIENT_NAME: [string] MESSAGE: [string+'\n\n']

Server sends to chatroom:

CHAT: [ROOM_REF] CLIENT_NAME: [string] MESSAGE: [string+'\n\n']