# SilentCryptoMinerRemover
A simple yet powerful program to remove Unam Sanctam's silent cryptocurrency miners.



Have you been downloading some weird torents? Are you scared some random Russian kid is going to use your hard earned cpu power to mine some Monero? Well, be afraid no more, with this simple program you can remove any of Unam Sanctam's silent cryptocurrency miners. 
While I recognise there is significant educational value in his free silent miner builders we cannot hide ourselves from the fact that someone is bound to use them for malicious purposes.



How does it work?
-The program kills any process the miner and "watchdog" are known to infect

-The registry key that allows the miner to boot on startup is removed (and backed up just in case)

-The executable linked to the registry key is removed

-The computer is restarted, since the aformentioned registry key was removed and critical files were deleted the miner is unable to restart 



What are this program's shortcomings?
-The program doesn't detect the miner, it presents the user with suspicious executables and its up to the user to decide which one it is, the user has to be sure there is a miner on his computer, else they might remove a legitimate program.

-The program is only effective against @UnamSanctam 's miners and though they are popular now this might change later.




Anyways, have fun, this took way too long to make because troubleshooting c++ is always pain.




Screenshots:
![Capture2](https://user-images.githubusercontent.com/110684896/183122194-63fe7fed-e779-4dfa-9d4e-5fa7baf5e536.PNG)

explorer.exe is the miner process in that example. As you can see it consumes abnormally high amounts of cpu power for XMR mining


![Capture5](https://user-images.githubusercontent.com/110684896/183122406-0d02b2a9-a139-4cab-886e-6abcb16e3b0a.PNG)

The miner creates a new registry key to boot on startup, in this example it is hidden as the Google Chrome updater


![Capture6](https://user-images.githubusercontent.com/110684896/183122657-9ac263ea-c948-4fe7-a88d-5a79e6261b14.PNG)

Screenshot of the remover's start up screen



Fuel my caffein addiction:
XMR: 49Jv6e96ZyiA1HKcZ6y3pf7XPdsqF77ph1GSzbgaBSd71FwfiwsBeQTNqh2DHzifvz4ZyVZXCFNVVNq6mETLPiV5FXAw3Kb
