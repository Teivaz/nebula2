#define _BOOL
#define REQUIRE_IOSTREAM
#include <maya/MLibrary.h>
#include <maya/MFileIO.h>
#include <maya/MGlobal.h>
#include <util/npathstring.h>
#include <cassert>

class Options
{
public:
	Options();

	inline bool exportScriptP() const { return _szScriptFilename != ""; }
	inline bool exportAnimP() const { return _szAnimKeysFilename != ""; }
	inline bool exportMeshP() const { return _szMeshFilename != ""; }
    inline bool hasSizeMult() const { return _szSizeMult != ""; }
	inline bool verboseP() const { return _bVerbose; }
	inline bool analyzeP() const { return _bAnalyze; }

	inline MString mayaInputFilename() const { return _szMayaInputFilename; }
	inline nPathString animKeysFilename() const { return _szAnimKeysFilename; }
	inline nPathString meshFilename() const { return _szMeshFilename; }
	inline nPathString scriptFilename() const { return _szScriptFilename; }
    inline MString getSizeMult() const { return _szSizeMult; }
	inline uint animStep() const { return _nKeyEveryNFrames; }
	inline int startFrame() const { return _nStartFrame; } // 0 is the beginning, negative values are relative to endFrame
	inline int endFrame() const { return _nEndFrame; } // 0 is the end, negative values are relative to the end of the animation

	void parse( int argc, char* argv[] );
	void usage() const;

	MString _szMayaInputFilename;
    MString _szSizeMult;
	nPathString _szScriptFilename; // Main script (.n2) file name
	nPathString _szMeshFilename; // .n3d2 file name
	nPathString _szAnimKeysFilename; // .nanim2

private:
	// Periodicity of the keys in exported Nebula animation as compared to Maya animation
	uint _nKeyEveryNFrames;
	int _nStartFrame;
	int _nEndFrame;

	bool _bAnalyze;
	bool _bVerbose;

	bool matchSwitchP( const char* szSwitch, const char* szProgSwitch ) const;
	bool matchSwitchP( const char* szSwitch, const char* szProgSwitch, const char* & szArg ) const;
};
