#include <ClanLib/display.h>
#include <ClanLib/sound.h>
#include <ClanLib/gl.h>

#include "application.h"
#include "world.h"

// This is the Program class that is called by CL_ClanApplication
class Program
{
public:
	static int main(const std::vector<CL_String> &args)
	{
		// Initialize ClanLib base components
		CL_SetupCore setupCore;

		// Initialize the ClanLib display component
		CL_SetupDisplay setupDisplay;

		// Initilize the OpenGL drivers
		CL_SetupGL setupGL;

		CL_SetupSound setupSound;

		// Start the Application
		Application application;

		int returnValue = application.main(args);

		return returnValue;
	}
};

CL_ClanApplication application(&Program::main);

int Application::main(const std::vector<CL_String> &args)
{
	try
	{		
		// Create a window
		CL_DisplayWindowDescription desc;
		desc.set_title("Tower Defense");
		desc.set_allow_resize(true);
		desc.set_size(CL_Size(800, 600), true);	// Use this resolution (as caption is disabled)
		CL_DisplayWindow window(desc);

		CL_GraphicContext gc = window.get_gc();

		CL_SoundOutput output(44100);

		// Create world
		World world(window);

		// Run the main loop
		world.run();
	}
	catch(CL_Exception &exception)
	{
		// Create a console window for text-output if not available
		CL_ConsoleWindow console("Console", 80, 160);
		CL_Console::write_line("Exception caught: " + exception.get_message_and_stack_trace());
		console.display_close_message();

		return -1;
	}

	return 0;
}

