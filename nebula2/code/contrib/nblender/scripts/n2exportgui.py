#!BPY

# """
# Name: 'Nebula Exporter(.n2)'
# Blender: 241
# Group: 'Export'
# Tooltip: 'Export to Nebula(.n2)'
# """

__author__ = 'Larry Weya'
__email__ = ["Larry Weya", "larry*create-101.com"]
__url__ = ["Create-101", "http://www.create-101.com.com", "blender", 
	"elysiun"]
__version__ = '0.01'
__bpydoc__ = """\

Nebula 2 Exporter

"""

import nblender as n2, n2export, Blender, os
from Blender import Scene, Registry, Draw, Window, BGL

persist_servers = ["ntclserver", "nbinscriptserver", "npythonserver", "nluaserver", "nrubyserver"]
mesh_formats = ["n3d2", "nvx2"]
renderpaths = ["dx7", "dx9", "dx9flt"]
aasamples = range(2, 17)
aasamples.insert(0, 0)

def DoExport():
	persist = n2.lookup("/sys/servers/persist")
	persist.setsaverclass(str(persist_servers[persist_server]))
		
	e = n2export.n2exporter()
	e.group = group
	e.mesh_format = str(mesh_formats[mesh_format])
	e.filename = filename
			
	if(e.ExportScene()):
		e.SaveScene()
	e.CleanUp()
	
	fullscreen_arg = ""
	if(fullscreen):
		fullscreen_arg = "-fullscreen true"
		
	os.spawnl(os.P_NOWAIT, "%s/nviewer.exe" % (file2.manglepath("bin:")), "program name", "-projdir %s" % n2.lookup("/sys/servers/file2").getassign("proj"), "-view gfxlib:%s/%s.n2" % (group, e.filename), "-eyeposx 0", "-eysposy 2", "-eyeposz -20", "-eyecoix 0", "-eyecoiy 0", "-eyecoiz 0", "-w %i" % window_width, "-h %i" % window_height, "-featureset %s" % str(renderpaths[renderpath]), "-aa %s" % aasamples[aa], fullscreen_arg)

def GetPrefDefaults():
	export_config = {}
	export_config["persist_server"] = 0
	export_config["mesh_format"] = 0
	export_config["window_width"] = 800
	export_config["window_height"] = 600
	export_config["fullscreen"] = 0
	export_config["renderpath"] = 0
	export_config["group"] = ""
	export_config["aa"] = 0
	return export_config

def SavePrefs():
	export_config["persist_server"] = persist_server
	export_config["mesh_format"] = mesh_format
	export_config["window_width"] = window_width
	export_config["window_height"] = window_height
	export_config["fullscreen"] = fullscreen
	export_config["renderpath"] = renderpath
	export_config["group"] = group
	export_config["aa"] = aa
	Registry.SetKey("n2exportconfig", export_config, True)

def buildMenu(items, start_at=0):
	index = start_at
	num_items = len(items)
	menu = ""
	for item in items:
		menu = menu + str(item)
		menu = menu + ' %x' + str(index)
		index = index + 1
		menu = menu + '|'
	return menu

def draw_event(evt, val):
	if(evt == Draw.ESCKEY):
		SavePrefs()
		Draw.Exit()
		return
	
def button_event(evt):
	global persist_server, mesh_format, window_width, window_height, fullscreen, renderpath, filename, group, aa

	if(evt == CANCEL_CLICKED):
		SavePrefs()
		Draw.Exit()
		return
	
	if(evt == PERSIST_SERVER_CHANGED):
		persist_server = mnu_persist_servers.val
		
	if(evt == MESH_FORMAT_CHANGED):
		mesh_format = mnu_mesh_formats.val
		
	if(evt == WIDTH_CHANGED):
		window_width = mnu_width.val
	
	if(evt == HEIGHT_CHANGED):
		window_height = mnu_height.val
		
	if(evt == FULLSCREEN_CHANGED):
		fullscreen = 1 - fullscreen
		
	if(evt == AA_CHANGED):
		aa = mnu_aa.val
		
	if(evt == FILENAME_CHANGED):
		filename = mnu_filename.val
	
	if(evt == GROUP_CHANGED):
		file2 = n2.lookup("/sys/servers/file2")
		path = ""
		is_valid = True
		if(mnu_group.val):
			path = file2.manglepath("gfxlib:") + "/" + mnu_group.val
		else:
			path = file2.manglepath("gfxlib:")
		if(not Blender.sys.exists(path)):
			is_valid = False
		path = ""
		if(mnu_group.val):
			path = file2.manglepath("meshes:") + "/" + mnu_group.val
		else:
			path = file2.manglepath("meshes:")
		if(not Blender.sys.exists(path)):
			is_valid = False
		if(is_valid):
			group = mnu_group.val
		else:
			err = "The directory ' %s ' must exist in both gfxlib and meshes folders" % mnu_group.val
			Draw.PupMenu("Invalid Group%t|" + err)
			Draw.Redraw(1)
	
	if(evt == RENDERPATH_CHANGED):
		renderpath = mnu_renderpath.val
	
	if(evt == OK_CLICKED):
		SavePrefs()
		DoExport()
		
