# Capstone Coding Project Reflection

## What advice would you give yourself if you were to start a project like this again?

If I were to start on a project like this again, I would prioritize creating a heap allocation system early on in the project. Back-end stuff like heap management was very easy to put off doing since it was hard to debug and not very obvious when it worked; however, I encountered many situations where putting data on the heap would've been much more effective than the solution I came up with.

Additionally, I would tell myself to manage my expectations for the "nice to have" list a little more. Some parts of that list came from me not fully understanding the difficulty of interacting with hardware devices like the hard disk or USB. I did learn a whole lot from my research though, and I think I could implement a basic USB driver over the summer if I wanted to.

## Did you complete everything in your "needs to have" list?

I did complete everything in my "needs to have" list, but some parts I am a bit more proud of than others. Out of everything in my project, I am the most proud of my implementation of interrupts. This implementation took days to get right, but eventually I managed to have a working exception handler which wouldn't crash the computer every time a basic exception was encountered.

## What was the hardest part of the project?

One of the things I struggled with the most was initializing the global descriptor table and interrupt descriptor table. These two tables have very specific table entries that are hard to write to. I had to pour through a lot of documentation in order to even begin setting them up, and I spent about 10 hours debugging the tables with the only evidence of them not working being a triple fault processor dump from virtualbox. You can see in my learning log that I was very excited when I wrote my own division by zero exception handler and got the processor to call it after an actual division by zero!

## Were there any problems you could not solve?

One problem that sticks out the most was the issue of connecting with non-legacy devices through USB. In order to accomplish this, I would have to write a parser for ACPI Machine Language (AML) to locate the memory-mapped address of the xHCI USB controller. From here, I would need to write a driver that could send and recieve USB packets. This entire process made something as simple as loading files into memory basically impossible outside of the bootloader unless I wrote a very complicated driver.

## Overall Outlook

Overall, I am very glad that I decided to pursue this project, since it's something I've wanted to do for a long time. This project gave me the opportunity to take a little bootloader project I had going and turn it into a much larger and much more impressive final product.