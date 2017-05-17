# CPOX

This is a Windows 7 C++ version of the Mac OS X Python POX project.  This repo also has firmware for a gizmo with a Parallax BASIC Stamp 1 (BS1) controller.

## Installation

The project compiles in the Community edition of Visual Studio 2015 (VS 2015).  It uses the Windows pre-built OpenCV libraries:  [opencv-3.2.0-vc14.exe](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.2.0/opencv-3.2.0-vc14.exe/download).  It creates a command-line Windows executable.  I have tested it on a Windows 7 64-bit machine with Service Pack 1.

I just followed the prompts for installing VS 2015.  It is installed in stages and will prompt you to install extra components when you first try to make projects of various types.  After installing VS 2015, I extracted the OpenCV files to a folder on my machine (c:\opencv-3.2.0).  I just copied the appropriate OpenCV DLLs to wherever I had my executables.

## Camera

I tested with a Logitech c270.  It was the cheapest one I could find that I could purchase locally.  It was plug-and-play.

## Speech Synthesis and Recognition

The old Python POX project used a system call with the Mac OS X built-in "say" routine to speak canned phrases.  I was using the Python **SpeechRecognition** library with the Google API for speech recognition.  But that depends on network access and an API key that can be revoked at any time.

So I started looking at the Microsoft Speech API (SAPI).  It came with my Windows 7 computer.  I found some good C# examples for using SAPI so I decided to brush up on my C# and write a GUI wrapper around a speech synthesis and recognition server that used UDP messages for communication with a client.  It works quite well for my purposes and doesn't require a network connection.  The Logitech c270 has a microphone that was also plug-and-play with the SAPI code.  It was detected as the default audio interface.

*NOTE: Firewalls and anti-virus software can cause problems with the UDP communication and camera interface!*

