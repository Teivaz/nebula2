//------------------------------------------------------------------------------
//  ngameswfmovie.cc
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "ngameswf/ngameswfmovie.h"
#include "kernel/ntimeserver.h"

nNebulaScriptClass(nGameSwfMovie, "nroot");

//-----------------------------------------------------------------------------
/**
    Constructor.
*/
nGameSwfMovie::nGameSwfMovie() :
    movieDefinition(NULL),
    movieInterface (NULL),
    mouse_x (0),
    mouse_y (0),
    mouse_bttn (0),
    mouse_pos_conv_x (0.0f),
    mouse_pos_conv_y (0.0f),
    prevTime (0.0)
{
}

//-----------------------------------------------------------------------------
/**
    Destructor.
*/
nGameSwfMovie::~nGameSwfMovie()
{
    // drop_ref() calls delete internally.
    if (this->movieDefinition != NULL)
    {
        this->movieDefinition->drop_ref ();
    }
    if (this->movieInterface != NULL)
    {
        this->movieInterface->drop_ref ();
    }
}

//-----------------------------------------------------------------------------
/**
    Load .swf file to create movie definition and then create movie interface 
    for the created movie definition.
    <br>
	Normally, will also try to load any cached data file (".gsc") that 
    corresponds to the given movie file.  This will still work even if there 
    is no cache file. You can	disable the attempts to load cache files by 
    calling	gameswf::use_cache_files(false).

	Uses the registered file-opener callback to read the files themselves.
	
	This calls add_ref() on the newly created definition; call drop_ref() when 
    you're done with it.

    @param filename .swf filename.
    @return true if loading is success.
*/
bool nGameSwfMovie::CreateMovie (const char* filename)
{
    n_assert (this->movieDefinition == NULL);
    n_assert (this->movieInterface == NULL);

    // Create a gameswf::movie_definition from the given file name.
    this->movieDefinition = gameswf::create_movie(filename);

    if (!this->movieDefinition)
    {
        n_printf ("nGameSwfMovie::Load: Error, unable to load file '%s'\n", filename);
        return false;
    }

    this->movieInterface = movieDefinition->create_instance();
    if (!this->movieInterface)
    {
        n_printf ("GameSwfMovie::Load: Error, unable to instance SWF '%s'\n", filename);
        return false;
    }

    //this->SetBackgroundAlpha(1.0f);

    return true;
}

//-----------------------------------------------------------------------------
/**
    This is just like create_movie(), except that it checks the	"library" to 
    see if a movie of this name has already been created, and returns that 
    movie if so.  Also, if it creates a new movie, it adds it back into the library.
	
	The "library" is used when importing symbols from external movies, so this 
    call might be useful if you want to	explicitly load a movie that you know 
    exports symbols (e.g. fonts) to other movies as well.

    @param filename .swf filename.
    @return true if loading is success.
*/
bool nGameSwfMovie::CreateLibraryMovie (const char* filename)
{
    n_assert (this->movieDefinition == NULL);
    n_assert (this->movieInterface == NULL);
    
    // Create a gameswf::movie_definition from the given file name.
    movieDefinition = gameswf::create_library_movie(filename);

    if (!this->movieDefinition)
    {
        n_printf ("nGameSwfMovie::Load: Error, unable to load file '%s'\n", filename);
        return false;
    }

    this->movieInterface = movieDefinition->create_instance();
    if (!this->movieInterface)
    {
        n_printf ("GameSwfMovie::Load: Error, unable to instance SWF '%s'\n", filename);
        return false;
    }

    //this->SetBackgroundAlpha(1.0f);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Specify position and size of the swf movie.

    @param x      x position where movie to put in screen coordinate.
    @param y      y position where movie to put in screen coordinate.
    @param width  Width of swf movie file
    @param height Height of swf movie file.
*/
void nGameSwfMovie::SetDisplayViewport (int x, int y, int width, int height)
{
    n_assert (this->movieInterface != NULL);

    mouse_pos_conv_x = movieDefinition->get_width () / (float)width;
    mouse_pos_conv_y = movieDefinition->get_height () / (float)height;

    this->movieInterface->set_display_viewport (x, y, width, height);
}

//-----------------------------------------------------------------------------
/**
    Notify mouse movement event to gameswf.
*/
void nGameSwfMovie::OnMouseMove (int mouseX, int mouseY)
{
    n_assert (this->movieInterface != NULL);

    this->mouse_x = (int)(mouseX * this->mouse_pos_conv_x);
    this->mouse_y = (int)(mouseY * this->mouse_pos_conv_y);

    this->movieInterface->notify_mouse_state ((int)mouseX, (int)mouseY, this->mouse_bttn);
}

//-----------------------------------------------------------------------------
/**
    Notify mouse down event to gameswf.
*/
void nGameSwfMovie::OnMouseDown ()
{
    n_assert (this->movieInterface != NULL);

    this->mouse_bttn = 2;

    this->movieInterface->notify_mouse_state (this->mouse_x, this->mouse_y, this->mouse_bttn);
}

//-----------------------------------------------------------------------------
/**
    Notify mouse up event to gameswf.
*/
void nGameSwfMovie::OnMouseUp ()
{
    n_assert (this->movieInterface != NULL);

    this->mouse_bttn = 0;

    this->movieInterface->notify_mouse_state (this->mouse_x, this->mouse_y, this->mouse_bttn);
}

//-----------------------------------------------------------------------------
/**
    Render movie with given time.
*/
void nGameSwfMovie::Render ()
{
    n_assert (this->movieInterface != NULL);

    double curTime = kernelServer->GetTimeServer ()->GetFrameTime ();
    movieInterface->advance ((float)(curTime - prevTime));

    this->prevTime = curTime;

    movieInterface->display ();
}



