to get SIGRT just change SIG and END_SIG in shared.h

for:
    #define SIG SIGRTMIN
    #define END_SIG SIGRTMAX
number of recieved was the same as specified in the function arguments
no signal was lost