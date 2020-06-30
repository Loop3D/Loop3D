import subprocess
from map2loop import m2l_topology
from map2loop import m2l_utils
from map2loop import m2l_geometry
from map2loop import m2l_interpolation
from map2loop import m2l_export
import LoopProjectFile
import geopandas
import pandas
from shapely.geometry import Polygon
import urllib
import rasterio
import numpy
import re

if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "small.loop3d"
#print(loopFilename)

boundaries = [500057,603028,7455348,7567953,-1200,8200]
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]: print(resp["errorString"])
else: boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]
#print(boundaries)

# Input to geoserver is in GDA94 Zone 50 (48-58 srs=epsg:28348-28358) (Mark uses geopandas.GeoDataFrame.to_crs
# for conversions) I need to look into options for geometry extents to these geodataframes currently
# using shapely Polygon structures  (Note WGS84 is epsg:4326, GDA94(lat/lon/alt) is epsg:4939, 
# GDA94(lat/long) is epsg:4283)
# WGS84/UTM zone 1N-60N  32601-32660, 1S-60S  32701-32760
# X is Eastings, Y is Northings in 'All in one Hamersley-LoopStructural.ipynb
minEasting = boundaries[0]
maxEasting = boundaries[1]
minNorthing = boundaries[2]
maxNorthing = boundaries[3]

zoneStr = 'epsg:28350'
zone = {'init':zoneStr}

orientation_decimate=0  #store every nth orientation (in object order) 0 = save all
contact_decimate=5      #store every nth contact point (in object order) 0 = save all
fault_decimate=5        #store every nth fault point (in object order) 0 = save all
fold_decimate=5         #store every nth fold axial trace point (in object order) 0 = save all
scheme='scipy_rbf'

bboxGDAZ50=(minEasting,minNorthing,maxEasting,maxNorthing)

boundariesString = str(minEasting)+","+str(minNorthing)+","+str(maxEasting)+","+str(maxNorthing)
geology_url='http://geo.waxi-gis.org/geoserver/loop/wfs?service=WFS&version=1.0.0&request=GetFeature&typeName=loop:geol_500k&bbox='+boundariesString+'&srs='+zoneStr
structure_url='http://geo.waxi-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=warox_points_f5011&bbox='+boundariesString+'&srs='+zoneStr
fault_url='http://geo.waxi-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=linear_500k&bbox='+boundariesString+'&srs='+zoneStr

data_dir="tmp_data2/"
data_dir_shp=data_dir+"shapeFiles/"
geology_file='geology.shp'
geology_file_csv=data_dir+geology_file.replace(".shp",".txt")
geology_file=data_dir_shp+geology_file
structure_file='structure.shp'
structure_file_csv=data_dir+structure_file.replace(".shp",".txt")
structure_file=data_dir_shp+structure_file
fault_file='fault.shp'
fault_file_csv=data_dir+fault_file.replace(".shp",".txt")
fault_file=data_dir_shp+fault_file

#print(geology_file)
c_l = {
    #Orientations
      "d": "dip",                 #field that contains dip information
      "dd": "dip_dir",            #field that contains dip direction information
      "sf": 'feature',            #field that contains information on type of structure
      "bedding": 'bed',           #text to search for in field defined by sfcode to show that this is a bedding measurement
    #Stratigraphy
      "g": 'group_',              #field that contains coarser stratigraphic coding
      "c": 'code',                #field that contains finer stratigraphic coding
      "ds": 'descriptn',          #field that contains information about lithology
      "u": 'unitname',            #field that contains alternate stratigraphic coding (not used??)
      "r1": 'rocktype1',          #field that contains  extra lithology information
      "r2": 'rocktype2',          #field that contains even more lithology information
      "sill": 'sill',             #text to search for in field defined by dscode to show that this is a sill
      "intrusive": 'intrusive',   #text to search for in field defined by dscode to show that this is an intrusion
      "volcanic": 'volcanic',     #text to search for in field defined by dscode to show that this is an intrusion
    #Timing
      "min": 'min_age_ma',        #field that contains minimum age of unit defined by ccode
      "max": 'max_age_ma',        #field that contains maximum age of unit defined by ccode
    #faults and folds
      "f": 'feature',             #field that contains information on type of structure
      "fault": 'Fault',           #text to search for in field defined by fcode to show that this is a fault
      "fold": 'Fold axial trace', #text to search for in field defined by fcode to show that this is a fold axial trace
      "n": 'name',                #field that contains information on name of fault (not used??)
      "t": 'type',                #field that contains information on type of fold
      "syn": 'syncline',          #text to search for in field defined by t to show that this is a syncline
    #ids
      "o": 'objectid',            #field that contains unique id of geometry object
      "gi": 'geopnt_id'           #field that contains unique id of structure point    
}

