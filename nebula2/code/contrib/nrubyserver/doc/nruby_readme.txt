Read license.txt for licensing terms.


nruby version 0.5 06-FEB-2004
------------------------------
This is a script server for the freely available and easy to learn ruby scripting language. It is the NEBULA 2 release for NEBULA 1 releases take a look at www.3d-inferno.com.
See www.ruby-lang.org for further language features. This release only has core ruby
features without any extension libraries. This is an example with the 1.8.0 version
of ruby. For ruby infos have a look at www.ruby-lang.org

compilation
------------------------------
You will need ruby 1.8; make sure the ruby dll is in your path and that your compiler can
find the ruby includes and libs.

Run the update.tcl from your nebula 2 directory. Open the newly created nrubyserver workspace/solution and compile the nrubydemo. If you have any trouble you can contact me at tom@3d-inferno.com. 

test
------------------------------
You can run code/contrib/nrubyserver/bin/win32d/nruby_demo.bat to see the torus rotating. For further ruby script techniques have a look at the code/contrib/nrubyserver/export/gfxlib or www.ruby-lang.org. Native nebula scripts can be easily converted to ruby. Just compare the one presented with the regular nebula 2 scripts.
If you open the console you will notice a difference. Just type dir and you will notice that no output is printed. Up to now this is expected behavior. Ruby returns its objects and does not normaly print its result as a string. You can easily addapt the code if you would like to change this. Or place a puts before the command you'd like printed. Another thing are the object commands. Calling commands from an object does not need a . in ruby. If you selected '/usr/scene' and you'd like to know the commands this node has just call getcmds and not as in tcl .getcmds.

commands
------------------------------
include Nebula	
	Placed at the end of a file tells ruby you will be interfacing with nebula in your ruby scripts.
dir
	Returns an array of objects below the actual one.
new parm1, parm2
	Creates a new object in nebula. Parm1 is the objecttype, parm2 is the place in the hierarchy the new object will be placed at.
delete parm1
	Deletes all objects starting at parm1.
puts OUTPUT
	Prints the desired output to the console.
sel parm1
	Selects the desired node (parm1)
psel
	Returns the current working object

This should be all you need...

If you need the unix  or macos libraries of ruby go to www.ruby-lang.org
Have fun.
--
