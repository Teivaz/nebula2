//------------------------------------------------------------------------------
//  ngameswfmovie.h
//  (C) 2003, 2004 George McBay, Kim, Hyoun Woo
//------------------------------------------------------------------------------
#ifndef N_GAMESWFMOVIE_H
#define N_GAMESWFMOVIE_H

//-----------------------------------------------------------------------------
/**
    @class nGameSwfMovie
    @ingroup nGameSwfGroup

    @brief
*/
#include <gameswf/gameswf.h>

#include "mathlib/matrix.h"
#include "gfx2/ngfxserver2.h"

class nGameSwfMovie : public nRoot
{
public:
    nGameSwfMovie();
    virtual ~nGameSwfMovie();

    virtual bool SaveCmds(nPersistServer* ps);

    static nKernelServer* kernelServer;

    bool CreateMovie (const char* filename);
    bool CreateLibraryMovie (const char* filename);

    virtual void SetDisplayViewport (int x, int y, int width, int height);
    
    virtual void OnMouseMove (int mouseX, int mouseY);
    virtual void OnMouseDown ();
    virtual void OnMouseUp ();
    virtual void Render ();

    int GetWidth ();
    int GetHeight ();

    void SetBackgroundAlpha (float alpha);

    gameswf::movie_definition* GetMovieDefinition();
    gameswf::movie_interface*  GetMovieInterface();

    int GetFrameCount ();
    int GetCurrentFrame ();

    gameswf::movie_interface::play_state GetPlayState ();
    void SetPlayState (gameswf::movie_interface::play_state state);

protected:
    gameswf::movie_definition* movieDefinition; //< movie definition.
    gameswf::movie_interface*  movieInterface; //< movie interface.

    int        mouse_x;             //< mouse cursor x position.
    int        mouse_y;             //< mouse cursor y position.
    int        mouse_bttn;          //< mouse button status.
    float      mouse_pos_conv_x;
    float      mouse_pos_conv_y;

    double     prevTime;            //< previous rendered time.
};

//-----------------------------------------------------------------------------
/**
    Specify alpha value of movie's background color.

    @param alpha alpha value of background color
                 0.0 : fully transparent, 1.0: opaque
*/
inline
void 
nGameSwfMovie::SetBackgroundAlpha (float alpha)
{
    n_assert (this->movieInterface != NULL);

    this->movieInterface->set_background_alpha (alpha);
}
//-----------------------------------------------------------------------------
/**
    Retrieve width of the movie.
*/
inline
int 
nGameSwfMovie::GetWidth ()
{
    n_assert (this->movieDefinition != NULL);
    return this->movieDefinition->get_width ();
}

//-----------------------------------------------------------------------------
/**
    Retrieve height of the movie.
*/
inline
int
nGameSwfMovie::GetHeight ()
{
   n_assert (this->movieDefinition != NULL);
   return this->movieDefinition->get_height ();
}

//-----------------------------------------------------------------------------
/**
    Retrieve pointer to movie definition.
*/
inline
gameswf::movie_definition* 
nGameSwfMovie::GetMovieDefinition()
{
    n_assert (this->movieDefinition);
    return this->movieDefinition;
}

//-----------------------------------------------------------------------------
/**
    Retrieve pointer to movie interface.
*/
inline
gameswf::movie_interface* 
nGameSwfMovie::GetMovieInterface()
{
    n_assert (this->movieInterface);
    return this->movieInterface;
}

//-----------------------------------------------------------------------------
/**
*/
inline
gameswf::movie_interface::play_state 
nGameSwfMovie::GetPlayState ()
{
    n_assert (this->movieInterface);
    return this->movieInterface->get_play_state ();
}

//-----------------------------------------------------------------------------
/**
*/
inline
void 
nGameSwfMovie::SetPlayState (gameswf::movie_interface::play_state state)
{
    n_assert (this->movieInterface);
    this->movieInterface->set_play_state (state);
}

#endif /*N_GAMESWFMOVIE_H*/

