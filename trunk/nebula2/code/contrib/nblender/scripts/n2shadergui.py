#!BPY

# """
# Name: 'Nebula Material Editor'
# Blender: 241
# Group: 'Materials'
# Tooltip: 'Nebula Material Editor'
# """

__author__ = 'Larry Weya'
__email__ = ["Larry Weya", "larry*create-101.com"]
__url__ = ["Create-101", "http://www.create-101.com", "blender", "elysiun"]
__version__ = '1.1'
__bpydoc__ = """\

Nebula Material Exporter. The exporter creates a scene - nebula_materials where
it stores empties for the scripts use.

"""

import Blender, string
from Blender import Scene, BGL, Draw, Registry, Image, Window, Object, Material, Mesh, sys
import n2export as n2exporter
import nblender as n2

def buildMenu(items, start_at=0):
	index = start_at
	num_items = len(items)
	menu = ""
	for item in items:
		menu = menu + item
		menu = menu + ' %x' + str(index)
		index = index + 1
		menu = menu + '|'
	return menu

def draw_event(evt, val):
	if(evt == Draw.ESCKEY):
		# TODO: Save changes to current material
		Draw.Exit()
		return
	Draw.Redraw(1)

def chooseTexture(filename):
	global n2mat
	texture_param_name = texture_param_list[texture_param_index]
	n2mat.SetParamValue(texture_param_name, filename)

def button_event(evt):
	global n2mat, bl_mat_index, n2_shader_index, texture_param_index
	
	if(evt == BL_MAT_CHANGED):
		bl_mat_index = bl_mats_menu.val
		n2mat = n2exporter.n2Material(bl_mats[bl_mat_index])
		n2_shader_index = n2_shaders.index(n2mat.GetShader().m_name)
		
	if(evt == NEBULA_SHADER_CHANGED):
		n2_shader_index = n2_shader_menu.val
		n2mat.CreateShaderFromName(n2_shaders[n2_shader_index])
		n2mat.SetDefaults()
		
	if(evt == SHADER_PARAM_CHANGED):
		params = n2mat.GetShader().m_params
		for key in params:
			param = params[key]
			if(not n2exporter.texture_param_types.has_key(param.m_type) and (not n2exporter.misc_param_types.has_key(param.m_type))):
				val = shader_params_menu[param.m_name].val
				n2mat.SetParamValue(param.m_name, val)
			elif(param.m_type == "Vector"):
				x = shader_params_menu[param.m_name + "_x"].val
				y = shader_params_menu[param.m_name + "_y"].val
				z = shader_params_menu[param.m_name + "_z"].val
				w = shader_params_menu[param.m_name + "_w"].val
				v = n2exporter.nVector(x, y, z, w)
				n2mat.SetParamValue(param.m_name, n2exporter.VectorToString(v))
	
	if(evt == TEXTURE_PARAM_CHANGED):
		texture_param_index = shader_params_menu["TextureParams"].val
		
	if(evt == TEXTURE_SELECT_CLICKED):
		Blender.Window.FileSelector(chooseTexture, "Choose Texture", n2.lookup("/sys/servers/file2").manglepath("textures:"))
		
	Draw.Redraw(1)
		
		
