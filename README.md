# s-talk
A simple client-server messaging program written in C using pthreads.

# Requirements
A UNIX terminal like `bash`; Windows is not supported.

# How to build
```
cd s-talk
make
```

# Usage
`s-talk [my port number] [remote machine name] [remote port number]`

# Example - Send messages to yourself from two different terminals on the same computer
Terminal 1
`s-talk 5000 localhost 5001`

Terminal 2
`s-talk 5001 localhost 5000`

# Example - Send messages from two different computers on the same LAN
Get your hostname by entering the commmand
`hostname`

Computer 1 (hostname "delta")
`s-talk 5000 gamma 5001`

Computer 2 (hostname "gamma")
`s-talk 5001 delta 5000`

# Details
s-talk listens for messages from [my port number] on the Local Area Network (LAN).


When you type a message and hit enter, the message is sent to [remote machine name] on [remote port number].

# Limitations
s-talk does not support communication outside of the Local Area Network (LAN).
