##########################################################################
#                     Nebula2 exporter module                           #
##########################################################################

import nblender as n2, Blender, os, string, re, math, xml.dom
from xml.dom import minidom
from Blender import Mesh, Object, Scene, Registry, Draw, Window, Mathutils, Material, Draw

# lists of known param types
normal_param_types = {"Int":1, "Float":2, "Bool":3, "Enum":4}
texture_param_types = {"Texture":1, "BumpTexture":2, "CubeTexture":3}
misc_param_types = {"Vector":1}

mat_scene_name = "nebulamaterials"
mat_scene = None

# Lookup table :  a dictionary with format blender_material:nebula_dummy_object_name
lookup_table = {}

def Init():
	"""
	Incase you go to file new, mat_scene and lookup_table still have their old values which causes all manners of bugs
	This function simply clears them and makes sure the material scene is created
	"""
	global mat_scene, lookup_table
	
	lookup_table = {}
	mat_scene = None
	scenes = Scene.Get()
	for scene in scenes:
		if(scene.getName() == mat_scene_name):
			mat_scene = scene
			return
	mat_scene = Scene.New(mat_scene_name)

class nVector:
	x = 0.0
	y = 0.0
	z = 0.0
	w = 0.0

	def __init__(self, x, y, z, w):
		self.x = float(x)
		self.y = float(y)
		self.z = float(z)
		self.w = float(w)


def StringToVector(s):
	c = string.split(s, " ")
	try:
		v = nVector(c[0], c[1], c[2], c[3])
		return v
	except IndexError:
		print "Invalid Components in string %s" % s

def VectorToString(v):
	s = ""
	try:
		s = "%.2f %.2f %.2f %.2f" % (v.x, v.y, v.z, v.w)
		return s
	except:
		print "%s is Not a Vector" % v

def GetPropertyValue(obj, property_name):
	"""
	Get the value of a property "property_name" or None if the property doesnt exist in the object
	"""
	properties = obj.getAllProperties()
	for property in properties:
		if(property.getName() == property_name):
			return property.getData()
			break
	# Fall through
	return None

def GetChildByName(obj, name):
	"""
	Get "obj's" child called "name" or None if child doesn't exist.
	
	This function is essentially used to get color information child object from a Lamp.
	"""
	children = GetChildren(obj)
	for child in children:
		if(child.getName() == name):
			return child
			break
	return None

def Getn2Materials():
	objs = mat_scene.getChildren()
	mats = []
	for obj in objs:
		if(obj.getType()=="Mesh"):
			mats.append(obj.getName())
	return mats

def Getn2Shaders():
	doc = minidom.parse(n2.lookup("/sys/servers/file2").manglepath("home:")+"/data/shaders/shaders.xml")
	shaders = doc.getElementsByTagName("shader")
	shd_list = []
	for shader in shaders:
		name = shader.getAttribute("name")
		shd_list.append(name)
	doc.unlink()	
	return shd_list
	
	
def CreateMaterial(mat_name):
	"""
	Create dummy mesh in mat_scene to link to material with name mat_name.
	"""
	global lookup_table
	obj = Object.New("Mesh", "n2_" + mat_name)
	me = Mesh.New("n2_" + mat_name)
	obj.link(me)
	obj.setMaterials([Material.Get(mat_name)])
	mat_scene.link(obj)
	lookup_table[mat_name] = obj.getName()
	n2mat = n2Material(mat_name)
	return n2mat

def BuildLookupTable():
	global lookup_table
	
	lookup_table = {}
	mats = Material.Get()
	n2mat_names = Getn2Materials()
	n2_shaders = Getn2Shaders()
	
	# Fill lookup Table
	for n2mat_name in n2mat_names:
		obj = Object.Get(n2mat_name)
		objs_mats = obj.getMaterials()
		if(len(objs_mats)):
			lookup_table[objs_mats[0].getName()] = n2mat_name
	
	# Check for unassigned materials
	for mat in mats:
		mat_name = mat.getName()
		if(not lookup_table.has_key(mat_name)):
			n2mat = CreateMaterial(mat_name)
			n2mat.SetDefaults()
	

