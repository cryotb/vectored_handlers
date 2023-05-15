# Intro
I wanted to create a demo project which interacts with vectored handlers on windows, and this is the outcome. <br/>
It currently implements enumerating and hijacking VHs. However, adding one is still on my todo! It was simply not a priority due to the use case of this code. <br/>

# Use Case Samples
- Enumerating all registered VHs in a process and run checks on them, for example if they reside in a valid module.
- Hijacking already present VHs to execute your own logic instead.
- Removing a VH by calling `RemoveVectoredExceptionHandler` on it's handle (TODO: implement removing them without API.)

# References
https://dimitrifourny.github.io/2020/06/11/dumping-veh-win10.html
