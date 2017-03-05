# CPOX

This is a C++ version of the Python POX project.  This repo also has firmware for a gizmo with a Parallax BASIC Stamp 1 (BS1) controller.

## Installation

The project compiles in the Community edition of Visual Studio 2015 (VS 2015).  It uses the Windows pre-built OpenCV libraries:  [opencv-3.2.0-vc14.exe](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/3.2.0/opencv-3.2.0-vc14.exe/download).  It creates a command-line Windows executable.  I have tested it on a Windows 7 64-bit machine with Service Pack 1.

I just followed the prompts for installing VS 2015.  It is installed in stages and will prompt you to install extra components when you first try to make projects of various types.  After installing VS 2015, I extracted the OpenCV files to a folder on my machine (c:\opencv-3.2.0).  I just copied the appropriate OpenCV DLLs to wherever I had my executables.

## Camera

I tested with a Logitech c270.  It was the cheapest one I could find that I could purchase locally.  It was plug-and-play.



