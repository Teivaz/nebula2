using System;
using Nebula2NET;

namespace Hello
{
	class Hello
	{
		static void Main(string[] args)
		{
			// Note: a NOH name can only be set via ctor!
			// Note: The ctor takes a NOH name as a parameter.
			// You can use either a full path, starting with '/'
			// or a relative path, relative to nebula's cwd!
			// Note: The ctor only creates a new nebula(c++) object
			// if it does not already exist in the nebula NOH, otherwise
			// the ctor returns the existing nebula(c++) object!
			// Note: The ctor throws an exception if the type of the 
			// nebula(c#.net) object does not match the type of 
			// the existing nebula(c++) object!
			nroot foo = new nroot("/");
				
			Console.Out.WriteLine(foo.getclass());
	        
			ntimeserver timeserver = new ntimeserver("/sys/servers/time");
			Console.Out.WriteLine(timeserver.gettime());

			//create some nroot objects
			nroot bar = new nroot("/hallo/spencer");
			bar = new nroot("/hallo/lory");
			bar = new nroot("hallo");

			// This is really an array of nroot objects
			// Note: you could replace the next code line with:
			// nroot[] children = (nroot[])bar.getchildren();
			object[] children = bar.getchildren();
			
			if(children.Length > 0)
				Console.Out.WriteLine("Children:");
	
			foreach(nroot child in children)
				Console.Out.WriteLine(child.getname());
			
			if(foo.isa("nroot"))
				Console.Out.WriteLine("foo is an nroot!");
			
			if(!foo.isa("ntimeserver"))
				Console.Out.WriteLine("foo is not a timeserver!");
		}
	}
}
