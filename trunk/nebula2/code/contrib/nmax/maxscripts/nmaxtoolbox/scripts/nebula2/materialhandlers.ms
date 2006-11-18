-------------------------------------------------------------------------------
--  materialhandlers.ms
--
--  Defines script plugin and parameters which of custom attribute 
--  event handlers. 
--  Each of handlers are called whenever parameters of the Nebula custom material 
--  plugins are changed or the plugin is loaded.
--  
--  If you want to add additional event handlers, see GetEventHandler() function
--  for parameter event handlers and AddPluginEventHandlers() function for
--  script plugin event handlers in nmaxmaterial_shd.cc
--  
--  (c)2005 Kim, Hyoun Woo
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
--  'create' script plugin event handler
--
-------------------------------------------------------------------------------
fn OnPluginCreate = 
(
)

-------------------------------------------------------------------------------
--  'postCreate' script plugin event handler
--
-------------------------------------------------------------------------------
fn OnPluginPostCreate = 
(
)

-------------------------------------------------------------------------------
--  'load' script plugin event handler
--
-------------------------------------------------------------------------------
fn OnPluginLoad = 
(
/*
    curMaterial = meditMaterials[activeMeditSlot]
    if classof curMaterial == MultiMaterial do
    (
        setMeditMaterial activeMeditSlot meditMaterials[1].materiallist[1] 
    )
*/
)

-------------------------------------------------------------------------------
--  'postLoad' script plugin event handler
--
-------------------------------------------------------------------------------
fn OnPluginPostLoad = 
(
)

-------------------------------------------------------------------------------
--  'MapAmbient' parameter event handler
--
--  When the .max file is loaded which of the material is multi material,
--  there is no activate slot. So we should check then skip it. 
--  Also delegate of the material in activate slot can not be accessible
--  in loading time.
--   
-------------------------------------------------------------------------------
fn OnMatAmbientChanged val =  
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
            if curMaterial.delegate != undefined do
                curMaterial.delegate.ambient = val
    )
)

-------------------------------------------------------------------------------
--  'MapDiffuse' parameter event handler
--
-------------------------------------------------------------------------------
fn OnMatDiffuseChanged val = 
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
            if curMaterial.delegate != undefined do
                curMaterial.delegate.diffuse = val
    )	
)

-------------------------------------------------------------------------------
--  'MapSpecular' parameter event handler
--
-------------------------------------------------------------------------------
fn OnMatSpecularChanged val = 
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
            if curMaterial.delegate != undefined do
                curMaterial.delegate.specular = val
    )
)

-------------------------------------------------------------------------------
--  'DiffuseMap0' parameter event handler
--
-------------------------------------------------------------------------------
fn OnDiffMap0Changed val = 
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
        (
            if curMaterial.delegate != undefined do
            (
                curMaterial.delegate.diffuseMap = val

                if curMaterial.delegate.diffuseMap != undefined do
                    curMaterial.delegate.diffuseMapEnable = true
            )
        )
    )
)

-------------------------------------------------------------------------------
--  'BumpMap0' parameter event handler
--
-------------------------------------------------------------------------------
fn OnBumpMap0Changed val = 
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do        
        (
            if curMaterial.delegate != undefined do
            (
                curMaterial.delegate.bumpMap = val

                if curMaterial.delegate.bumpMap != undefined do
                    curMaterial.delegate.bumpMapEnable = true
            )
        )
    )
)

-------------------------------------------------------------------------------
--  'CubeMap' parameter event handler
--
-------------------------------------------------------------------------------
fn OnCubeMapChanged val = 
(
)

-------------------------------------------------------------------------------
--  'SpecMap0' parameter event handler
--
-------------------------------------------------------------------------------
fn OnSpecMap0Changed val = 
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
        (		 
            if curMaterial.delegate != undefined do
            (
                curMaterial.delegate.specularMap  = val

                if curMaterial.delegate.specularMap  != undefined do
                    curMaterial.delegate.specularMapEnable = true
            )
        )
    )
)

-------------------------------------------------------------------------------
-- diffuse map 1
-------------------------------------------------------------------------------
fn OnDiffMap1Changed val = 
(
)

-------------------------------------------------------------------------------
-- diffuse map 2
-------------------------------------------------------------------------------
fn OnDiffMap2Changed val = 
(
)

-------------------------------------------------------------------------------
-- diffuse map 3
-------------------------------------------------------------------------------
fn OnDiffMap3Changed val = 
(
)

-------------------------------------------------------------------------------
-- bump map 1
-------------------------------------------------------------------------------
fn OnBumpMap1Changed val = 
(
)

-------------------------------------------------------------------------------
-- cube map 0
-------------------------------------------------------------------------------
fn OnCubeMap0Changed val = 
(
)
-------------------------------------------------------------------------------
-- EOF
-------------------------------------------------------------------------------
