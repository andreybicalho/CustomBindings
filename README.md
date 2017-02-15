# **What is it?**
This is a simple project, build on top of the ThridPerson template, demonstrating the use of the KeyBindingUtil library for remapping game input keys.
It has a very basic UI menu, made with UMG and Blueprints, to demonstrate how to use it in your own game.

# **Details:**
* KeyBindingUtil is a C++ library, also exposed to Blueprints, that allows you to create the key rebinding/remapping system for your game.
* Usefull for creating traditional Settings/Controls menu for your game.
* Add, edit and remove keys for both Input Actions and Input Axis.

## How to use this simple project:
* Right click on CustomBindings.uproject > Generate Visual Studio Files
* Build it to make sure it works, and that's it!!

## How to use it on your game:
* If your project is purely Blueprint you may have to convert it to C++ first
* Put the "Util" directory into the "Source" directory of your project
* Compile the whole project to make sure it works
* Just call the exposed functions to add, rebind or remove your game key bindings.