# About to try servers for data collection ...
try:
    geology = geopandas.read_file(geology_url,bbox=bboxGDAZ50)
    geology.to_file(geology_file)
    sub_geol = geology[['geometry', c_l['o'], c_l['c'], c_l['g'], c_l['u'],
        c_l['min'], c_l['max'], c_l['ds'], c_l['r1'], c_l['r2']]]
    m2l_topology.save_geol_wkt(sub_geol,geology_file_csv,c_l)

    structure = geopandas.read_file(structure_url,bbox=bboxGDAZ50)
    structure.to_file(structure_file)
    sub_structure = structure[['geometry',c_l['gi'],c_l['d'],c_l['dd']]]
    m2l_topology.save_structure_wkt(sub_structure,structure_file_csv,c_l)

    faults = geopandas.read_file(fault_url,bbox=bboxGDAZ50)
    faults.to_file(fault_file)
    sub_faults = faults[['geometry',c_l['o'],c_l['f']]]
    m2l_topology.save_faults_wkt(sub_faults,fault_file_csv,c_l)

    print("Working out orientations from geological map")
except urllib.error.HTTPError as err:
    print(err)
    print("Aborting download using pre-dled data")

# Convert MGA Zone 50 data to GDA94 (Lat/Long)
eastingList=[minEasting,maxEasting,maxEasting,minEasting,minEasting]
northingList=[minNorthing,minNorthing,maxNorthing,maxNorthing,minNorthing]
geomBounds = Polygon(zip(eastingList,northingList))
geoframe = geopandas.GeoDataFrame(index=[0], crs=zone,geometry=[geomBounds])
geoframeLL = geoframe.to_crs({'init':'epsg:4326'})

# get DEM from hawaii source via map2loop functions reproject to MGA Zone 50
try:
    expandDeg = 0.045
    m2l_utils.get_dtm(data_dir+"/dtm.tif",
      geoframeLL.total_bounds[0]-expandDeg,geoframeLL.total_bounds[2]+expandDeg,
      geoframeLL.total_bounds[1]-expandDeg,geoframeLL.total_bounds[3]+expandDeg)
    demReprojected = m2l_utils.reproject_dtm(
      data_dir+"/dtm.tif",
      data_dir+"/dtm_reprojected.tif",
      {'init':'epsg:4326'},
      zone)
except:
    print("DTM service down proceeding with stored data")

# About to run map2model-cpp code
m2l_topology.save_Parfile("./",c_l,
    data_dir+"/graph",
    data_dir+"/geology.txt",
    data_dir+"/fault.txt",
    data_dir+"/structure.txt",
    minEasting,maxEasting,minNorthing,maxNorthing)
# This subprocess creates a lot of files in <data_dir>/graph directory
subprocess.run(["C:\workspace\map2model\m2m_cpp\map2model.exe","Parfile"])

# Clipping data to region of interest
geology = geopandas.read_file(geology_file,bbox=bboxGDAZ50)
geology.columns = geology.columns.str.lower()
geology.crs = zone
structure = geopandas.read_file(structure_file,bbox=bboxGDAZ50)
structure.columns = structure.columns.str.lower()
structure.crs = zone
faults = geopandas.read_file(fault_file,bbox=bboxGDAZ50)
faults.columns = faults.columns.str.lower()
faults.crs = zone