def GetChildren(obj):
	"""
	Get obj's children as a list
	
	Consider using the current scene's objects to speed up 
	since its a slooooow liner search.
	"""
	children = ()
	all_objects = Blender.Object.Get()
	for current_object in all_objects:
		if current_object.getParent() == obj:
			children = children + (current_object,)
	return children

def GetTexturePath(path):
	# check if 'path' is a nebula path
	r = re.search('^[a-zA-Z0-9]+:{1}[a-zA-Z0-9]+', path)
	if(r):
		return path
	texture_dir = n2.lookup("/sys/servers/file2").manglepath("textures:")
		
	#path = os.path.abspath(path)
	path = Blender.sys.expandpath(path)
	filename = Blender.sys.basename(path)
	path = string.replace(path, "\\", "/")
	if(path[0:len(texture_dir)].lower() == texture_dir.lower()):
		path = "textures:" + path[(len(texture_dir)+1):]
	else:
		# Path outside texture assign
		file2 = n2.lookup("/sys/servers/file2")
		if(file2.fileexists(path)):
			file2.copyfile(path, "textures:%s" % filename)
			return "textures:%s" % filename
		return None
	return path

def DoStartupChecks():
	file2 = n2.lookup("/sys/servers/file2")
	# Check for shaders.xml in home:data/shaders/shaders.xml
	valid = True
	if(not file2.fileexists("home:data/shaders/shaders.xml")):
		print "shaders.xml not found in %s" % file2.manglepath("home:data/shaders/shaders.xml")
		valid = False
	if(not file2.fileexists("bin:nviewer.exe")):
		print "nviewer not found in %s" % file2.manglepath("bin:nviewer.exe")
		valid = False
	return valid
	
class n2ShaderParam:
	
	m_name = ""
	m_label = ""
	m_type = ""
	m_gui = ""
	m_export = ""
	m_default = ""
	m_min = ""
	m_max = ""
	m_enum = []
	m_default_enum = 0
	
	def __init__(self, _name, _label, _type, _gui, _export, _default, _min, _max, _enum, default_enum):
		self.m_name = _name
		self.m_label = _label
		self.m_type = _type
		self.m_gui = _gui
		self.m_export = _export
		self.m_default = _default
		self.m_min = _min
		self.m_max = _max
		self.m_enum = _enum
		self.m_default_enum = default_enum 

# End of n2ShaderParam class def

class n2Shader:
	
	m_name = ""
	m_params = {}
	
	def __init__(self, name):
		self.Clear()
		self.CreateNew(name)
		
	def Clear(self):
		self.m_params = {}
	
	def CreateNew(self, name):
		if(self.m_name != name):
			self.Clear()
			doc = minidom.parse(n2.lookup("/sys/servers/file2").manglepath("home:")+"/data/shaders/shaders.xml")
			shaders = doc.getElementsByTagName("shader")
			for shader in shaders:
				tmp_name = shader.getAttribute("name")
				if(tmp_name == name):
					self.m_name = name
					self.m_file = shader.getAttribute("file")
					# Create shader Params
					tmp_params = shader.getElementsByTagName("param")
					for tmp_param in tmp_params:
						param_type = tmp_param.getAttribute("type")
						param_gui = tmp_param.getAttribute("gui")
						if(param_type != "Color"):
							param_name = tmp_param.getAttribute("name")
							param_label = tmp_param.getAttribute("label")
							param_export = tmp_param.getAttribute("export")
							param_default = tmp_param.getAttribute("def")
							param_min = ""
							param_max = ""
							param_enum = []
							param_default_enum = 0
							if(tmp_param.hasAttribute("min") and tmp_param.hasAttribute("max")):
								param_min = tmp_param.getAttribute("min")
								param_max = tmp_param.getAttribute("max")
							if(tmp_param.hasAttribute("enum")):
								enum = tmp_param.getAttribute("enum")
								items = string.split(enum, ":")
								first_items = string.split(items[0], "=")
								items[0] = first_items[0]
								param_default_enum = int(first_items[1])
								param_enum = items
							param = n2ShaderParam(param_name, param_label, param_type, param_gui, param_export, param_default, param_min, param_max, param_enum, param_default_enum)
							self.m_params[param_name] = param
					break
			doc.unlink()
			
	def SetParamValue(self, param_name, value):
		pass
