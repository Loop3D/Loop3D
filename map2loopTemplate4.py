from map2loop.project import Project
import LoopProjectFile
# from threading import Timer
import time

# Accept loopFilename from python state otherwise default
if ('loopFilename' not in vars() and 'loopFilename' not in globals()):
    loopFilename = "testing.loop3d"

data_dir = "gibberish"

# TODO: Add utm/gda zone when that can happen
# Collect extents from loopFilename
resp = LoopProjectFile.Get(loopFilename,"extents")
if resp["errorFlag"]:
    boundaries = [-334463.74,-292119.62,7713337.07,7739773.24,-1200.0,12000.0] 
    print(resp["errorString"])
else:
    boundaries = resp["value"]["utm"][2:] + resp["value"]["depth"]

def swap(list2):
    return [min(list2),max(list2)]

# boundaries = swap(boundaries[0:2]) + swap(boundaries[2:4]) + swap(boundaries[4:6])

bbox = {"minx":boundaries[0], "maxx":boundaries[1],"miny":boundaries[2],"maxy":boundaries[3],"base":boundaries[5],"top":boundaries[4]}
bbox2=str(bbox["minx"])+","+str(bbox["miny"])+","+str(bbox["maxx"])+","+str(bbox["maxy"])

print(bbox2)
structure_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=outcrops_28355&bbox='+bbox2+'&srsName=EPSG:28355'
geology_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_geol_dissolved_join_fix_mii_clip_wgs84&bbox='+bbox2+'&srsName=EPSG:28355'
mindep_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_mindeps_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fault_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'
fold_url='http://geo.loop-gis.org/geoserver/loop/wfs?service=WFS&version=1.1.0&request=GetFeature&typeName=qld_faults_folds_28355&bbox='+bbox2+'&srsName=EPSG:28355'
# strat_graph_file='source_data/graph_strat_NONE.gml'
# Temp override
geology_url='qld_tmp_data/qld_geol_asud.shp'
metadata="qld_tmp_data/qld_meta.hjson"

# geology_url = "data/hams_test.shp"
# fault_url="data/GEOS_GEOLOGY_LINEARSTRUCTURE_500K_GSD.shp"
# fold_url="data/"
# structure_url="data/hams2_structure.shp"
# mindep_url="data/mindeps_2018.shp"

# Override temporarily

start = time.time()
proj = Project(state="QLD",
               remote=True,
               geology_file=geology_url,
               fault_file=fault_url,
               fold_file=fold_url,
               structure_file=structure_url,
               mindep_file=mindep_url,
               metadata=metadata
               )
postInit = time.time()               
print("MAP2LOOP Project init took " + str(postInit-start) + " seconds")
proj.update_config(
                out_dir=data_dir,
                bbox_3d=bbox,
                step_out=0.1,
                dtm_crs="EPSG:4326",
                proj_crs="EPSG:28355",
                quiet=True
                )
postConfig = time.time()
print("MAP2LOOP Project config took " + str(postConfig-postInit) + " seconds")
# proj.plot()
proj.run()
postRun = time.time()
print("MAP2LOOP Project run took " + str(postRun-postConfig) + " seconds")

print("total map2loop time is " + str(time.time()-start))
# Extract data from pandas/geopandas and put into project file
# resp = LoopProjectFile.Set(loopFilename,"faults",faults)
# if resp["errorFlag"]: print(resp["errorString"])

# resp = LoopProjectFile.Set(loopFilename,"faultObservations",faultObservations)
# if resp["errorFlag"]: print(resp["errorString"])

# ...
################ Sending outputs to loop Project File #########################

import pandas
import numpy
import re

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
stratigraphicLogData['name'] = uniqueLayers
stratigraphicLogData['enabled'] = 1
stratigraphicLogData['rank'] = 0
stratigraphicLogData['type'] = 4
stratigraphicLogData['thickness'] = list(thickness.values())
resp = LoopProjectFile.Set(loopFilename,"stratigraphicLog",data=stratigraphicLogData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

faults = pandas.read_csv(data_dir+"/output/fault_orientations.csv")
faultEvents = numpy.zeros(faults.shape[0],LoopProjectFile.faultEventType)
faultEvents['name'] = faults['formation']  # The fault eventId is called formation for some reason
faultEvents['enabled'] = 0
faultEvents['rank'] = 0
faultEvents['type'] = 0
faultEvents['minAge'] = numpy.arange(1.0,7.0, 6.0/faults.shape[0])
faultEvents['maxAge'] = faultEvents['minAge']
faultEvents['avgDisplacement'] = 0
for i in range(faultEvents.size):
    faults.loc[i,'formation'] = re.sub('.*_','',faults['formation'][i])
tmp = faults['formation']
faultEvents['eventId'] = tmp 

# tmp = faults_clip[~faults_clip['fname'].isnull()]
# lookup = dict(zip(numpy.array(tmp['objectid']),numpy.array(tmp['fname'])))

# def replaceName(row,lookup):
#     key = int(row['eventId'])
#     if key in lookup:
#         val = lookup[key]
#         row['name'] = val
#         row['enabled'] = 1
#     else:
#         pass

# for row in faultEvents:
#     replaceName(row,lookup)

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
    if a in stratigraphicLogData['name']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['name']==a]['layerId']))
    else:
        layerIds.append(0)
contactsData = numpy.zeros(contacts.shape[0],LoopProjectFile.contactObservationType)
contactsData['layerId'] = layerIds
contactsData['easting'] = contacts['X']
contactsData['northing'] = contacts['Y']
contactsData['altitude'] = contacts['Z']
# contactsData['dipdir'] = contacts['']
# contactsData['dip'] = contacts['']
resp = LoopProjectFile.Set(loopFilename,"contacts",data=contactsData,verbose=True)
if resp["errorFlag"]: print(resp["errorString"])

observations = pandas.read_csv(data_dir+"/output/orientations.csv")
layerIds = []
for form in observations['formation']:
    a = bytes(form,'ascii')
    if a in stratigraphicLogData['name']:
        layerIds.append(int(stratigraphicLogData[stratigraphicLogData['name']==a]['layerId']))
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