def draw():
	global n2mat, bl_mats_menu, n2_shader_menu, shader_params_menu, texture_param_list
	
	win_size = Window.GetAreaSize()
	left = 0
	top = win_size[1]
	
	button_width = 100
	button_height = 17
	pen_x = left + padding
	pen_y = win_size[1] - (button_height + padding)
		
	# TODO: get material for selected object if any				
	
	if(n2mat):
		# Draw Label
		BGL.glColor3f(1.0, 1.0, 1.0)
		BGL.glRasterPos2i(pen_x, pen_y)
		text_width = Draw.Text('Blender Material')
		
		# Move Pen Point
		pen_x = pen_x + text_width + padding
		pen_y = pen_y - 5
		#
		menu = buildMenu(bl_mats)
		bl_mats_menu = Draw.Menu(menu, BL_MAT_CHANGED, pen_x, pen_y, button_width, button_height, bl_mat_index, 'Blender Material')
		
		# Move Pen Point
		pen_x = pen_x + button_width + (padding * 2)
		pen_y = pen_y + 5
		BGL.glRasterPos2i(pen_x, pen_y)

		# Draw Label
		text_width = Draw.Text('Nebula Shader')
		
		# Move Pen Point
		pen_x = pen_x + text_width + (padding)
		pen_y = pen_y - 5
		button_width = 180
		#
		menu = buildMenu(n2_shaders)
		n2_shader_menu = Draw.Menu(menu, NEBULA_SHADER_CHANGED, pen_x, pen_y, button_width, button_height, n2_shader_index, 'Nebula Shader')
		
		# Draw Params for current nebula shader
		shader = n2mat.GetShader()
		# Move Pen
		params_start_y = pen_y
		pen_x = left + padding
		params = shader.m_params
		label_width = 130
		label_max_chars = 25
		button_width_max = 80
		name_max_chars = 10
		column_size = label_width + (padding *2) + button_width_max
		# Get Mesh object holding the shaders name and params
		texture_param_list = []
		button_width = 120
		#params = []
		for key in params:
			param = params[key]
			if(not n2exporter.texture_param_types.has_key(param.m_type)):
				if((pen_y - button_height - padding) < button_height):
					pen_x = pen_x + column_size#(win_size[0] / 2)
					pen_y = params_start_y
				pen_y = pen_y - button_height - padding
				BGL.glRasterPos2i(pen_x, pen_y)
				label = param.m_label
				name = param.m_name
				stored_value = n2mat.GetParamValue(param.m_name)
				if(len(label)>label_max_chars):
					label = label[0:label_max_chars]
				if(len(name)>name_max_chars):
					name = name[0:name_max_chars]
				Draw.Text(str(label))
			if(param.m_type == "Int"):
				button_width = 50
				pen_x = pen_x + label_width + padding
				pen_y = pen_y - 5
				shader_params_menu[param.m_name] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, int(stored_value), int(param.m_min), int(param.m_max), str(param.m_label))
				pen_x = pen_x - label_width - padding
				pen_y = pen_y + 5
			elif(param.m_type == "Bool"):
				button_width = 55
				pen_x = pen_x + label_width + padding
				pen_y = pen_y - 5
				menu = buildMenu(["False", "True"])
				shader_params_menu[param.m_name] = Draw.Menu(menu, SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, int(stored_value), str(param.m_label))
				pen_x = pen_x - label_width - padding
				pen_y = pen_y + 5
			elif(param.m_type == "Float"):
				button_width = 60
				pen_x = pen_x + label_width + padding
				pen_y = pen_y - 5
				shader_params_menu[param.m_name] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, float(stored_value), float(param.m_min), float(param.m_max), str(param.m_label))
				pen_x = pen_x - label_width - padding
				pen_y = pen_y + 5
			elif(param.m_type == "Enum"):
				button_width = 80
				pen_x = pen_x + label_width + padding
				pen_y = pen_y - 5
				menu = buildMenu(param.m_enum, param.m_default_enum)
				shader_params_menu[param.m_name] = Draw.Menu(menu, SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, int(stored_value), str(param.m_label))
				pen_x = pen_x - label_width - padding
				pen_y = pen_y + 5
			elif(param.m_type == "Vector"):
				button_width = 50
				pen_y = pen_y - button_height - 5
				v = n2exporter.StringToVector(stored_value)
				shader_params_menu[param.m_name + "_x"] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, v.x, 0.0, 100.0, "x Component")
				pen_x = pen_x + button_width
				shader_params_menu[param.m_name + "_y"] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, v.y, 0.0, 100.0, "y Component")
				pen_x = pen_x + button_width
				shader_params_menu[param.m_name + "_z"] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, v.z, 0.0, 100.0, "z Component")
				pen_x = pen_x + button_width
				shader_params_menu[param.m_name + "_w"] = Draw.Number("", SHADER_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, v.w, 0.0, 100.0, "w Component")
				pen_x = pen_x - ((button_width) * 3)
			elif(n2exporter.texture_param_types.has_key(param.m_type)):
				texture_param_list.append(param.m_name)
		# Build textures menu
		if(len(texture_param_list)>0):
			if((pen_y - button_height - padding) < button_height):
				pen_x = pen_x = pen_x + column_size
				pen_y = params_start_y
			button_width = 80
			pen_y = pen_y - button_height - padding - 5
			#
			texture_param_name = texture_param_list[texture_param_index]
			menu = buildMenu(texture_param_list)
			shader_params_menu["TextureParams"] = Draw.Menu(menu, TEXTURE_PARAM_CHANGED, pen_x, pen_y, button_width, button_height, texture_param_index, "Shader Textures")
			# Move Pen
			pen_x = pen_x + button_width + padding
			button_width = 160
			default = n2mat.GetParamValue(texture_param_name)
			shader_params_menu[texture_param_name] = Draw.String("", TEXTURE_PARAM_CLICKED, pen_x, pen_y, button_width, button_height, default, 255, "Texture Params")
			#Draw Browse button
			pen_x = pen_x + button_width
			button_width = 20
			Draw.PushButton("...", TEXTURE_SELECT_CLICKED, pen_x, pen_y, button_width, button_height, "Select Texture")
	else:
		Draw.PupMenu("Error%t|You must have a blender material in the scene")
		Draw.Exit()
		return