# End of n2Shader class def


class n2Material:
	
	m_shader = None
	m_obj = None
	
	def __init__(self, mat_name):
		self.m_obj = Object.Get(lookup_table[mat_name])		
		shader_name = GetPropertyValue(self.m_obj, "shader")
		if(shader_name):
			self.CreateShaderFromName(shader_name)
		else:
			self.CreateShaderFromName("Standard")
	
	def CreateShaderFromName(self, name):
		self.m_shader = n2Shader(name)
		
	def SetDefaults(self):
		self.m_obj.removeAllProperties()
		shader_name = self.m_shader.m_name
		self.m_obj.addProperty("shader", str(shader_name))
		params = self.m_shader.m_params
		for key in params:
			param = params[key]
			self.m_obj.addProperty(param.m_name, str(param.m_default))
	
	def SetParamValue(self, param_name, value):
		p = self.m_obj.getProperty(param_name)
		p.setData(value)
		
	def GetParamValue(self, param_name):
		p = self.m_obj.getProperty(param_name)
		return p.getData()
	
	def GetShader(self):
		return self.m_shader
# End of n2Material class def

	
class n2exporter:
	
	def __init__(self):
		"""
		Initialise scene root /scene and meshes NOH /meshes and delete if they exists to avoid conflicts
		
		"""
		self.CleanUp()
		self.mesh_uv = True
		self.mesh_col = False
		self.mesh_format = "n3d2"
		self.group = "examples"
		
		n2.new("nroot", "/export")
		
		self.model = n2.new("ntransformnode", "/export/model")
		n2.sel(self.model)
		self.tmp_mesh = n2.new("nscriptablemeshbuilder", "/export/meshes/tmp")
		self.mesh = n2.new("nscriptablemeshbuilder", "/export/meshes/model")
		
	def CleanUp(self):
		n2.sel("/")
		if(n2.exists("/export")):
			n2.delete("/export")
	
	def AddVertex(self, verts, face, obj):
		"""
		I only have thi here because after triangulating a quad, I end up with 2 faces,
		its just to avoid repeating code.
		"""
		mesh = obj.getData(mesh=1)
		use_vertex_normals = GetPropertyValue(obj, "vertexnormals")
		for dict in verts:
			for key in dict.keys():
				vert = dict.get(key)
				vidx = self.tmp_mesh.beginaddvertex()
				self.tmp_mesh.addcoord(vidx, -vert.co.x, vert.co.z, vert.co.y)
				if(self.mesh_uv == True):
					if(mesh.faceUV):
						self.tmp_mesh.adduv(vidx, 0, face.uv[key][0], face.uv[key][1])
					else:
						self.tmp_mesh.adduv(vidx, 0, 0, 0)
				if(not use_vertex_normals):
					self.tmp_mesh.addnormal(vidx, -face.no.x, face.no.z, face.no.y)
						
				self.tmp_mesh.endaddvertex()
	
	def CreateMesh(self, face_list, obj):
		mesh = obj.getData(mesh=1)
		self.tmp_mesh.clear()
		i = 0;
		for f in face_list:
			verts = []
			if(len(f.verts)>3):
				# Triangulate
				verts.append({0:f.verts[0]})
				verts.append({2:f.verts[2]})
				verts.append({1:f.verts[1]})
				self.AddVertex(verts, f, obj)
				i = i+3
				self.tmp_mesh.addtriangle(0, i-3, i-1, i-2)
				
				verts = []
				verts.append({0:f.verts[0]})
				verts.append({3:f.verts[3]})
				verts.append({2:f.verts[2]})
				self.AddVertex(verts, f, obj)
				i = i+3
				self.tmp_mesh.addtriangle(0, i-3, i-1, i-2)
			else:
				verts.append({0:f.verts[0]})
				verts.append({2:f.verts[2]})
				verts.append({1:f.verts[1]})
				self.AddVertex(verts, f, obj)
				i = i+3
				self.tmp_mesh.addtriangle(0, i-3, i-1, i-2)
			
	
	def SetShader(self, node, mat=None):
		if(mat):
			mat_name = mat.getName()
			n2mat = None
			if(lookup_table.has_key(mat_name)):
				n2mat = n2Material(mat_name)
			else:
				n2mat = CreateMaterial(mat_name)
				n2mat.SetDefaults()
			diffuse = mat.getRGBCol()
			specular = mat.getSpecCol()
			emissive = mat.getMirCol()
			alpha = mat.getAlpha()
			node.setvector("MatDiffuse", diffuse[0], diffuse[1], diffuse[2], alpha)
			node.setvector("MatEmissive", emissive[0], emissive[1], emissive[2], alpha)
			node.setvector("MatSpecular", specular[0], specular[1], specular[2], alpha)
			shader = n2mat.GetShader()
			params = shader.m_params
			node.setshader(str(shader.m_file))
				
			for key in params:
				param = params[key]
				param_name = str(param.m_name)
				if(param.m_type == "Int"):
					node.setint(param_name, int(n2mat.GetParamValue(key)))
				elif(param.m_type == "Bool"):
					node.setbool(param_name, int(n2mat.GetParamValue(key))) 
				elif(param.m_type == "Float"):
					node.setfloat(param_name, float(n2mat.GetParamValue(key)))
				elif(param.m_type == "Vector"):
					v = StringToVector(n2mat.GetParamValue(key))
					node.setvector(param_name, v.x, v.y, v.z, v.w)
				elif(texture_param_types.has_key(param.m_type)):
					param_value = str(n2mat.GetParamValue(key))
					texture_path = GetTexturePath(param_value)
					if(texture_path):
						node.settexture(param_name, texture_path)
		else:
			node.setshader("static")
		
	
	def GetTypeFromProperty(self, obj):
		return GetPropertyValue(obj, "class")
			
								
	def ExportAsMesh(self, obj, node=None):
		"""
		Export "obj" as Mesh by extracting and saving mesh data.
		"""
		node = n2.lookup(n2.psel())
		me = obj.getData(mesh=1)
		editmode = Window.EditMode()
		if(editmode):
			Window.EditMode(0)
		obj_name = obj.getName()
		# Check if more than one material
		if(len(me.materials)>1):
			n2class = self.GetTypeFromProperty(obj)
			if(not n2class):
				n2class = "nshapenode"
			# Create dictionary with materila index as key and kist of face indices as value
			mat_dict = {}
			faces = me.faces
			for f in faces:
				if(mat_dict.has_key(f.mat)):
					mat_dict[f.mat].append(f)
				else:
					mat_dict[f.mat] = []
					mat_dict[f.mat].append(f)
			# Create nshapenode for each material
			i = 0 # hack in case the same materila is assigned to diffrenet vertex groups
			for index in mat_dict:
				mat = me.materials[index]
				node = n2.new(n2class, "%s_%s_%i" % (obj_name, mat.getName(), i))
				self.CreateMesh(mat_dict[index], obj)
				group_idx = self.mesh.append(self.tmp_mesh)
				try:
					node.setgroupindex(group_idx)
					node.setmesh("meshes:%s/%s.%s" % (self.group, self.filename, self.mesh_format))
					self.SetShader(node, mat)
				except AttributeError:
					n2.nprint("%s 'class' property must be an nshapenode or derived\n" % obj_name)
				i = i+1
		else:
			if(node.isa("nshapenode")):
				mesh = obj.getData(mesh=1)
				self.CreateMesh(me.faces, obj)
				group_idx = self.mesh.append(self.tmp_mesh)
				node.setmesh("meshes:%s/%s.%s" % (self.group, self.filename, self.mesh_format))
				node.setgroupindex(group_idx)
				if(len(mesh.materials)>0):
					mat = mesh.materials[0]
					self.SetShader(node, mat)
				else:
					self.SetShader(node)
			else:
				n2.nprint("WARNING: Trying to export Mesh '%s' as ntransformnode, Mesh data not exported\n" % obj.getName())
		
		Window.EditMode(editmode)
			
	
	
	def ExportAsLight(self, obj, node=None):
		"""
		Export object as a light node.
		
		If the light has different color values for Ambient, Diffuse and Specular, 
		create a mesh object and give its material colors the desired values then parent it to the light. 
		The objects name has to be in the format <light_name>_"color" e.g. if the light's name is Lamp01 
		the Meshes name should be "Lamp01_color"
		"""
	
		if(obj.getType()=="Lamp"):
			obj_name = obj.getName()
			lamp = obj.getData()
			scene_node = n2.lookup(n2.psel())
			scene_node.setfloat("LightRange", lamp.getDist())
			#scene_node.settype("Directional")
			
			mode = lamp.getMode()
			if(mode & lamp.Modes["RayShadow"]):
				scene_node.setcastshadows(True)
			else:
				scene_node.setcastshadows(False)
			
			color_info_obj = GetChildByName(obj, obj_name + "_color")
			if(color_info_obj and color_info_obj.getType() == "Mesh" and color_info_obj.getData(mesh=True).materials[0]):
				material = color_info_obj.getData(mesh=True).materials[0]
				# Col = Diffuse, Spe = Specular, Mir = Ambient
				diffuse = material.getRGBCol()
				specular = material.getSpecCol()
				ambient = material.getMirCol()
				scene_node.setvector("LightDiffuse", diffuse[0], diffuse[1], diffuse[2], 1.0)
				scene_node.setvector("LightSpecular", specular[0], specular[1], specular[2], 1.0)
				scene_node.setvector("LightAmbient", ambient[0], ambient[1], ambient[2], 1.0)
			else:
				n2.nprint("WARNING: Color information object for Lamp '%s' not found or not valid" % obj_name)
				col = lamp.col
				scene_node.setvector("LightDiffuse", col[0], col[1], col[2], 1.0)
				scene_node.setvector("LightSpecular", col[0], col[1], col[2], 1.0)
				scene_node.setvector("LightAmbient", col[0], col[1], col[2], 1.0)
		else:
			n2.nprint("WARNING: %s is Not a Lamp and was not exported\n" % obj.getName())
	
	
	def GetTypeFromData(self, obj):
		if(obj.getType()=="Lamp"):
			return "nlightnode"
		else:
			return "ntransformnode"
		
	
	def ExportObj(self, obj):
		"""
		Call for each object to be added to the export hierachy.
		
		"obj's" children will also be exported by this function.
		Usage. Only call directly when you need to export a single or selected objects, otherwise
		use ExportScene(blender_scene).
		Determine Nebula scenenode class by first checking for a property "class" in obj if not found
		determine through obj.getData() and if this fails export as ntransformnode.
		If "obj" has more than one material, create a transform node and export each material 
		group as a different nshapenode
		
		"""
		bl_type = obj.getType()
		obj_name = obj.getName()
		
		n2class = None #self.GetTypeFromProperty(obj)
		if(bl_type == "Mesh" and len(obj.getData(mesh=1).materials)>1):
			# Multimaterial mesh with 'class' property
			n2class = "ntransformnode"
		elif(bl_type == "Mesh" and len(obj.getData(mesh=1).materials)<=1):
			n2class = self.GetTypeFromProperty(obj)
			if(not n2class):
				n2class = "nshapenode"
				
		if(not n2class):
			n2class = self.GetTypeFromData(obj)
		
		node = n2.new(n2class, obj_name)
		n2.sel(node)
		
		# TODO: Set transform properties
		location = obj.getLocation()
		pos = [location[0], location[1], location[2]]
		
		rot = Mathutils.Euler(obj.getEuler())
		rot[0] = math.degrees(rot[0])
		rot[1] = math.degrees(rot[1])
		rot[2] = math.degrees(rot[2])
		
		size = obj.getSize()
		scale = [size[0], size[1], size[2]]
		
		if(obj.parent):
			parent = obj.parent
			parent_world_mat = parent.getMatrix("worldspace")
			
			# Copy wrapped data and Invert
			parent_world_mat = Mathutils.Matrix(parent_world_mat)
			parent_world_mat.invert()
			local_mat = obj.getMatrix("localspace")
			world_mat = obj.getMatrix("worldspace")
	
			local_mat = world_mat * parent_world_mat
			
			# Rotation
			rot = local_mat.rotationPart().toEuler()
			
			# Position
			pos[0] = local_mat[3][0]
			pos[1] = local_mat[3][1]
			pos[2] = local_mat[3][2]
			
			# Scale
			quat = local_mat.toQuat()
			inv_rot_mat = quat.toMatrix()
			inv_rot_mat.resize4x4()
			inv_rot_mat.invert()
			scale_mat = local_mat * inv_rot_mat
			
			scale[0] = scale_mat[0][0]
			scale[1] = scale_mat[1][1]
			scale[2] = scale_mat[2][2]
		
		node.setposition(-pos[0], pos[2], pos[1])
		node.seteuler(-rot[0], rot[2], rot[1])
		node.setscale(scale[0], scale[2], scale[1])	
		
		if(bl_type == "Mesh"):
			self.ExportAsMesh(obj)
		elif(bl_type == "Lamp"):
			self.ExportAsLight(obj)
			
		# TODO: Export Animators
		
		# Get objects children and export each one while still in parent object's working directory
		children = GetChildren(obj)
		for child in children:
			# HACK: Dont export objetcs whose parent is a light and name is <parent>_color
			if(obj.getType()=="Lamp" and child.getName()==(obj.getName()+"_color")):
				# child provides color information
				pass
			else:
				self.ExportObj(child)
		# Select initial working dirctory
		n2.sel("..")
	
	
	def ExportScene(self, scene=None):
		"""
		Export all objects in "scene"
		"""
		scn = Blender.Scene.GetCurrent()
		
		if(scene != None):
			scn = scene
		
		objs = scn.getChildren()
		
		if(len(objs)):
			root = n2.new("ntransformnode", self.filename)
			n2.sel(root)
			model = n2.new("ntransformnode", "model")
			n2.sel(model)
			for obj in objs:
				if(not obj.parent): #parented objects will be exported by parent
					self.ExportObj(obj)
			return True
		else:
			return False
	
	
	def SaveScene(self):
		"""
		Save the exported scene to an .n2 object.
		
		If the scene_name == None, the .n2 object's name will be the current scenes name.
		"""
		self.model.saveas("gfxlib:%s/%s.n2" % (self.group, self.filename))
		file2 = n2.lookup("/sys/servers/file2")
		self.mesh.buildtrianglenormals()
		self.mesh.buildvertextangents(False)
		self.mesh.buildvertexnormals()
		self.mesh.cleanup()
		self.mesh.save(file2, "meshes:%s/%s.%s" % (self.group, self.filename, self.mesh_format));
#end of n2exporter class def
		