geologyExp = m2l_utils.explode(geology)
geologyExp.crs = zone
geologyExpClip = m2l_utils.clip_shp(geologyExp,geoframe)
geologyExpClip.to_file(data_dir+"/tmp/geol_clip.shp")

sub_structure = structure[['geometry',"dip","dip_dir","feature"]]
print(sub_structure)
structureJoined = geopandas.sjoin(sub_structure,geologyExp,how="left", op="within")

is_bed = structureJoined["feature"].str.contains("Bedding")
bedding = structureJoined[is_bed]
print("Elements with 'Bedding' in feature",bedding.size)
beddingClip = m2l_utils.clip_shp(bedding,geoframe)
beddingClip.to_file(data_dir+"/tmp/structure_clip.shp")

folds = faults[faults["feature"].str.contains("Fold")]
print("Elements with 'Fold' in feature",folds.size)
foldsClip = m2l_utils.clip_shp(folds,geoframe)
foldsClip.to_file(data_dir+"/tmp/folds_clip.shp")

faultsList = faults[faults["feature"].str.contains("Fault")]
print("Elements with 'Fault' in feature",faultsList.size)
faultsClip = m2l_utils.clip_shp(faultsList,geoframe)
faultsClip.to_file(data_dir+"/tmp/faults_clip.shp")

# Parsing stratigraphy network for data
groups, glabels, G = m2l_topology.get_series(data_dir+"/graph/graph_strat.gml",'id')
m2l_topology.save_units(G,data_dir+"/tmp/",glabels,showPlot=False)
m2l_topology.save_group(G,data_dir+"/tmp/",glabels,geologyExpClip,c_l,showPlot=False)

# Saving orientation data to output
# important for loopStructural as save_orientations makes (orientations.csv)
dtm = rasterio.open(data_dir+"/dtm_reprojected.tif")
m2l_geometry.save_orientations(structureJoined,data_dir+"/output",c_l,orientation_decimate,dtm)
m2l_geometry.create_orientations(data_dir+"/tmp/",data_dir+"/output",
  dtm,geologyExpClip,beddingClip,c_l)

# Examining fault network
# 0 is intrusion_mode
ls_dict, ls_dict_decimate= m2l_geometry.save_basal_contacts(data_dir+"/tmp/",dtm,
  geologyExpClip,contact_decimate,c_l,0)
m2l_geometry.save_basal_no_faults(data_dir+"/tmp/basal_contacts.shp",
  data_dir+"/tmp/faults_clip.shp",ls_dict,10,c_l,zone)
contacts = geopandas.read_file(data_dir+"/tmp/basal_contacts.shp")
m2l_geometry.save_basal_contacts_csv(contacts,data_dir+"/output/",dtm,contact_decimate,c_l)

# 5 is fault_decimate, 20 is min_fault_length, 90.0 is fault_dip
m2l_geometry.save_faults(data_dir+"/tmp/faults_clip.shp",data_dir+"/output/",dtm,
  c_l,fault_decimate,20,90.0)

# Need to to_lower and remove ' ' and '_' from code checks
use_gcode=('Hamersley Group',
  'Fortescue Group',
  'Wyloo Group',
  'Shingle Creek Group',
  'Turee Creek Group' )
use_gcode2=('Hamersley_Group',
  'Fortescue_Group',
  'Wyloo_Group',
  'Shingle_Creek_Group',
  'Turee_Creek_Group' )

# Note function uses hardcode file names (structure_clip.shp, faults_clip.shp, ...)
m2l_interpolation.process_fault_throw_and_near_orientations(data_dir+"/tmp/",
  data_dir+"/output/",data_dir+"/dtm_reprojected.tif",c_l,use_gcode,use_gcode2,
  zone,bboxGDAZ50,'scipy_rbf',showPlot=False)

m2l_geometry.process_plutons(data_dir+"/tmp/",data_dir+"/output/",geologyExpClip,True,
  dtm,'domes',str(45),contact_decimate,c_l)