############################################################################
#                          Main Program starts here                        #
############################################################################
	
# Get Window width and height an set padding
win_size = Window.GetAreaSize()
left = 0
top = win_size[1]
padding = 5

# Declare global buttons - Blender materials button and 
# Nebula's Shader buton
bl_mats_menu = 0
bl_mat_index = 0
n2_shader_menu = 0
n2_shader_index = 0
shader_params_menu = {}
texture_param_list = []
texture_param_index = 0

active_shader_index = 0
texture_param_index = 0

# Define events
BL_MAT_CHANGED = 1
NEBULA_SHADER_CHANGED = 2
SHADER_PARAM_CHANGED = 3
TEXTURE_PARAM_CHANGED = 4
TEXTURE_PARAM_CLICKED = 5
TEXTURE_SELECT_CLICKED = 6

file2 = n2.lookup("/sys/servers/file2")

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
	if(n2exporter.DoStartupChecks()):
		n2exporter.Init()
		mats = Material.Get()
		n2mats = n2exporter.Getn2Materials()
		n2_shaders = n2exporter.Getn2Shaders()
		bl_mats = []
		#mat_dict = {}
		
		n2exporter.BuildLookupTable()
		bl_mats = n2exporter.lookup_table.keys()
		
		n2mat = None
		
		# Get current objects material if any an set as active material
		objs = Object.GetSelected()
		if(len(objs) and objs[0].getType()=="Mesh" and len(objs[0].getData(mesh=1).materials) and objs[0].getData(mesh=1).materials[0]!=None):
			mat_name = objs[0].getData(mesh=1).materials[0].getName()
			print "mat_name: %s" % mat_name
			n2mat = n2exporter.n2Material(mat_name)
			bl_mat_index = bl_mats.index(mat_name)
			n2_shader_index = n2_shaders.index(n2mat.GetShader().m_name)
		elif(len(mats)):
			mat_name = mats[0].getName()
			n2mat = n2exporter.n2Material(mat_name)
			bl_mat_index = bl_mats.index(mat_name)
			n2_shader_index = n2_shaders.index(n2mat.GetShader().m_name)
		Draw.Register(draw, draw_event, button_event)
	else:
		print "Config Error|Go to Misc>Nebula Config"
		Draw.PupMenu("Startup Checks Failed%t|Startup Cheks Failed, check console")


	