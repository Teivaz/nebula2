-------------------------------------------------------------------------------
--  materialhandlers.ms
--
--  Defines script plugin and parameters which of custom attribute 
--  event handlers.
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
-------------------------------------------------------------------------------
fn OnMatAmbientChanged val =  
(
    if loading != true do
    (
        curMaterial = medit.GetCurMtl()
        if classof curMaterial != MultiMaterial do
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
            curMaterial.delegate.diffuseMap = val

            if curMaterial.delegate.diffuseMap != undefined do
                curMaterial.delegate.diffuseMapEnable = true
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
            curMaterial.delegate.bumpMap = val

            if curMaterial.delegate.bumpMap != undefined do
                curMaterial.delegate.bumpMapEnable = true
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
        curMaterial.delegate.specularMap  = val

        if curMaterial.delegate.specularMap  != undefined do
            curMaterial.delegate.specularMapEnable = true
        )
    )
)