m2l_interpolation.interpolate_orientations(data_dir+"/tmp/structure_clip.shp",
  data_dir+"/tmp/",bboxGDAZ50,c_l,use_gcode,scheme,50,50,False,showPlot=False)
m2l_interpolation.interpolate_contacts(data_dir+"/tmp/basal_contacts.shp",
  data_dir+"/tmp/",dtm,bboxGDAZ50,c_l,use_gcode2,scheme,50,50,False,showPlot=False)

lc=numpy.loadtxt(data_dir+"/tmp/interpolation_contacts_l.csv",skiprows=1,delimiter=',',dtype=float)
mc=numpy.loadtxt(data_dir+"/tmp/interpolation_contacts_m.csv",skiprows=1,delimiter=',',dtype=float)
lo=numpy.loadtxt(data_dir+"/tmp/interpolation_l.csv",skiprows=1,delimiter=',',dtype=float)
mo=numpy.loadtxt(data_dir+"/tmp/interpolation_m.csv",skiprows=1,delimiter=',',dtype=float)
no=numpy.loadtxt(data_dir+"/tmp/interpolation_n.csv",skiprows=1,delimiter=',',dtype=float)
xy=numpy.loadtxt(data_dir+"/tmp/interpolation_"+scheme+".csv",skiprows=1,delimiter=',',dtype=float)

m2l_interpolation.join_contacts_and_orientations(data_dir+"/tmp/combo.csv",
  data_dir+"/tmp/geol_clip.shp",data_dir+"/tmp/",data_dir+"/dtm_reprojected.tif",
  c_l,lo,mo,no,lc,mc,xy,zone,bboxGDAZ50,False)

## Fold axial trace points
# 5 is fold_decimate
m2l_geometry.save_fold_axial_traces(data_dir+"/tmp/folds_clip.shp",
  data_dir+"/output/",dtm,c_l,fold_decimate)

# 750 is fat_step, 20.0 is close_dip
m2l_geometry.save_fold_axial_traces_orientations(data_dir+"/tmp/folds_clip.shp",
  data_dir+"/output/",data_dir+"/tmp/",dtm,c_l,zone,5,750,20.0)

new_use_gcode = (
  'Hamersley_Group',
  'Fortescue_Group',
  'Wyloo_Group',
  'Shingle_Creek_Group',
  'Turee_Creek_Group',
  'A_mgn_PRK',
  'A_mgn_PMI')
inputs=('invented_orientations',
  'interpolated_orientations',
  'intrusive_orientations',
  'fat_orientations',
  'near_fault_orientations')

# True is use_interpolations, then use_fat(foldAxialTrace)
m2l_geometry.tidy_data(data_dir+"/output/",data_dir+"/tmp/",new_use_gcode,True,True,'domes',inputs)
m2l_topology.parse_fault_relationships(data_dir+"/graph/",data_dir+"/tmp/",data_dir+"/output/",showPlot=False)

# a change from earlier as formation thickness example is different
contact_decimate = 2 
m2l_interpolation.save_contact_vectors(data_dir+"/tmp/basal_contacts.shp",
  data_dir+"/tmp/",dtm,bboxGDAZ50,c_l,scheme,contact_decimate,showPlot=False)
# 5000 is buffer, 10000 is max_thickness_allowed
m2l_geometry.calc_thickness(data_dir+"/tmp/",data_dir+"/output/",5000,10000,c_l)
m2l_geometry.normalise_thickness(data_dir+"/output/")



################ Sending outputs to loop Project File #########################

# used to give a mean width of each formation
stratigraphicLayers = pandas.read_csv(data_dir+"/output/formation_thicknesses.csv")
thickness = {}
uniqueLayers = stratigraphicLayers['formation'].unique()
for f in uniqueLayers:
    thickness[f] = numpy.mean(stratigraphicLayers[stratigraphicLayers['formation']==f]['thickness'])