def draw():
	global mnu_persist_servers, mnu_mesh_formats, mnu_width, mnu_height, mnu_fullscreen, mnu_renderpath, mnu_group, mnu_filename, mnu_aa
	
	win_size = Window.GetAreaSize()
	left = 0
	top = win_size[1]
	
	button_width = 130
	button_height = 20
	label_width = 105
	pen_x = left + padding
	pen_y = win_size[1] - (button_height + padding)
		
	# Draw Label
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	text_width = Draw.Text('Persist Server')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	
	# Draw Persist Server Menu
	mnu_persist_servers = Draw.Menu(buildMenu(persist_servers), PERSIST_SERVER_CHANGED, pen_x, pen_y, button_width, button_height, persist_server, "Persist Server to use")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 180
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	Draw.Text('Mesh Format')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 80
	
	# Draw Mesh Format Menu
	mnu_mesh_formats = Draw.Menu(buildMenu(mesh_formats), MESH_FORMAT_CHANGED, pen_x, pen_y, button_width, button_height, mesh_format, "Mesh Format to use(ASCII/Binary)")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 180
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	text_width = Draw.Text('Preview Window')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 100
	
	mnu_width = Draw.Number("Width", WIDTH_CHANGED, pen_x, pen_y, button_width, button_height, window_width, 32, 2048, "Preview Window Width")
	
	# Move Pen Point
	pen_x = pen_x + button_width + padding
	
	mnu_height = Draw.Number("Height", HEIGHT_CHANGED, pen_x, pen_y, button_width, button_height, window_height, 32, 2048, "Preview Window Height")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	text_width = Draw.Text('Preview Fullscreen')
	button_width = 20
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	
	mnu_fullscreen = Draw.Toggle("X", FULLSCREEN_CHANGED, pen_x, pen_y, button_width, button_height, fullscreen, "Preview Fullscreen")
	
	# Move Pen Point
	pen_x = pen_x + button_width + padding
	pen_y = pen_y + 5
	button_width = 180
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	Draw.Text('Antialias Samples')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 40
	
	# Draw Mesh Format Menu
	mnu_aa = Draw.Menu(buildMenu(aasamples), AA_CHANGED, pen_x, pen_y, button_width, button_height, aa, "Antialias Samples")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 180
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	text_width = Draw.Text('Export Group / filename')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 80
	
	mnu_group = Draw.String("", GROUP_CHANGED, pen_x, pen_y, button_width, button_height, group, 255, "Export Group")
	
	# Move Pen Point
	pen_x = pen_x + button_width + padding
	
	mnu_filename = Draw.String("", FILENAME_CHANGED, pen_x, pen_y, button_width, button_height, filename, 255, "Export Filename")
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 100
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	Draw.Text('Preview Renderpath')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	button_width = 60
	
	mnu_renderpath = Draw.Menu(buildMenu(renderpaths), RENDERPATH_CHANGED, pen_x, pen_y, button_width, button_height, renderpath, "Renderpath to use for preview")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 80
	
	Draw.PushButton("Close", CANCEL_CLICKED, pen_x, pen_y, button_width, button_height, "Close Exporter")
	
	pen_x = pen_x + button_width + padding
	Draw.PushButton("Export", OK_CLICKED, pen_x, pen_y, button_width, button_height, "Export Scene")
	

file2 = n2.lookup("/sys/servers/file2")

reg_name = "n2exportconfig"			
export_config = Registry.GetKey(reg_name, True)

if(not export_config):
	export_config = GetPrefDefaults()

# Get Window width and height
win_size = Window.GetAreaSize()
left = 0
top = win_size[1]

# Space to leave between buttons
padding = 10

# Buttons
mnu_persist_servers = None
mnu_mesh_formats = None
mnu_width = None
mnu_height = None
mnu_fullscreen = None
mnu_renderpath = None
mnu_group = None
mnu_filename = None
mnu_aa = None

# Button Defaults
persist_server = export_config["persist_server"]
mesh_format = export_config["mesh_format"]
window_width = export_config["window_width"]
window_height = export_config["window_height"]
fullscreen = export_config["fullscreen"]
renderpath = export_config["renderpath"]
group = export_config["group"]
aa = export_config["aa"]

filename = Blender.sys.basename(Blender.Get("filename"))
filename = Blender.sys.splitext(filename)
filename = filename[0]


# Button Events
PERSIST_SERVER_CHANGED = 1
MESH_FORMAT_CHANGED = 2
WIDTH_CHANGED = 3
HEIGHT_CHANGED = 4
FULLSCREEN_CHANGED = 5
RENDERPATH_CHANGED = 6
CANCEL_CLICKED = 7
OK_CLICKED = 8
GROUP_CHANGED = 9
FILENAME_CHANGED = 10
AA_CHANGED = 11

reg_name = "n2pathconfig"
path_config = Registry.GetKey(reg_name, True)

if(not path_config):
	Draw.PupMenu("Config Error|Go to Misc>Nebula Config")
else:
	file2.setassign("home", str(path_config["home"]))
	file2.setassign("proj", str(path_config["proj"]))
	file2.setassign("bin", str(path_config["bin"]))
	file2.setassign("export", str(path_config["export"]))
	file2.setassign("anims", str(path_config["anims"]))
	file2.setassign("gfxlib", str(path_config["gfxlib"]))
	file2.setassign("meshes", str(path_config["meshes"]))
	file2.setassign("textures", str(path_config["textures"]))
	if(n2export.DoStartupChecks()):
		n2export.Init()
		n2export.BuildLookupTable()
		Draw.Register(draw, draw_event, button_event)
	else:
		print "Config Error|Go to Misc>Nebula Config"
		Draw.PupMenu("Startup Checks Failed%t|Startup Cheks Failed, check console")