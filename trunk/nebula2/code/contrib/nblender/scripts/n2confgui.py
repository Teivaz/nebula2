#!BPY

# """
# Name: 'Nebula Config'
# Blender: 241
# Group: 'Misc'
# Tooltip: 'Configure Nebula Path Assigns'
# """

__author__ = 'Larry Weya'
__email__ = ["Larry Weya", "larry*create-101.com"]
__url__ = ["Create-101", "http://www.create-101.com.com", "blender", 
	"elysiun"]
__version__ = '0.01'
__bpydoc__ = """\

GUI to configure the Nebula exporter and Mateial Editor. Select any file in the
nebula directory.

"""

import Blender, nblender as n2, os.path, string
from Blender import BGL, Draw, Registry, Image, Window

# Order of the assigns in the window
assigns = ["home", "proj", "bin", "export", "anims", "gfxlib", "meshes", "textures"]

def SetDir(assign, filenam):
	global config_data
	config_data[assign] = Blender.sys.expandpath(Blender.sys.dirname(filenam))
	
def SetHomeDir(filename):
	SetDir("home", filename)
	
def SetProjDir(filename):
	SetDir("proj", filename)
	
def getDefaults():
	data = {}
	data["home"] = "C:\\"
	data["proj"] = "C:\\"
	data["bin"] = "home:bin/win32"
	data["export"] = "proj:export"
	data["anims"] = "export:anims"
	data["gfxlib"] = "export:gfxlib"
	data["meshes"] = "export:meshes"
	data["textures"] = "export:textures"
	return data

def SaveConfig():
	global config_data
	file2 = n2.lookup("/sys/servers/file2")
	#file2.setassign("home", str(config_data["home"]))
	#file2.setassign("proj", str(config_data["proj"]))
	for data in config_data:
			#if(data != "home"):
			data = str(data)
			assign = config_data[data]
			file2.setassign(data, assign)
	texture_dir = file2.manglepath("textures:")
	texture_dir = os.path.abspath(texture_dir)
	texture_dir = string.replace(texture_dir, "\\", "/")
	config_data["texture_dir"] = texture_dir
	Registry.SetKey(reg_name, config_data, True)

def draw_event(evt, val):
	if(evt == Draw.ESCKEY):
		Draw.Exit()
		return
	
def button_event(evt):
	global config_data
	
	if(evt == BROWSE_HOME_CLICKED):
		default_path = "C:\\"
		if(Blender.sys.exists(config_data["home"])):
			default_path = config_data["home"]
		Window.FileSelector(SetHomeDir, "Nebula Home", default_path)
	if(evt == BROWSE_PROJ_CLICKED):
		default_path = "C:\\"
		if(Blender.sys.exists(config_data["home"])):
			default_path = config_data["proj"]
		Window.FileSelector(SetProjDir, "Project Dir", default_path)
	
	if(evt == ASSIGN_DIR_CHANGED):
		for data in config_data:
			if(data != "home" and data != "texture_dir" and data != "proj"):
				config_data[data] = dir_assigns[data].val
	
	if(evt == OK_CLICKED):
		SaveConfig()
		result = Draw.PupMenu("Saved%t|Do you want to exit?")
		if(result != -1):
			Draw.Exit()
			return
	
	if(evt == CANCEL_CLICKED):
		result = Draw.PupMenu("Exit%t|Exit and Discard Changes?")
		if(result != -1):
			Draw.Exit()
			return
		
def draw():
	win_size = Window.GetAreaSize()
	left = 0
	top = win_size[1]
	
	button_width = 180
	button_height = 20
	label_width = 105
	pen_x = left + padding
	pen_y = win_size[1] - (button_height + padding)
		
	# Draw Label
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	text_width = Draw.Text('Nebula Home Dir')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	
	# Draw String Button
	home_dir_input = Draw.String("", HOME_DIR_CHANGED, pen_x, pen_y, button_width, button_height, config_data["home"], 255, "Nebula's Home Directory")
	pen_x = pen_x + button_width
	button_width = 20
	Draw.PushButton("...", BROWSE_HOME_CLICKED, pen_x, pen_y, button_width, button_height, "Browse for Nebula Home Directory")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 180
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	Draw.Text('Project Dir')
	
	# Move Pen Point
	pen_x = pen_x + text_width + padding
	pen_y = pen_y - 5
	
	# Draw String Button
	proj_dir_input = Draw.String("", PROJ_DIR_CHANGED, pen_x, pen_y, button_width, button_height, config_data["proj"], 255, "Projetct Directory")
	pen_x = pen_x + button_width
	button_width = 20
	Draw.PushButton("...", BROWSE_PROJ_CLICKED, pen_x, pen_y, button_width, button_height, "Browse for Project Directory")
	
	# Move Pen Point
	pen_x = left + padding
	pen_y = pen_y - button_height - 5
	button_width = 150
	BGL.glColor3f(1.0, 1.0, 1.0)
	BGL.glRasterPos2i(pen_x, pen_y)
	Draw.Text('Export Dir')
	
	# Draw String Button
	pen_x = pen_x + label_width
	pen_y = pen_y - 5
	dir_assigns["export"] = Draw.String("", ASSIGN_DIR_CHANGED, pen_x, pen_y, button_width, button_height, config_data["export"], 255, "Export Directory")
	pen_y = pen_y + 5
	
	for data in config_data:
		if(data != "home" and data != "export" and data != "texture_dir" and data != "proj"):
			# Move Pen Point
			pen_x = left + padding
			pen_y = pen_y - button_height - 5
			# Draw Label
			BGL.glColor3f(1.0, 1.0, 1.0)
			BGL.glRasterPos2i(pen_x, pen_y)
			text_width = Draw.Text(data)
			# Draw Button
			pen_x = pen_x + label_width
			pen_y = pen_y - 5
			dir_assigns[data] = Draw.String("", ASSIGN_DIR_CHANGED, pen_x, pen_y, button_width, button_height, config_data[data], 255, "Assign")
			pen_y = pen_y + 5
	# Draw OK Cancel Buttons
	button_width = 100
	
	pen_x = left + padding
	pen_y = pen_y - button_height - 5 - padding
	Draw.PushButton("Cancel", CANCEL_CLICKED, pen_x, pen_y, button_width, button_height, "Cancel Configuration")
	
	pen_x = pen_x + button_width + padding
	Draw.PushButton("OK", OK_CLICKED, pen_x, pen_y, button_width, button_height, "Save Configuration")
	
############################################################################
#                          Main Program starts here                        #
############################################################################

n2logo = 0
logo_size = [0, 0]

reg_name = "n2pathconfig"

config_data = Registry.GetKey(reg_name, True)
if(not config_data):
	config_data = getDefaults()
	
# Get Window width and height
win_size = Window.GetAreaSize()
left = 0
top = win_size[1]

# Space to leave between buttons
padding = 10

home_dir_input = 0
proj_dir_input = 0
dir_assigns = {}

HOME_DIR_CHANGED = 1
PROJ_DIR_CHANGED = 2
BROWSE_HOME_CLICKED = 3
BROWSE_PROJ_CLICKED = 4
ASSIGN_DIR_CHANGED = 5
CANCEL_CLICKED = 6
OK_CLICKED = 7

Draw.Register(draw, draw_event, button_event)
	