stratigraphicLogData = numpy.zeros(uniqueLayers.shape[0],LoopProjectFile.stratigraphicLayerType)
stratigraphicLogData['layerId'] = range(uniqueLayers.shape[0])
stratigraphicLogData['layerId'] += 1
stratigraphicLogData['minAge'] = range(uniqueLayers.shape[0])
stratigraphicLogData['maxAge'] = range(uniqueLayers.shape[0])
stratigraphicLogData['maxAge'] += 0.5
stratigraphicLogData['formation'] = uniqueLayers
stratigraphicLogData['thickness'] = list(thickness.values())
resp = LoopProjectFile.Set(loopFilename,"stratigraphicLog",data=stratigraphicLogData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

faults = pandas.read_csv(data_dir+"/output/fault_orientations.csv")
faultEvents = numpy.zeros(faults.shape[0],LoopProjectFile.faultEventType)
faultEvents['name'] = faults['formation']  # The fault eventId is called formation for some reason
faultEvents['enabled'] = 0
faultEvents['minAge'] = numpy.arange(1.0,7.0, 6.0/faults.shape[0])
faultEvents['maxAge'] = faultEvents['minAge']
faultEvents['avgDisplacement'] = 0
for i in range(faultEvents.size):
    faults.loc[i,'formation'] = re.sub('.*_','',faults['formation'][i])
tmp = faults['formation']
faultEvents['eventId'] = tmp 

tmp = faultsClip[~faultsClip['fname'].isnull()]
lookup = dict(zip(numpy.array(tmp['objectid']),numpy.array(tmp['fname'])))

def replaceName(row,lookup):
    key = int(row['eventId'])
    if key in lookup:
        val = lookup[key]
        row['name'] = val
        row['enabled'] = 1
    else:
        pass

for row in faultEvents:
    replaceName(row,lookup)

resp = LoopProjectFile.Set(loopFilename,"faultLog",data=faultEvents,verbose=False)
if resp["errorFlag"]: print(resp["errorString"])

faultsData = numpy.zeros(faults.shape[0],LoopProjectFile.faultObservationType)
faultsData['eventId'] = faults['formation']
faultsData['easting'] = faults['X']
faultsData['northing'] = faults['Y']
faultsData['altitude'] = faults['Z']
faultsData['dipDir'] = faults['DipDirection']
faultsData['dip'] = faults['dip']
faultsData['dipPolarity'] = faults['DipPolarity']
resp = LoopProjectFile.Set(loopFilename,"faultObservations",data=faultsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

# each contact contains a location and which formation it is on
contacts = pandas.read_csv(data_dir+"/output/contacts4.csv")
layerIds = []
for form in contacts['formation']:
    a = bytes(form,'ascii')
    if a in stratigraphicLogData['formation']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['formation']==a]['layerId']))
    else:
        layerIds.append(0)
contactsData = numpy.zeros(contacts.shape[0],LoopProjectFile.contactObservationType)
contactsData['layerId'] = layerIds
contactsData['easting'] = contacts['X']
contactsData['northing'] = contacts['Y']
contactsData['altitude'] = contacts['Z']
resp = LoopProjectFile.Set(loopFilename,"contacts",data=contactsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

observations = pandas.read_csv(data_dir+"/output/orientations.csv")
layerIds = []
for form in observations['formation']:
    a = bytes(form,'ascii')
    if a in stratigraphicLogData['formation']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['formation']==a]['layerId']))
    else:
        layerIds.append(0)
observations['layer'] = "s0"
observationsData = numpy.zeros(observations.shape[0],LoopProjectFile.stratigraphicObservationType)
observationsData['layerId'] = layerIds
observationsData['easting'] = observations['X']
observationsData['northing'] = observations['Y']
observationsData['altitude'] = observations['Z']
observationsData['dipDir'] = observations['azimuth']
observationsData['dip'] = observations['dip']
observationsData['dipPolarity'] = observations['polarity']
observationsData['layer'] = observations['layer']
resp = LoopProjectFile.Set(loopFilename,"stratigraphicObservations",data=observationsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])



import winsound
duration = 700  # milliseconds
freq = 1100  # Hz
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
winsound.Beep(freq, duration)
