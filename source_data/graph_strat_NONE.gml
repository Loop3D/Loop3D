# --- COLUMN NAMES IN CSV DATA FILES: -------------------------------------------------------------
# OBJECT COORDINATES              =WKT
# FAULT: ID                       =objectid
# FAULT: FEATURE                  =type
# POLYGON: ID                     =objectid
# POLYGON: LEVEL1 NAME            =RU_NAME
# POLYGON: LEVEL2 NAME            =Parent_Nam
# POLYGON: MIN AGE                =Top_Mini_1
# POLYGON: MAX AGE                =ASUD_AGE
# POLYGON: CODE                   =Stratigrap
# POLYGON: DESCRIPTION            =ROCK_TYPE
# POLYGON: ROCKTYPE1              =Intrusive
# POLYGON: ROCKTYPE2              =LITH_SUMM
# DEPOSIT: SITE CODE              =site_no
# DEPOSIT: SITE TYPE              =mine_statu
# DEPOSIT: SITE COMMODITY         =main_com_1
# --- SOME CONSTANTS: ----------------------------------------------------------------------------
# FAULT AXIAL FEATURE NAME        =Fold
# SILL UNIT DESCRIPTION CONTAINS  =sill
# IGNEOUS ROCKTYPE CONTAINS                           =Y
# VOLCANIC ROCKTYPE CONTAINS                          =VOLCANIC
# IGNORE DEPOSITS WITH SITE TYPE                      =Infrastructure
# Intersect Contact With Fault: angle epsilon (deg)   =1.0
# Intersect Contact With Fault: distance epsilon (m)  =15.0
# Distance buffer (fault stops on another fault) (m)  =20.0
# Distance buffer (point on contact) (m)              =500.0
# Intersect polygons distance buffer (for bad maps)   =3.
# ------------------------------------------------------------------------------------------------
# Path to the output data folder                      =tmp_data/graph/
# Path to geology data file                           =tmp_data/tmp/geology_file_file.csv
# Path to faults data file                            =tmp_data/tmp/fault_file.csv
# Path to mineral deposits data file                  =tmp_data/tmp/mindep_file.csv
# ------------------------------------------------------------------------------------------------
# Clipping window X1 Y1 X2 Y2 (zeros for infinite)    =-334514 7713303 -292074 7739799
# Min length fraction for strat/fault graphs          =0.0
# Graph edge width categories (three doubles)         =2000. 20000. 200000.
# Graph edge direction (0-min age, 1-max age, 2-avg)  =1
# Deposit names for adding info on the graph          =Fe,Cu,Au,NONE
# Partial graph polygon ID                            =32
# Partial graph depth                                 =4
# Map subregion size dx, dy [m] (zeros for full map)  =0. 0.
# ------------------------------------------------------------------------------------------------
Creator "map2model-cpp"
graph [
  hierarchic 1
  directed 1
  node [
    id -2
    LabelGraphics [ text "Carters Bore Rhyolite" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -3
    LabelGraphics [ text "Eastern Creek Volcanics" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -6
    LabelGraphics [ text "Fe" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -4
    LabelGraphics [ text "Haslingden Group" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -9
    LabelGraphics [ text "Keithys Granite" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -10
    LabelGraphics [ text "Kitty Plain Microgranite" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -11
    LabelGraphics [ text "Lakeview Dolerite" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -15
    LabelGraphics [ text "Loretta Supersequence" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -5
    LabelGraphics [ text "McNamara Group" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -8
    LabelGraphics [ text "Mount Isa Group" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -1
    LabelGraphics [ text "Myally Subgroup" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -17
    LabelGraphics [ text "Paradise Creek Formation" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -21
    LabelGraphics [ text "Quilalar Formation" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -23
    LabelGraphics [ text "Sybella Granite" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id -27
    LabelGraphics [ text "do" anchor "n" fontStyle "bold" fontSize 14 ]
    isGroup 1
    graphics [ fill "#FAFAFA" ]
  ]
  node [
    id 6
    LabelGraphics [ text "Alsace Quartzite" fontSize 14 ]
    gid -1
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 14
    LabelGraphics [ text "Carters Bore Rhyolite" fontSize 14 ]
    gid -2
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 7
    LabelGraphics [ text "Cromwell Metabasalt Member" fontSize 14 ]
    gid -3
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 8
    LabelGraphics [ text "Eastern Creek Volcanics" fontSize 14 ]
    gid -4
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 25
    LabelGraphics [ text "Esperanza Formation" fontSize 14 ]
    gid -5
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 0
    LabelGraphics [ text "Fe" fontSize 14 ]
    gid -6
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 23
    LabelGraphics [ text "Gunpowder Creek Formation" fontSize 14 ]
    gid -5
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 16
    LabelGraphics [ text "Hero Formation" fontSize 14 ]
    gid -8
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 5
    LabelGraphics [ text "Keithys Granite" fontSize 14 ]
    gid -9
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 4
    LabelGraphics [ text "Kitty Plain Microgranite" fontSize 14 ]
    gid -10
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 1
    LabelGraphics [ text "Lakeview Dolerite" fontSize 14 ]
    gid -11
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 13
    LabelGraphics [ text "Leander Quartzite" fontSize 14 ]
    gid -4
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 9
    LabelGraphics [ text "Lena Quartzite Member" fontSize 14 ]
    gid -3
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 11
    LabelGraphics [ text "Lochness Formation" fontSize 14 ]
    gid -1
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 20
    LabelGraphics [ text "Loretta Supersequence" fontSize 14 ]
    gid -15
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 17
    LabelGraphics [ text "Moondarra Siltstone" fontSize 14 ]
    gid -8
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 21
    LabelGraphics [ text "Mount Oxide Chert Member" fontSize 14 ]
    gid -17
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 10
    LabelGraphics [ text "Myally Subgroup" fontSize 14 ]
    gid -4
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 24
    LabelGraphics [ text "Paradise Creek Formation" fontSize 14 ]
    gid -5
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 12
    LabelGraphics [ text "Pickwick Metabasalt Member" fontSize 14 ]
    gid -3
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 26
    LabelGraphics [ text "Quilalar Formation" fontSize 14 ]
    gid -21
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 27
    LabelGraphics [ text "Surprise Creek Formation" fontSize 14 ]
    gid -8
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 3
    LabelGraphics [ text "Sybella Granite" fontSize 14 ]
    gid -23
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 22
    LabelGraphics [ text "Torpedo Creek Quartzite" fontSize 14 ]
    gid -5
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 19
    LabelGraphics [ text "Warrina Park Quartzite" fontSize 14 ]
    gid -8
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 15
    LabelGraphics [ text "Whitworth Quartzite" fontSize 14 ]
    gid -1
    graphics [ fill "#ffffff" w 150 ]
  ]
  node [
    id 28
    LabelGraphics [ text "do" fontSize 14 ]
    gid -27
    graphics [ fill "#ffffff" w 150 ]
  ]
  edge [
    source 0
    target 16
    graphics [ style "line" arrow "last" width 1 fill "#002ad4" ]
  ]
  edge [
    source 1
    target 13
    graphics [ style "line" arrow "last" width 3 fill "#0028d6" ]
  ]
  edge [
    source 1
    target 28
    graphics [ style "line" arrow "both" width 1 fill "#0000ff" ]
  ]
  edge [
    source 3
    target 5
    graphics [ style "line" arrow "both" width 3 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 5
    graphics [ style "line" arrow "both" width 3 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 7
    graphics [ style "line" arrow "last" width 3 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 8
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 9
    graphics [ style "line" arrow "last" width 3 fill "#0031cd" ]
  ]
  edge [
    source 4
    target 10
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 11
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 12
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 4
    target 15
    graphics [ style "line" arrow "last" width 3 fill "#0000ff" ]
  ]
  edge [
    source 5
    target 12
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 15
    target 6
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 9
    target 7
    graphics [ style "line" arrow "last" width 5 fill "#007f7f" ]
  ]
  edge [
    source 12
    target 7
    graphics [ style "line" arrow "last" width 5 fill "#006c92" ]
  ]
  edge [
    source 7
    target 13
    graphics [ style "line" arrow "last" width 5 fill "#00fe00" ]
  ]
  edge [
    source 15
    target 7
    graphics [ style "line" arrow "last" width 3 fill "#3cc200" ]
  ]
  edge [
    source 16
    target 7
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 8
    target 9
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 12
    target 9
    graphics [ style "line" arrow "last" width 5 fill "#0047b7" ]
  ]
  edge [
    source 11
    target 12
    graphics [ style "line" arrow "last" width 3 fill "#009668" ]
  ]
  edge [
    source 14
    target 11
    graphics [ style "line" arrow "last" width 3 fill "#00f40a" ]
  ]
  edge [
    source 11
    target 15
    graphics [ style "line" arrow "last" width 5 fill "#00619d" ]
  ]
  edge [
    source 23
    target 11
    graphics [ style "line" arrow "last" width 5 fill "#00b34b" ]
  ]
  edge [
    source 26
    target 11
    graphics [ style "line" arrow "last" width 5 fill "#003dc1" ]
  ]
  edge [
    source 27
    target 11
    graphics [ style "line" arrow "last" width 3 fill "#0035c9" ]
  ]
  edge [
    source 15
    target 12
    graphics [ style "line" arrow "last" width 5 fill "#00847a" ]
  ]
  edge [
    source 26
    target 12
    graphics [ style "line" arrow "last" width 1 fill "#9f5f00" ]
  ]
  edge [
    source 16
    target 13
    graphics [ style "line" arrow "last" width 3 fill "#fd0100" ]
  ]
  edge [
    source 28
    target 13
    graphics [ style "line" arrow "last" width 5 fill "#000cf2" ]
  ]
  edge [
    source 22
    target 14
    graphics [ style "line" arrow "last" width 3 fill "#0042bc" ]
  ]
  edge [
    source 23
    target 14
    graphics [ style "line" arrow "last" width 5 fill "#003ec0" ]
  ]
  edge [
    source 14
    target 26
    graphics [ style "line" arrow "last" width 5 fill "#0042bc" ]
  ]
  edge [
    source 27
    target 14
    graphics [ style "line" arrow "last" width 3 fill "#0027d7" ]
  ]
  edge [
    source 23
    target 15
    graphics [ style "line" arrow "last" width 3 fill "#b14d00" ]
  ]
  edge [
    source 26
    target 15
    graphics [ style "line" arrow "last" width 1 fill "#ca3400" ]
  ]
  edge [
    source 27
    target 15
    graphics [ style "line" arrow "last" width 3 fill "#003dc1" ]
  ]
  edge [
    source 17
    target 16
    graphics [ style "line" arrow "last" width 1 fill "#0000ff" ]
  ]
  edge [
    source 20
    target 24
    graphics [ style "line" arrow "last" width 3 fill "#fe0000" ]
  ]
  edge [
    source 20
    target 25
    graphics [ style "line" arrow "last" width 5 fill "#000bf3" ]
  ]
  edge [
    source 21
    target 23
    graphics [ style "line" arrow "last" width 5 fill "#0005f9" ]
  ]
  edge [
    source 21
    target 24
    graphics [ style "line" arrow "last" width 5 fill "#0016e8" ]
  ]
  edge [
    source 22
    target 27
    graphics [ style "line" arrow "last" width 1 fill "#002ad4" ]
  ]
  edge [
    source 24
    target 23
    graphics [ style "line" arrow "last" width 5 fill "#4faf00" ]
  ]
  edge [
    source 23
    target 26
    graphics [ style "line" arrow "last" width 5 fill "#008678" ]
  ]
  edge [
    source 23
    target 27
    graphics [ style "line" arrow "last" width 5 fill "#0058a6" ]
  ]
  edge [
    source 25
    target 24
    graphics [ style "line" arrow "last" width 5 fill "#00a559" ]
  ]
  edge [
    source 27
    target 26
    graphics [ style "line" arrow "last" width 3 fill "#0035c9" ]
  ]
  edge [
    source 5
    target 7
    graphics [ style "line" arrow "last" width 5 fill "#000af4" ]
  ]
  edge [
    source 15
    target 8
    graphics [ style "line" arrow "last" width 1 fill "#0020de" ]
  ]
  edge [
    source 19
    target 26
    graphics [ style "line" arrow "last" width 1 fill "#0058a6" ]
  ]
  edge [
    source 19
    target 27
    graphics [ style "line" arrow "last" width 1 fill "#003bc3" ]
  ]
  edge [
    source 23
    target 22
    graphics [ style "line" arrow "last" width 3 fill "#0047b7" ]
  ]
]